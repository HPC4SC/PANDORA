/*
 * Copyright ( c ) 2014
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACI-N
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software;
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or ( at your option ) any later version.
 *
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <MPIAutoAdjustment.hxx>

#include <MpiFactory.hxx>
#include <Exception.hxx>

#include <string.h>
#include <numeric>
#include <cfloat>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

namespace Engine {

    /** PUBLIC METHODS **/

    MPIAutoAdjustment::MPIAutoAdjustment()
    {
        uint64_t ts0, ts1;
        ts0 = getTicks();
        sleep(1);
        ts1 = getTicks();

        _cpuTicks = ts1 - ts0;
    }

    MPIAutoAdjustment::~MPIAutoAdjustment()
    {
    }

    void MPIAutoAdjustment::initAutoAdjustment(MPIMultiNode& schedulerInstance)
    {
        _schedulerInstance = &schedulerInstance;
    }

    /** PROTECTED METHODS **/

    uint64_t MPIAutoAdjustment::getTicks(void)
    {
        unsigned int lo, hi;
        __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
        return ((uint64_t) hi << 32) | lo;
    }

    void MPIAutoAdjustment::sendDataRequestToNode(const void* data, const int& numberOfElements, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag, const MPI_Comm& mpiComm)
    {
        MPI_Request* mpiRequest = new MPI_Request;

        MPI_Isend(data, numberOfElements, mpiDatatype, destinationNode, tag, mpiComm, mpiRequest);
        _sendRequests.push_back(mpiRequest);
    }

    void MPIAutoAdjustment::sendSignalToAllWorkingNodes(const int& signal, const int& messageTag)
    {
        int signal_nonConst = signal;

        for (int workingNodeID = 0; workingNodeID < _schedulerInstance->_numberOfActiveProcesses; ++workingNodeID)
        {
            if (workingNodeID != _schedulerInstance->_masterNodeID)
                sendDataRequestToNode(&signal_nonConst, 1, MPI_INT, workingNodeID, messageTag, _schedulerInstance->_activeProcessesComm);
        }
    }

    double MPIAutoAdjustment::getAgentPhasesTotalTime() const
    {
        return  _schedulerInstance->_world->getUpdateKnowledgeTotalTime() + 
                _schedulerInstance->_world->getSelectActionsTotalTime() + 
                _schedulerInstance->_world->getExecuteActionsTotalTime() + 
                _schedulerInstance->_world->getUpdateStateTotalTime();
    }

    bool MPIAutoAdjustment::validParametersForRebalancing() const
    {
        if (_schedulerInstance->_world->getConfig().getMaximumLoadPerNode() < 0 or
            _schedulerInstance->_world->getConfig().getMaximumPercOfUnbalance() < 0) 
                return false;
        return true;
    }

    bool MPIAutoAdjustment::needToRebalanceByLoad(const std::vector<double>& nodesTime) const
    {
        double maximumLoadPerNode = _schedulerInstance->_world->getConfig().getMaximumLoadPerNode();
        double maximumPercOfUnbalance = _schedulerInstance->_world->getConfig().getMaximumPercOfUnbalance();

        double lastNodeTime = nodesTime[nodesTime.size() - 1];

        if (lastNodeTime >= maximumLoadPerNode) return true;

        for (int j = 0; j < nodesTime.size() - 1; ++j)
        {
            double lastNodeComparison = 100.0 * (std::abs(1.0 - (lastNodeTime/nodesTime[j])));
            
            if (lastNodeComparison > maximumPercOfUnbalance)
                return true;
        }
        return false;
    }

    bool MPIAutoAdjustment::needToRebalance(double& agentPhasesAVGTime)
    {
        if (not validParametersForRebalancing()) 
        {
            sendSignalToAllWorkingNodes(eMessage_SendAgentPhasesTotalTime_false, eSendAgentPhasesTotalTime);
            return false;
        }

        bool needToRebalance = false;

        double masterAgentPhasesTotalTime = getAgentPhasesTotalTime();

        std::vector<double> nodesTime;
        nodesTime.push_back(masterAgentPhasesTotalTime);

        bool unbalancedByLoad = false;
        if (needToRebalanceByLoad(nodesTime))
        {
            sendSignalToAllWorkingNodes(eMessage_SendAgentPhasesTotalTime_false, eSendAgentPhasesTotalTime);
            needToRebalance = unbalancedByLoad = true;
        }
        else 
        {
            sendSignalToAllWorkingNodes(eMessage_SendAgentPhasesTotalTime_true, eSendAgentPhasesTotalTime);
        }

        if (not unbalancedByLoad)
        {
            for (int processID = 0; processID < _schedulerInstance->_numberOfActiveProcesses; ++processID)
            {
                if (processID != _schedulerInstance->_masterNodeID)
                {
                    double nodeAgentPhasesTotalTime;
                    MPI_Recv(&nodeAgentPhasesTotalTime, 1, MPI_DOUBLE, processID, eAgentPhasesTotalTime, _schedulerInstance->_activeProcessesComm, MPI_STATUS_IGNORE);

                    nodesTime.push_back(nodeAgentPhasesTotalTime);
                    if (not needToRebalance and needToRebalanceByLoad(nodesTime))
                        needToRebalance = true;
                }
            }
        }

        if (not nodesTime.empty()) 
        {
            agentPhasesAVGTime = 0.0;
            for (int i = 0; i < nodesTime.size(); ++i)
                agentPhasesAVGTime += nodesTime[i];
            agentPhasesAVGTime /= nodesTime.size();
        }

        return true;
        //return needToRebalance;
    }

    void MPIAutoAdjustment::receiveAllAgentsFromWorkingNodes()
    {
        for (int workingNodeID = 0; workingNodeID < _schedulerInstance->_numberOfActiveProcesses; ++workingNodeID)
        {
            if (workingNodeID != _schedulerInstance->_masterNodeID)
            {
                int numberOfTypes;
                MPI_Recv(&numberOfTypes, 1, MPI_INT, workingNodeID, eNumTypes, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int i = 0; i < numberOfTypes; ++i)
                {
                    int agentsTypeID;
                    MPI_Recv(&agentsTypeID, 1, MPI_INT, workingNodeID, eAgentsTypeID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    std::string agentsTypeName = MpiFactory::instance()->getNameFromTypeID(agentsTypeID);
                    MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentsTypeName);

                    int numberOfAgentsToReceive;
                    MPI_Recv(&numberOfAgentsToReceive, 1, MPI_INT, workingNodeID, eNumAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    int sizeOfAgentsPackage = MpiFactory::instance()->getSizeOfPackage(agentsTypeName);
                    void* agentsPackageArray = malloc(numberOfAgentsToReceive * sizeOfAgentsPackage);

                    MPI_Recv(agentsPackageArray, numberOfAgentsToReceive, *agentTypeMPI, workingNodeID, eAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    for (int j = 0; j < numberOfAgentsToReceive; ++j)
                    {
                        void* package = (char*) agentsPackageArray + j * sizeOfAgentsPackage;
                        Agent* agent = MpiFactory::instance()->createAndFillAgent(agentsTypeName, package);

                        if (not _schedulerInstance->_nodeSpace.ownedAreaWithOuterOverlap.contains(agent->getPosition()))
                                _schedulerInstance->_world->addAgent(agent, false);
                    }
                    //free(agentsPackageArray);
                }
            }
        }
    }

    // ToDo Makes this method to return a boolean.
    double MPIAutoAdjustment::estimateTotalCost(const int& numberOfProcessesToEstimate, const double& executingAgentsEstimatedTime, bool& isPartitioningSuitable)
    {
        isPartitioningSuitable = true;

        if (numberOfProcessesToEstimate < 1 or numberOfProcessesToEstimate > _schedulerInstance->getNumTasksMax()) return DBL_MAX;

std::cout << "numberOfProcessesToEstimate: " << numberOfProcessesToEstimate << " PERFORMING DIVIDE TEST...\n";

        double executingAgentsEstimatedTime_normalized = executingAgentsEstimatedTime / _cpuTicks;


        // ToDo: the performDivideTest() method must return a boolean stating whether the division into numberOfProcessesToEstimate is really feasible or not (using arePartitionsSuitable() somehow). Then, the numberOfOverlappingCells is a parameter by ref.


        int numberOfOverlappingCells;
        isPartitioningSuitable = _schedulerInstance->performDivideTest(numberOfProcessesToEstimate, numberOfOverlappingCells);
        if (not isPartitioningSuitable)
            std::cout << "[PROCESS " << _schedulerInstance->getId() << "] ================================================================================= isPartitioningSuitable FALSE\tnumberOfProcessesToEstimate: " << numberOfProcessesToEstimate << "\n";

        double numberOfOverlappingCells_normalized = (double) numberOfOverlappingCells / _cpuTicks;
std::cout << "DIVIDE TEST COMPLETED. executingAgentsEstimatedTime_normalized: " << executingAgentsEstimatedTime_normalized << "\tnumberOfOverlappingCells_normalized:" << numberOfOverlappingCells_normalized << "\t_cpuTicks: " << _cpuTicks << "\n";
        return executingAgentsEstimatedTime_normalized + (double) numberOfOverlappingCells_normalized;
    }

    int MPIAutoAdjustment::exploreUp(const int& initialNumberOfProcesses, const double& totalCost, const double& executingAgentsEstimatedTime)
    {
        int numberOfProcessesAtMinimumCost = initialNumberOfProcesses;

        bool localMinimumFound = false;
        while (not localMinimumFound)
        {
            int doubleProcesses = numberOfProcessesAtMinimumCost * 2.0;
            double estimatedTime = executingAgentsEstimatedTime / 2.0;

            bool isPartitioningSuitable;
            double nextCost = estimateTotalCost(doubleProcesses, estimatedTime, isPartitioningSuitable);
            if (isPartitioningSuitable and nextCost < totalCost)
                numberOfProcessesAtMinimumCost = doubleProcesses;
            else
                localMinimumFound = true;
        }

        return numberOfProcessesAtMinimumCost;
    }

    int MPIAutoAdjustment::exploreDown(const int& initialNumberOfProcesses, const double& totalCost, const double& executingAgentsEstimatedTime)
    {
        int numberOfProcessesAtMinimumCost = initialNumberOfProcesses;

        bool localMinimumFound = false;
        while (not localMinimumFound)
        {
            int halfProcesses = numberOfProcessesAtMinimumCost / 2.0;
            double estimatedTime = executingAgentsEstimatedTime * 2.0;

            bool isPartitioningSuitable;
            double nextCost = estimateTotalCost(halfProcesses, estimatedTime, isPartitioningSuitable);
            if (isPartitioningSuitable and nextCost < totalCost)
                numberOfProcessesAtMinimumCost = halfProcesses;
            else
                localMinimumFound = true;
        }

        return numberOfProcessesAtMinimumCost;
    }

    bool MPIAutoAdjustment::exploreMinimumCost(const double& agentPhasesAVGTime, int& numberOfProcessesAtMinimumCost)
    {
        int currentNumberOfProcesses = _schedulerInstance->_numberOfActiveProcesses;
        int doubleNumberOfProcesses = currentNumberOfProcesses * 2;
        int halfNumberOfProcesses = currentNumberOfProcesses / 2;

        double doubleProcessesEstimatedTime = agentPhasesAVGTime / 2.0;
        double halfProcessesEstimatedTime = agentPhasesAVGTime * 2.0;
        
        bool currentIsSuitable, doubleIsSuitable, halfIsSuitable;
        double currentCost = estimateTotalCost(currentNumberOfProcesses, agentPhasesAVGTime, currentIsSuitable);
        double doubleProcessesCost = estimateTotalCost(doubleNumberOfProcesses, doubleProcessesEstimatedTime, doubleIsSuitable);
        double halfProcessesCost = estimateTotalCost(halfNumberOfProcesses, halfProcessesEstimatedTime, halfIsSuitable);

std::cout << "currentNumberOfProcesses: " << currentNumberOfProcesses << "\tdoubleNumberOfProcesses: " << doubleNumberOfProcesses << "\thalfNumberOfProcesses: " << halfNumberOfProcesses << "\n";
std::cout << "currentCost: " << currentCost << "\tdoubleProcessesCost: " << doubleProcessesCost << "\thalfProcessesCost: " << halfProcessesCost << "\n";
std::cout << "currentIsSuitable: " << currentIsSuitable << "\tdoubleIsSuitable: " << doubleIsSuitable << "\thalfIsSuitable: " << halfIsSuitable << "\n";

        bool isRebalanceSuitable = false;
        if (doubleIsSuitable and doubleProcessesCost < currentCost and doubleProcessesCost < halfProcessesCost)
        {
            numberOfProcessesAtMinimumCost = exploreUp(doubleNumberOfProcesses, doubleProcessesCost, doubleProcessesEstimatedTime);
            isRebalanceSuitable = true;
        }
        else if (halfIsSuitable and halfProcessesCost < currentCost and halfProcessesCost < doubleProcessesCost)
        {
            numberOfProcessesAtMinimumCost = exploreDown(halfNumberOfProcesses, halfProcessesCost, halfProcessesEstimatedTime);
            isRebalanceSuitable = true;
        }
        else if (currentIsSuitable)
        {
            numberOfProcessesAtMinimumCost = currentNumberOfProcesses;
            isRebalanceSuitable = true;
        }

        if (_schedulerInstance->_world->getCurrentStep() == 3) numberOfProcessesAtMinimumCost = 2;
        else if (_schedulerInstance->_world->getCurrentStep() == 6) numberOfProcessesAtMinimumCost = 4;
std::cout << "¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?¿? isRebalanceSuitable: " << isRebalanceSuitable << "\tnumberOfProcessesAtMinimumCost: " << numberOfProcessesAtMinimumCost << "\n";
        return isRebalanceSuitable;
    }

    void MPIAutoAdjustment::sendNumberOfProcessesToWorkingNodes(const int& newNumberOfProcesses)
    {
        int newNumberOfProcesses_local = newNumberOfProcesses;

        for (int processID = 0; processID < _schedulerInstance->_numberOfActiveProcesses; ++processID)
        {
            if (processID != _schedulerInstance->_masterNodeID)
                sendDataRequestToNode(&newNumberOfProcesses_local, 1, MPI_INT, processID, eNumProcesses, _schedulerInstance->_activeProcessesComm);
        }
    }

    void MPIAutoAdjustment::awakeNodesIfNecessary(const int& numberOfRequestedProcesses)
    {
        if (numberOfRequestedProcesses > _schedulerInstance->_numberOfActiveProcesses)
        {
            if (_schedulerInstance->getId() == _schedulerInstance->_masterNodeID)
            {
                for (int processID = _schedulerInstance->_numberOfActiveProcesses; processID < numberOfRequestedProcesses; ++processID)
                {
                    int eventType = eMessage_AwakeToRepartition;
                    sendDataRequestToNode(&eventType, 1, MPI_INT, processID, eTypeOfEventAfterWakeUp, MPI_COMM_WORLD);

                    int newNumberOfProcessesAndCurrentStep[2];
                    newNumberOfProcessesAndCurrentStep[0] = numberOfRequestedProcesses;
                    newNumberOfProcessesAndCurrentStep[1] = _schedulerInstance->_world->getCurrentStep();
                    sendDataRequestToNode(newNumberOfProcessesAndCurrentStep, 2, MPI_INT, processID, eNumberOfProcessesAndStep, MPI_COMM_WORLD);
                }
            }
        }
    }

    void MPIAutoAdjustment::doMasterForRebalance(bool& neededToRebalance, int& newNumberOfProcesses)
    {
        neededToRebalance = false;
        newNumberOfProcesses = -1;

        if (needToRebalance(_currentAgentPhasesAVGTime))
        {
            sendSignalToAllWorkingNodes(eMessage_CheckToRepartition_true, eCheckToRepartition);
            receiveAllAgentsFromWorkingNodes();

            bool isRebalanceSuitable = exploreMinimumCost(_currentAgentPhasesAVGTime, newNumberOfProcesses);
            if (isRebalanceSuitable)
            {
                neededToRebalance = true;

                sendSignalToAllWorkingNodes(eMessage_PrepareToRepartition_true, ePrepareToRepartition);

                sendNumberOfProcessesToWorkingNodes(newNumberOfProcesses);
                awakeNodesIfNecessary(newNumberOfProcesses);
            }
            else
                sendSignalToAllWorkingNodes(eMessage_PrepareToRepartition_false, ePrepareToRepartition);
        }
        else
            sendSignalToAllWorkingNodes(eMessage_CheckToRepartition_false, eCheckToRepartition);
    }

    int MPIAutoAdjustment::receiveSignalFromMasterWithTag(const int& messageTag) const
    {
        int eventType;
        MPI_Recv(&eventType, 1, MPI_INT, _schedulerInstance->_masterNodeID, messageTag, _schedulerInstance->_activeProcessesComm, MPI_STATUS_IGNORE);

        return eventType;
    }

    void MPIAutoAdjustment::waitForMasterNeedsToRebalance() 
    {
        int eventType = receiveSignalFromMasterWithTag(eSendAgentPhasesTotalTime);
        if (eventType == eMessage_SendAgentPhasesTotalTime_true)
        {
            double agentPhasesTotalTime = getAgentPhasesTotalTime();
            sendDataRequestToNode(&agentPhasesTotalTime, 1, MPI_DOUBLE, _schedulerInstance->_masterNodeID, eAgentPhasesTotalTime, _schedulerInstance->_activeProcessesComm);
        }
    }

    void MPIAutoAdjustment::getAllOwnedAreaAgentsByType(std::map<std::string, AgentsList>& agentsByType) const
    {
        for (AgentsMap::const_iterator it = _schedulerInstance->_world->beginAgents(); it != _schedulerInstance->_world->endAgents(); ++it)
        {
            AgentPtr agentPtr = it->second;
            Agent* agent = agentPtr.get();

            if (_schedulerInstance->_nodeSpace.ownedArea.contains(agent->getPosition()))
            {
                std::string agentType = agent->getType();
                agentsByType[agentType].push_back(agentPtr);
            }
        }
    }

    void MPIAutoAdjustment::sendAllAgentsToMasterNode()
    {
        std::map<std::string, AgentsList> agentsByType;
        getAllOwnedAreaAgentsByType(agentsByType);

        int numberOfTypes = agentsByType.size();
        sendDataRequestToNode(&numberOfTypes, 1, MPI_INT, _schedulerInstance->_masterNodeID, eNumTypes, MPI_COMM_WORLD);

        for (std::map<std::string, AgentsList>::const_iterator it = agentsByType.begin(); it != agentsByType.end(); ++it)
        {
            std::string agentsTypeName = it->first;
            AgentsList agentsToSend = it->second;

            int agentsTypeID = MpiFactory::instance()->getIDFromTypeName(agentsTypeName);
            sendDataRequestToNode(&agentsTypeID, 1, MPI_INT, _schedulerInstance->_masterNodeID, eAgentsTypeID, MPI_COMM_WORLD);

            int numberOfAgentsToSend = agentsToSend.size();
            sendDataRequestToNode(&numberOfAgentsToSend, 1, MPI_INT, _schedulerInstance->_masterNodeID, eNumAgents, MPI_COMM_WORLD);

            int sizeOfAgentsPackage = MpiFactory::instance()->getSizeOfPackage(agentsTypeName);
            void* agentsPackageArray = malloc(numberOfAgentsToSend * sizeOfAgentsPackage);

            int i = 0;
            MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentsTypeName);
            for (AgentsList::const_iterator itAgent = agentsToSend.begin(); itAgent != agentsToSend.end(); ++itAgent)
            {
                Agent* agent = itAgent->get();

                void* agentPackage = agent->fillPackage();
                memcpy((char*) agentsPackageArray + i * sizeOfAgentsPackage, agentPackage, sizeOfAgentsPackage);
                agent->freePackage(agentPackage);
                ++i;
            }
            sendDataRequestToNode(agentsPackageArray, numberOfAgentsToSend, *agentTypeMPI, _schedulerInstance->_masterNodeID, eAgents, MPI_COMM_WORLD);

            //free(agentsPackageArray);
        }
    }

    void MPIAutoAdjustment::receiveNumberOfProcessesFromMasterNode(int& newNumberOfProcesses)
    {
        MPI_Recv(&newNumberOfProcesses, 1, MPI_INT, _schedulerInstance->_masterNodeID, eNumProcesses, _schedulerInstance->_activeProcessesComm, MPI_STATUS_IGNORE);
    }

    void MPIAutoAdjustment::doWorkersForRebalance(bool& neededToRebalance, int& newNumberOfProcesses)
    {
        if (_schedulerInstance->hasBeenTaggedAsJustAwaken())
        {
            neededToRebalance = true;
            newNumberOfProcesses = _schedulerInstance->_numberOfActiveProcesses;

            _schedulerInstance->_world->setCurrentStep(_schedulerInstance->_world->getCurrentStep() + 1);
        }
        else
        {
            waitForMasterNeedsToRebalance();

            int eventType = receiveSignalFromMasterWithTag(eCheckToRepartition);
            if (eventType == eMessage_CheckToRepartition_true)
            {
                neededToRebalance = true;

                sendAllAgentsToMasterNode();

                eventType = receiveSignalFromMasterWithTag(ePrepareToRepartition);
                if (eventType == eMessage_PrepareToRepartition_true)
                    receiveNumberOfProcessesFromMasterNode(newNumberOfProcesses);
            }
        }
    }

    void MPIAutoAdjustment::saveSpaces(MPINodesMap& spaces)
    {
        // Copy all the simple types
        spaces = _schedulerInstance->_mpiNodesMapToSend;

        // Copy pointers
        for (MPINodesMap::const_iterator itMpiNodes = _schedulerInstance->_mpiNodesMapToSend.begin(); itMpiNodes != _schedulerInstance->_mpiNodesMapToSend.end(); ++itMpiNodes)
        {
            int nodeID = itMpiNodes->first;
            for (std::map<int, MPINode*>::const_iterator itNeighbours = itMpiNodes->second.neighbours.begin(); itNeighbours != itMpiNodes->second.neighbours.end(); ++itNeighbours)
            {
                int neighbourID = itNeighbours->first;
                MPINode* neighbourMPINode = new MPINode();
                *neighbourMPINode = *(itNeighbours->second);

                spaces.at(nodeID).neighbours.at(neighbourID) = neighbourMPINode;
            }
        }
    }

    void MPIAutoAdjustment::sendSpacesToAllNodes(const MPINodesMap& spaces, const int& numberOfProcessesToIntercommunicate)
    {
        int amountOfNodesToSend = spaces.size();

        MPIMultiNode::Coordinates coordinatesForSize;
        int sizeOfCoordinates = sizeof(coordinatesForSize);
        
        void* coordinatesArray = malloc(amountOfNodesToSend * sizeOfCoordinates);

        int i = 0;
        for (MPINodesMap::const_iterator it = spaces.begin(); it != spaces.end(); ++it)
        {
            Rectangle<int> nodeArea = it->second.ownedArea;

            MPIMultiNode::Coordinates* nodeAreaCoordinates = new MPIMultiNode::Coordinates();
            nodeAreaCoordinates->top = nodeArea.top();
            nodeAreaCoordinates->left = nodeArea.left();
            nodeAreaCoordinates->bottom = nodeArea.bottom();
            nodeAreaCoordinates->right = nodeArea.right();

            memcpy((char*) coordinatesArray + i * sizeOfCoordinates, nodeAreaCoordinates, sizeOfCoordinates);
            ++i;
        }

        for (int processID = 0; processID < numberOfProcessesToIntercommunicate; ++processID)
        {
            if (processID != _schedulerInstance->_masterNodeID)
            {
                sendDataRequestToNode(&amountOfNodesToSend, 1, MPI_INT, processID, eNumCoordinates, MPI_COMM_WORLD);
                sendDataRequestToNode(coordinatesArray, amountOfNodesToSend, *_schedulerInstance->_coordinatesDatatype, processID, eCoordinates, MPI_COMM_WORLD);
            }
        }
        //free(coordinatesArray);
    }

    void MPIAutoAdjustment::receiveSpacesFromMasterNode(MPINodesMap& spaces) const
    {
        int amountOfNodesToReceive;
        MPI_Recv(&amountOfNodesToReceive, 1, MPI_INT, _schedulerInstance->_masterNodeID, eNumCoordinates, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPIMultiNode::Coordinates coordinatesForSize;
        int sizeOfCoordinates = sizeof(coordinatesForSize);

        void* coordinatesArray = malloc(amountOfNodesToReceive * sizeOfCoordinates);
        MPI_Recv(coordinatesArray, amountOfNodesToReceive, *_schedulerInstance->_coordinatesDatatype, _schedulerInstance->_masterNodeID, eCoordinates, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int processID = 0; processID < amountOfNodesToReceive; ++processID)
        {
            void* package = (char*) coordinatesArray + processID * sizeOfCoordinates;
            MPIMultiNode::Coordinates* coordinates = (MPIMultiNode::Coordinates*) package;

            if (spaces.find(processID) == spaces.end()) 
            {
                MPINode* mpiNode = new MPINode;
                mpiNode->ownedArea = Rectangle<int>(coordinates->left, coordinates->top, coordinates->right, coordinates->bottom);
                spaces[processID] = *mpiNode;
            }
        }

        free(coordinatesArray);
    }

    void MPIAutoAdjustment::generateSpacesOverlapsAndNeighbours(MPINodesMap& spaces) const
    {
        for (MPINodesMap::iterator it = spaces.begin(); it != spaces.end(); ++it)
            _schedulerInstance->generateOverlapAreas(it->second);

        for (MPINodesMap::iterator it1 = spaces.begin(); it1 != spaces.end(); ++it1)
        {
            int node1ID = it1->first;

            for (MPINodesMap::iterator it2 = std::next(it1); it2 != spaces.end(); ++it2)
            {
                int node2ID = it2->first;

                Rectangle<int> rectangle1 = it1->second.ownedArea;
                Rectangle<int> rectangle2 = it2->second.ownedArea;

                if (_schedulerInstance->areTheyNeighbours(rectangle1, rectangle2))
                {
                    if (it1->second.neighbours.find(node2ID) == it1->second.neighbours.end())
                        it1->second.neighbours[node2ID] = new MPINode();
                    it1->second.neighbours.at(node2ID)->ownedArea = rectangle2;

                    if (it2->second.neighbours.find(node1ID) == it2->second.neighbours.end())
                        it2->second.neighbours[node1ID] = new MPINode();
                    it2->second.neighbours.at(node1ID)->ownedArea = rectangle1;
                }
            }
        }
    }

    void MPIAutoAdjustment::printSpaces(const MPINodesMap& spaces, const bool& oldType) const
    {
        std::stringstream ss;

        if (oldType) ss << "[Process ID: " << _schedulerInstance->getId() << "] oldSpaces:\n";
        else ss << "[Process ID: " << _schedulerInstance->getId() << "] newSpaces:\n";

        for (MPINodesMap::const_iterator it = spaces.begin(); it != spaces.end(); ++it)
        {
            ss << "node id: " << it->first << ":\n";
            ss << "\townedArea: " << it->second.ownedArea << "\n";
            ss << "\townedAreaWithOuterOverlap: " << it->second.ownedAreaWithOuterOverlap << "\n";
            ss << "\townedAreaWithoutInnerOverlap: " << it->second.ownedAreaWithoutInnerOverlap << "\n";
            ss << "\n"; 
        }
        std::cout << ss.str();
    }

    void MPIAutoAdjustment::removeNonBelongingAgentsToMPINode(const MPINodesMap& spaces)
    {
        if (_schedulerInstance->getId() < _schedulerInstance->_numberOfActiveProcesses)
        {
            std::vector<Agent*> agentsToRemove;
            MPINode mpiNode = spaces.at(_schedulerInstance->getId());

            for (AgentsMap::const_iterator itAgent = _schedulerInstance->_world->beginAgents(); itAgent != _schedulerInstance->_world->endAgents(); ++itAgent)
            {
                AgentPtr agentPtr = itAgent->second;
                Agent* agent = agentPtr.get();

                if (not mpiNode.ownedAreaWithOuterOverlap.contains(agent->getPosition()))
                    agentsToRemove.push_back(agent);
            }

            _schedulerInstance->removeAgentsInVector(agentsToRemove);
        }
    }

    void MPIAutoAdjustment::removeMasterNodeNoNNeededAgents(const MPINodesMap& oldSpaces)
    {
        if (_schedulerInstance->getId() == _schedulerInstance->_masterNodeID)
            removeNonBelongingAgentsToMPINode(oldSpaces);
    }

    void MPIAutoAdjustment::updateOwnStructures(const MPINodesMap& newSpaces)
    {
        if (_schedulerInstance->getId() < _schedulerInstance->_numberOfActiveProcesses)
        {
            _schedulerInstance->fillOwnStructures(newSpaces.at(_schedulerInstance->getId()));
        }
    }

    void MPIAutoAdjustment::initializeAgentsToSendMap(std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode, const int& totalNumberOfSendingNodes) const
    {
        for (int processID = 0; processID < totalNumberOfSendingNodes; ++processID)
        {
            if (processID != _schedulerInstance->getId())
                agentsByTypeAndNode[processID] = std::map<std::string, AgentsList>();
        }
    }

    std::set<int> MPIAutoAdjustment::getNodesContainingPosition(const Point2D<int>& position, const MPINodesMap& spaces) const
    {
        std::set<int> resultingSet;
        for (MPINodesMap::const_iterator it = spaces.begin(); it != spaces.end(); ++it)
        {
            int mpiNodeID = it->first;
            MPINode mpiNode = it->second;
            
            if (mpiNode.ownedAreaWithOuterOverlap.contains(position))
                resultingSet.insert(mpiNodeID);
        }
        return resultingSet;
    }

    void MPIAutoAdjustment::printAgentsByTypeAndNodeToSend(const std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode) const
    {
        std::stringstream ss;
        ss << "Node " << _schedulerInstance->getId() << " sending agents to:\n";
        for (std::map<int, std::map<std::string, AgentsList>>::const_iterator it = agentsByTypeAndNode.begin(); it != agentsByTypeAndNode.end(); ++it)
        {
            ss << "\tnode = " << it->first << ":\n";
            for (std::map<std::string, AgentsList>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                ss << "\t\t type = " << it2->first << ": ";
                for (AgentsList::const_iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) ss << it3->get()->getId() << ", ";
                ss << "\n";
            }
            ss << "\t\t";
            ss << "\n";
        }
        ss << "\n";
        std::cout << ss.str();
    }

    void MPIAutoAdjustment::sendAgentsInMap(const std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode)
    {
        double initialTime = _schedulerInstance->getWallTime();

        _schedulerInstance->sendAgentsInMap(agentsByTypeAndNode);

        double endTime = _schedulerInstance->getWallTime();
if (_schedulerInstance->_printInstrumentation) _schedulerInstance->_schedulerLogs->printInstrumentation("[Process # " + std::to_string(_schedulerInstance->getId()) + "] MPIAutoAdjustment::sendAgentsInMap() \tTOTAL TIME: " + std::to_string(endTime - initialTime));
    }

    void MPIAutoAdjustment::sendAgentsToOtherNodesIfNecessary(const MPINodesMap& newSpaces, const MPINodesMap& oldSpaces)
    {
        std::map<int, std::map<std::string, AgentsList>> agentsByTypeAndNode;
        initializeAgentsToSendMap(agentsByTypeAndNode, std::max(newSpaces.size(), oldSpaces.size()));

        for (AgentsMap::const_iterator itAgent = _schedulerInstance->_world->beginAgents(); itAgent != _schedulerInstance->_world->endAgents(); ++itAgent)
        {
            AgentPtr agentPtr = itAgent->second;
            Agent* agent = agentPtr.get();

            std::string agentType = agent->getType();

            std::set<int> newNodesContainingAgent = getNodesContainingPosition(agent->getPosition(), newSpaces);
            std::set<int> oldNodesContainingAgent = getNodesContainingPosition(agent->getPosition(), oldSpaces);

            for (std::set<int>::const_iterator it = newNodesContainingAgent.begin(); it != newNodesContainingAgent.end(); ++it)
            {
                int newNodeID = *it;

                if (newNodeID != _schedulerInstance->getId())
                {
                    if (oldNodesContainingAgent.find(newNodeID) == oldNodesContainingAgent.end())
                    {
                        if (agentsByTypeAndNode.at(newNodeID).find(agentType) == agentsByTypeAndNode.at(newNodeID).end()) 
                            agentsByTypeAndNode.at(newNodeID)[agentType] = AgentsList();

                        agentsByTypeAndNode.at(newNodeID).at(agentType).push_back(agentPtr);
                    }
                }
            }
        }

if (_schedulerInstance->_printInConsole) printAgentsByTypeAndNodeToSend(agentsByTypeAndNode);

        sendAgentsInMap(agentsByTypeAndNode);
    }

    void MPIAutoAdjustment::receiveAgentsFromOtherNodesIfNecessary(const int& numberOfNodesToReceiveFrom)
    {
        for (int sendingNodeID = 0; sendingNodeID < numberOfNodesToReceiveFrom; ++sendingNodeID)
        {
            if (sendingNodeID != _schedulerInstance->getId())
            {
                int numberOfAgentTypesToReceive;
                MPI_Recv(&numberOfAgentTypesToReceive, 1, MPI_INT, sendingNodeID, eNumGhostAgentsType, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int i = 0; i < numberOfAgentTypesToReceive; ++i)
                {
                    int agentTypeID;
                    MPI_Recv(&agentTypeID, 1, MPI_INT, sendingNodeID, eGhostAgentsType, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    std::string agentsTypeName = MpiFactory::instance()->getNameFromTypeID(agentTypeID);
                    MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentsTypeName);

                    int numberOfAgentsToReceive;
                    MPI_Recv(&numberOfAgentsToReceive, 1, MPI_INT, sendingNodeID, eNumGhostAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    int sizeOfAgentPackage = MpiFactory::instance()->getSizeOfPackage(agentsTypeName);

                    void* agentsPackageArray = malloc(numberOfAgentsToReceive * sizeOfAgentPackage);
                    MPI_Recv(agentsPackageArray, numberOfAgentsToReceive, *agentTypeMPI, sendingNodeID, eGhostAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    for (int j = 0; j < numberOfAgentsToReceive; ++j)
                    {
                        void* package = (char*) agentsPackageArray + j * sizeOfAgentPackage;
                        Agent* agent = MpiFactory::instance()->createAndFillAgent(agentsTypeName, package);

                        if (_schedulerInstance->_world->getAgentsMap().find(agent->getId()) == _schedulerInstance->_world->getAgentsMap().end())
                            _schedulerInstance->_world->addAgent(agent);
                    }
                }
            }
        }
    }

    void MPIAutoAdjustment::initializeRasterValuesToSendMap(std::map<int, MapOfValuesByRaster>& rastersValuesByNode, const int& totalNumberOfSendingNodes) const
    {
        for (int processID = 0; processID < totalNumberOfSendingNodes; ++processID)
        {
            if (processID != _schedulerInstance->getId())
                rastersValuesByNode[processID] = std::map<int, MapOfPositionsAndValues>();
        }
    }

    void MPIAutoAdjustment::sendRastersInMap(const std::map<int, MapOfValuesByRaster>& rastersValuesByNode)
    {
        double initialTime = _schedulerInstance->getWallTime();

        _schedulerInstance->sendRasterValuesInMap(rastersValuesByNode);

        double endTime = _schedulerInstance->getWallTime();
if (_schedulerInstance->_printInstrumentation) _schedulerInstance->_schedulerLogs->printInstrumentation("[Process # " + std::to_string(_schedulerInstance->getId()) +  "] MPIAutoAdjustment::sendRastersInMap() \tTOTAL TIME: " + std::to_string(endTime - initialTime));
    }

    void MPIAutoAdjustment::sendRastersToOtherNodesIfNecessary(const MPINodesMap& newSpaces, const MPINodesMap& oldSpaces)
    {
        std::map<int, MapOfValuesByRaster> rastersValuesByNode;
        initializeRasterValuesToSendMap(rastersValuesByNode, std::max(newSpaces.size(), oldSpaces.size()));

        if (not _schedulerInstance->_justAwaken)
        {
            for (int rasterIndex = 0; rasterIndex < _schedulerInstance->_world->getNumberOfRasters(); ++rasterIndex)
            {
                if (not _schedulerInstance->_world->rasterExists(rasterIndex) or not _schedulerInstance->_world->isRasterDynamic(rasterIndex))
                    continue;

                MPINode originalOwnSpace = oldSpaces.at(_schedulerInstance->getId());
                for (int i = originalOwnSpace.ownedAreaWithOuterOverlap.top(); i < originalOwnSpace.ownedAreaWithOuterOverlap.bottom() + 1; ++i)
                {
                    for (int j = originalOwnSpace.ownedAreaWithOuterOverlap.left(); j < originalOwnSpace.ownedAreaWithOuterOverlap.right() + 1; ++j)
                    {
                        Point2D<int> rasterCellPosition = Point2D<int>(j, i);
                        int rasterCellValue = _schedulerInstance->_world->getValue(rasterIndex, rasterCellPosition);

                        std::set<int> newNodesContainingRasterCell = getNodesContainingPosition(rasterCellPosition, newSpaces);
                        std::set<int> oldNodesContainingRasterCell = getNodesContainingPosition(rasterCellPosition, oldSpaces);

                        for (std::set<int>::const_iterator it = newNodesContainingRasterCell.begin(); it != newNodesContainingRasterCell.end(); ++it)
                        {
                            int newNodeID = *it;

                            if (newNodeID != _schedulerInstance->getId())
                            {
                                if (oldNodesContainingRasterCell.find(newNodeID) == oldNodesContainingRasterCell.end())
                                {
                                    if (rastersValuesByNode.at(newNodeID).find(rasterIndex) == rastersValuesByNode.at(newNodeID).end())
                                        rastersValuesByNode.at(newNodeID)[rasterIndex] = MapOfPositionsAndValues();

                                    rastersValuesByNode.at(newNodeID).at(rasterIndex)[rasterCellPosition] = rasterCellValue;
                                }
                            }
                        }
                    }
                }
            }
        }

        sendRastersInMap(rastersValuesByNode);
    }

    void MPIAutoAdjustment::receiveRastersFromOtherNodesIfNecessary(const int& numberOfNodesToReceiveFrom)
    {
        for (int sendingNodeID = 0; sendingNodeID < numberOfNodesToReceiveFrom; ++sendingNodeID)
        {
            if (sendingNodeID != _schedulerInstance->getId())
                _schedulerInstance->receiveRasterForOneNode(sendingNodeID);
        }
    }

    void MPIAutoAdjustment::removeNonBelongingRasterCellsToMPINode(const MPINodesMap& spaces)
    {
        // Not need to remove rasters outside the boundaries.
    }

    void MPIAutoAdjustment::rebalanceAgentsAndRastersAmongNodes(const int& newNumberOfProcesses)
    {
        MPINodesMap oldSpaces, newSpaces;

        if (_schedulerInstance->getId() == _schedulerInstance->_masterNodeID)
        {
            saveSpaces(oldSpaces);
            sendSpacesToAllNodes(oldSpaces, std::max((int) oldSpaces.size(), _schedulerInstance->_numberOfActiveProcesses));

            _schedulerInstance->resetPartitioning(newNumberOfProcesses);
            _schedulerInstance->divideSpace();

            saveSpaces(newSpaces);
            sendSpacesToAllNodes(newSpaces, std::max((int) oldSpaces.size(), _schedulerInstance->_numberOfActiveProcesses));
        }
        else
        {
            receiveSpacesFromMasterNode(oldSpaces);
            receiveSpacesFromMasterNode(newSpaces);
        }

        generateSpacesOverlapsAndNeighbours(oldSpaces);
        generateSpacesOverlapsAndNeighbours(newSpaces);

//if (_schedulerInstance->_printInConsole) { printSpaces(oldSpaces, true);    printSpaces(newSpaces, false); }

        removeMasterNodeNoNNeededAgents(oldSpaces);
        updateOwnStructures(newSpaces);

        sendAgentsToOtherNodesIfNecessary(newSpaces, oldSpaces);
        receiveAgentsFromOtherNodesIfNecessary(std::max(newSpaces.size(), oldSpaces.size()));

        removeNonBelongingAgentsToMPINode(newSpaces);

        sendRastersToOtherNodesIfNecessary(newSpaces, oldSpaces);
        receiveRastersFromOtherNodesIfNecessary(std::max(newSpaces.size(), oldSpaces.size()));

        removeNonBelongingRasterCellsToMPINode(newSpaces);
    }

    void MPIAutoAdjustment::writeStateAfterRebalanceInLog()
    {
        _schedulerInstance->_schedulerLogs->writeInDebugFile("STATE AFTER BEING REBALANCED AT THE END OF THE STEP " + std::to_string(_schedulerInstance->_world->getCurrentStep()) + ":");
        _schedulerInstance->printOwnNodeStructureAfterMPI();
        _schedulerInstance->printNodeAgents();
        _schedulerInstance->printNodeRasters();
    }

    void MPIAutoAdjustment::putNonNeededWorkersToSleep(const int& newNumberOfProcesses)
    {
        if (_schedulerInstance->getId() >= newNumberOfProcesses) 
            _schedulerInstance->waitSleeping(_schedulerInstance->_masterNodeID);
    }

    /** PUBLIC METHODS **/

    void MPIAutoAdjustment::checkForRebalancingSpace()
    {
        bool neededToRebalance = false;
        int newNumberOfProcesses = -1;

        if (_schedulerInstance->getId() == _schedulerInstance->_masterNodeID)
            doMasterForRebalance(neededToRebalance, newNumberOfProcesses);
        else
            doWorkersForRebalance(neededToRebalance, newNumberOfProcesses);
        
        _schedulerInstance->enableOnlyProcesses(newNumberOfProcesses);

        if (neededToRebalance) {
            rebalanceAgentsAndRastersAmongNodes(newNumberOfProcesses);
            writeStateAfterRebalanceInLog();
            putNonNeededWorkersToSleep(newNumberOfProcesses);
        }

        _schedulerInstance->_justAwaken = false;  // ??

        std::cout << "[PROCESS " << _schedulerInstance->getId() << "] JUST BEFORE THE LAST MPI_Barrier.\n";
        MPI_Barrier(_schedulerInstance->_activeProcessesComm);
        std::cout << "[PROCESS " << _schedulerInstance->getId() << "] JUST AFTER THE LAST MPI_Barrier.\n";
    }

} // namespace Engine