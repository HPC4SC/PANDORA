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

    void MPIAutoAdjustment::sendDataRequestToNode(void* data, const int& numberOfElements, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag, const MPI_Comm& mpiComm)
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

    int MPIAutoAdjustment::receiveSignalFromMasterWithTag(const int& messageTag) const
    {
        int eventType;
        MPI_Recv(&eventType, 1, MPI_INT, _schedulerInstance->_masterNodeID, messageTag, _schedulerInstance->_activeProcessesComm, MPI_STATUS_IGNORE);

        return eventType;
    }

    double MPIAutoAdjustment::getAgentPhasesTotalTime() const
    {
        return  _schedulerInstance->_world->getUpdateKnowledgeTotalTime() + 
                _schedulerInstance->_world->getSelectActionsTotalTime() + 
                _schedulerInstance->_world->getExecuteActionsTotalTime() + 
                _schedulerInstance->_world->getUpdateStateTotalTime();
    }

    bool MPIAutoAdjustment::needToRebalanceByLoad(const std::vector<double>& nodesTime) const
    {
        double maximumLoadPerNode = _schedulerInstance->_world->getConfig().getMaximumLoadPerNode();
        double maximumPercOfUnbalance = _schedulerInstance->_world->getConfig().getMaximumPercOfUnbalance();

        double lastNodeTime = nodesTime[nodesTime.size() - 1];

        if (lastNodeTime >= maximumLoadPerNode) return true;

        for (int j = 0; j < nodesTime.size() - 1; ++j)
        {
            if (100.0 * (std::abs(1.0 - (lastNodeTime/nodesTime[j]))) > maximumPercOfUnbalance)
                return true;
        }
        return false;
    }

    bool MPIAutoAdjustment::needToRebalance(double& agentPhasesAVGTime)
    {
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
            sendSignalToAllWorkingNodes(eMessage_SendAgentPhasesTotalTime_true, eSendAgentPhasesTotalTime);

        if (not unbalancedByLoad)
        {
            for (int processID = 0; processID < _schedulerInstance->_numberOfActiveProcesses; ++processID)
            {
                if (processID != _schedulerInstance->_masterNodeID)
                {
                    double nodeAgentPhasesTotalTime;
                    MPI_Recv(&nodeAgentPhasesTotalTime, 1, MPI_DOUBLE, processID, eAgentPhasesTotalTime, _schedulerInstance->_activeProcessesComm, MPI_STATUS_IGNORE);

                    nodesTime.push_back(nodeAgentPhasesTotalTime);
                    if (needToRebalanceByLoad(nodesTime))
                    {
                        needToRebalance = true;
                        break;
                    }
                }
            }
        }

        if (not nodesTime.empty()) 
        {
            agentPhasesAVGTime = 0.0;
            for (int i = 0; i < nodesTime.size(); ++i)
                agentPhasesAVGTime += nodesTime[i];
        }

        return needToRebalance;
    }

    int MPIAutoAdjustment::waitForMasterNeedsToRebalance() 
    {
        int eventType = receiveSignalFromMasterWithTag(eSendAgentPhasesTotalTime);
        if (eventType == eMessage_SendAgentPhasesTotalTime_true)
        {
            double agentPhasesTotalTime = getAgentPhasesTotalTime();
            MPI_Send(&agentPhasesTotalTime, 1, MPI_DOUBLE, _schedulerInstance->_masterNodeID, eAgentPhasesTotalTime, _schedulerInstance->_activeProcessesComm);
        }
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

                        if (not _schedulerInstance->_nodeSpace.ownedAreaWithOuterOverlaps.contains(agent->getPosition()))
                                _schedulerInstance->_world->addAgent(agent, false);
                    }
                    free(agentsPackageArray);
                }
            }
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

            free(agentsPackageArray);
        }
    }

    double MPIAutoAdjustment::estimateTotalCost(const int& numberOfProcessesToEstimate, const double& executingAgentsEstimatedTime)
    {
        if (numberOfProcessesToEstimate < 1 or numberOfProcessesToEstimate > _schedulerInstance->getNumTasksMax()) return DBL_MAX;

std::cout << CreateStringStream("estimateTotalCost: " << numberOfProcessesToEstimate << " PERFORMING DIVIDE TEST...\n").str();

        double executingAgentsEstimatedTime_normalized = executingAgentsEstimatedTime / _cpuTicks;

        int numberOfOverlappingCells = _schedulerInstance->performDivideTest(numberOfProcessesToEstimate);
        double numberOfOverlappingCells_normalized = (double) numberOfOverlappingCells / _cpuTicks;

std::cout << CreateStringStream("DIVIDE TEST COMPLETED. executingAgentsEstimatedTime_normalized: " << executingAgentsEstimatedTime_normalized << "\tnumberOfOverlappingCells_normalized:" << numberOfOverlappingCells_normalized << "\t_cpuTicks: " << _cpuTicks << "\n").str();
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

            double nextCost = estimateTotalCost(doubleProcesses, estimatedTime);
            if (nextCost < totalCost)
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

            double nextCost = estimateTotalCost(halfProcesses, estimatedTime);
            if (nextCost < totalCost)
                numberOfProcessesAtMinimumCost = halfProcesses;
            else
                localMinimumFound = true;
        }

        return numberOfProcessesAtMinimumCost;
    }

    int MPIAutoAdjustment::exploreMinimumCost()
    {
        int currentNumberOfProcesses = _schedulerInstance->_numberOfActiveProcesses;
        int doubleProcesses = currentNumberOfProcesses * 2;
        int halfProcesses = currentNumberOfProcesses / 2;

        double doubleProcessesEstimatedTime = _currentAgentPhasesAVGTime / 2.0;
        double halfProcessesEstimatedTime = _currentAgentPhasesAVGTime * 2.0;
        
        double currentCost = estimateTotalCost(currentNumberOfProcesses, _currentAgentPhasesAVGTime);
        double doubleProcessesCost = estimateTotalCost(doubleProcesses, doubleProcessesEstimatedTime);
        double halfProcessesCost = estimateTotalCost(halfProcesses, halfProcessesEstimatedTime);

        int numberOfProcessesAtMinimumCost = currentNumberOfProcesses;
        if (doubleProcessesCost < currentCost and doubleProcessesCost < halfProcessesCost)
            numberOfProcessesAtMinimumCost = exploreUp(doubleProcesses, doubleProcessesCost, doubleProcessesEstimatedTime);
        else if (halfProcessesCost < currentCost and halfProcessesCost < doubleProcessesCost)
            numberOfProcessesAtMinimumCost = exploreDown(halfProcesses, halfProcessesCost, halfProcessesEstimatedTime);
        
        return numberOfProcessesAtMinimumCost;
    }

    void MPIAutoAdjustment::sendNumberOfProcessesToWorkingNodes(const int& newNumberOfProcesses) const
    {
        int newNumberOfProcesses_local = newNumberOfProcesses;

        for (int processID = 0; processID < _schedulerInstance->_numberOfActiveProcesses; ++processID)
        {
            if (processID != _schedulerInstance->_masterNodeID)
                sendDataRequestToNode(newNumberOfProcesses, 1, MPI_INT, processID, eNumProcesses, _schedulerInstance->_activeProcessesComm);
        }
    }

    void MPIAutoAdjustment::receiveNumberOfProcessesFromMasterNode(int& newNumberOfProcesses)
    {
        MPI_Recv(&newNumberOfProcesses, 1, MPI_INT, _schedulerInstance->_masterNodeID, eNumProcesses, _schedulerInstance->_activeProcessesComm, MPI_STATUS_IGNORE);
    }

    void MPIAutoAdjustment::awakeOrPutToSleepNodesToRepartition(const int& numberOfRequestedProcesses)
    {
        if (numberOfRequestedProcesses > _schedulerInstance->_numberOfActiveProcesses)
        {
            for (int processID = _schedulerInstance->_numberOfActiveProcesses; processID < numberOfRequestedProcesses; ++processID)
            {
                int numberOfProcessesToAwake = numberOfRequestedProcesses;
                sendDataRequestToNode(&numberOfProcessesToAwake, 1, MPI_INT, processID, eProcessWakeUp, MPI_COMM_WORLD);

                int eventType = eMessage_AwakeToRepartition;
                sendDataRequestToNode(&eventType, 1, MPI_INT, processID, eTypeOfEventAfterWakeUp, MPI_COMM_WORLD);
            }
        }
        else
        {
            for (int processID = numberOfRequestedProcesses; processID < _schedulerInstance->_numberOfActiveProcesses; ++processID)
            {
                _schedulerInstance->waitSleeping(_schedulerInstance->_masterNodeID);
            }
        }
    }

    void MPIAutoAdjustment::sendAllNewSpacesToAllNodes()
    {
        MPIMultiNode::MPINodesMap mpiNodes = _schedulerInstance->_mpiNodesMapToSend;
        int amountOfNodesToSend = mpiNodes.size();

        MPIMultiNode::Coordinates coordinatesForSize;
        int sizeOfCoordinates = sizeof(coordinatesForSize);
        
        void* coordinatesArray = malloc(amountOfNodesToSend * sizeOfCoordinates);

        int i = 0;
        for (MPIMultiNode::MPINodesMap::const_iterator it = mpiNodes; it != mpiNodes; ++it)
        {
            int nodeID = it->first;
            Rectangle<int> nodeArea = it->second.ownedArea;

            MPIMultiNode::Coordinates nodeAreaCoordinates;
            nodeAreaCoordinates.top = nodeArea.top();
            nodeAreaCoordinates.left = nodeArea.left();
            nodeAreaCoordinates.bottom = nodeArea.bottom();
            nodeAreaCoordinates.right = nodeArea.right();

            memcpy((char*) coordinatesArray + i * sizeOfCoordinates, nodeAreaCoordinates, sizeOfCoordinates);
            ++i;
        }

        for (int processID = _schedulerInstance->_numberOfActiveProcesses; processID < _schedulerInstance->_numberOfActiveProcesses; ++processID)
        {
            sendDataRequestToNode(&amountOfNodesToSend, 1, MPI_INT, processID, eNumCoordinates, _schedulerInstance->_activeProcessesComm);
            sendDataRequestToNode(&coordinatesArray, amountOfNodesToSend, *_schedulerInstance->_coordinatesDatatype, processID, eCoordinates, _schedulerInstance->_activeProcessesComm);
        }
        free(coordinatesArray);
    }

    void MPIAutoAdjustment::receiveNewSpacesFromMasterNode()
    {
        int amountOfNodesToReceive;
        MPI_Recv(&amountOfNodesToReceive, 1, MPI_INT, _schedulerInstance->_masterNodeID, eNumCoordinates, _schedulerInstance->_activeProcessesComm, MPI_STATUS_IGNORE);
        
        MPIMultiNode::Coordinates coordinatesForSize;
        int sizeOfCoordinates = sizeof(coordinatesForSize);

        void* coordinatesArray = malloc(amountOfNodesToReceive * sizeOfCoordinates);
        MPI_Recv(coordinatesArray, amountOfNodesToReceive, *_schedulerInstance->_coordinatesDatatype, _schedulerInstance->_masterNodeID, eCoordinates, _schedulerInstance->_activeProcessesComm, MPI_STATUS_IGNORE);

        for (int i = 0; i < amountOfNodesToReceive; ++i)
        {
            void* package = (char*) coordinatesArray + i * sizeOfCoordinates;
            MPIMultiNode::Coordinates coordinates = (MPIMultiNode::Coordinates) *package;



        }
        free(coordinatesArray);
    }

    void MPIAutoAdjustment::rebalance(const int& newNumberOfProcesses)
    {
        if (_schedulerInstance->getId() == _schedulerInstance->_masterNodeID)
        {
            _schedulerInstance->resetPartitioning(newNumberOfProcesses);
            _schedulerInstance->divideSpace();

            // wake up nodes if necessary

            sendAllNewSpacesToAllNodes();


            _schedulerInstance->filterOutInitialAgents();
        }
        else
        {
            // remove all the agents and all the rasters info (just in case the new awaken nodes has some data from a lot of steps before)
            // set current step for new nodes (masters send it to the just awaken processes)

            receiveNewSpacesFromMasterNode();
        }
        
    }

    /** PUBLIC METHODS **/

    void MPIAutoAdjustment::checkForRebalancingSpace()
    {
        int newNumberOfProcesses;
        bool neededToRebalance = false;

        if (_schedulerInstance->getId() == _schedulerInstance->_masterNodeID)
        {
            neededToRebalance = needToRebalance(_currentAgentPhasesAVGTime);
            if (neededToRebalance)
            {
                sendSignalToAllWorkingNodes(eMessage_PrepareToRepartition_true, ePrepareToRepartition);
                receiveAllAgentsFromWorkingNodes();

                newNumberOfProcesses = exploreMinimumCost();
                sendNumberOfProcessesToWorkingNodes(newNumberOfProcesses);

                awakeOrPutToSleepNodesToRepartition(newNumberOfProcesses);
            }
            else sendSignalToAllWorkingNodes(eMessage_PrepareToRepartition_false, ePrepareToRepartition);
        }
        else
        {
            if (_schedulerInstance->hasBeenTaggedAsJustAwaken())
            {
                neededToRebalance = true;
                _schedulerInstance->_justAwaken = false;
            }
            else
            {
                waitForMasterNeedsToRebalance();

                int eventType = receiveSignalFromMasterWithTag(ePrepareToRepartition);
                if (eventType == eMessage_PrepareToRepartition_true)
                {
                    neededToRebalance = true;

                    sendAllAgentsToMasterNode();
                    receiveNumberOfProcessesFromMasterNode(newNumberOfProcesses);
                }
            }
        }

        MPI_Barrier(_schedulerInstance->_activeProcessesComm);

        if (neededToRebalance) rebalance(newNumberOfProcesses);

        std::cout << CreateStringStream("[PROCESS " << _schedulerInstance->getId() << "] JUST BEFORE THE LAST MPI_Barrier. \n").str();
        MPI_Barrier(_schedulerInstance->_activeProcessesComm);
    }

} // namespace Engine