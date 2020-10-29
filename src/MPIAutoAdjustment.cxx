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

namespace Engine {

    /** PUBLIC METHODS **/

    MPIAutoAdjustment::MPIAutoAdjustment()
    {
    }

    MPIAutoAdjustment::~MPIAutoAdjustment()
    {
    }

    void MPIAutoAdjustment::initAutoAdjustment(const MPIMultiNode& schedulerInstance)
    {
        _schedulerInstance = &schedulerInstance;
    }

    /** PROTECTED METHODS **/

    void MPIAutoAdjustment::sendDataRequestToNode(void* data, const int& numberOfElements, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag, const MPI_Comm& mpiComm)
    {
        MPI_Request* mpiRequest = new MPI_Request;

        MPI_Isend(data, numberOfElements, mpiDatatype, destinationNode, tag, mpiComm, mpiRequest);
        _sendRequests.push_back(mpiRequest);
    }

    void MPIAutoAdjustment::sendSignalsToBeginRebalance(const bool& beginRebalance)
    {
        int eventType = eMessage_PrepareToRepartition_false;
        if (beginRebalance) eventType = eMessage_PrepareToRepartition_true;

        for (int workingNodeID = 0; workingNodeID < _schedulerInstance->_numberOfActiveProcesses; ++workingNodeID)
        {
            if (workingNodeID != _schedulerInstance->_masterNodeID)
                sendDataRequestToNode(&eventType, 1, MPI_INT, workingNodeID, ePrepareToRepartition, MPI_COMM_WORLD);
        }
    }

    bool MPIAutoAdjustment::receiveSignalToBeginRebalance() const
    {
        int eventType;
        MPI_Recv(&eventType, 1, MPI_INT, _schedulerInstance->_masterNodeID, ePrepareToRepartition, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (eventType == eMessage_PrepareToRepartition_true) return true;
        return false;
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

    bool MPIAutoAdjustment::needToRebalance(const int& masterNodeID, double& totalAgentPhasesTotalTime) const
    {
        if (_schedulerInstance->getId() == masterNodeID)
        {
            std::vector<double> nodesTime(_schedulerInstance->_numberOfActiveProcesses);
            double masterAgentPhasesTotalTime = getAgentPhasesTotalTime();

            totalAgentPhasesTotalTime = masterAgentPhasesTotalTime;
            nodesTime[masterNodeID] = masterAgentPhasesTotalTime;

            if (needToRebalanceByLoad(nodesTime)) return true;

            for (int processID = 0; processID < _schedulerInstance->_numberOfActiveProcesses; ++processID)
            {
                if (processID != masterNodeID)
                {
                    double nodeAgentPhasesTotalTime;
                    MPI_Recv(&nodeAgentPhasesTotalTime, 1, MPI_DOUBLE, processID, eAgentPhasesTotalTime, _schedulerInstance->_activeProcessesComm, MPI_STATUS_IGNORE);

                    totalAgentPhasesTotalTime += nodeAgentPhasesTotalTime;

                    nodesTime[processID] = nodeAgentPhasesTotalTime;
                    if (needToRebalanceByLoad(nodesTime)) return true;
                }
            }
        }
        else
        {
            double agentPhasesTotalTime = getAgentPhasesTotalTime();
            MPI_Send(&agentPhasesTotalTime, 1, MPI_DOUBLE, masterNodeID, eAgentPhasesTotalTime, _schedulerInstance->_activeProcessesComm);
        }

        //if (_numTasks == 1 and totalAgentPhasesTotalTime > _world->getConfig().getMinimumLoadToRebalance()) return true;

        return false;
    }

    void MPIAutoAdjustment::receiveAllAgentsFromWorkingNodes()
    {
        //#pragma omp parallel for
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
                        {
                            //#pragma omp critical 
                            //{ 
                                _schedulerInstance->_world->addAgent(agent, false); 
                            //}
                        }
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

    /** PUBLIC METHODS **/

    void MPIAutoAdjustment::checkForRebalancingSpace()
    {
        if (_schedulerInstance->getId() == _schedulerInstance->_masterNodeID)
        {
            double totalAgentPhasesTotalTime;
            if (needToRebalance(_schedulerInstance->_masterNodeID, totalAgentPhasesTotalTime))
            {
                sendSignalsToBeginRebalance(true);
                receiveAllAgentsFromWorkingNodes();
            }
            else sendSignalsToBeginRebalance(false);
        }
        else
        {
            bool beginRebalance = receiveSignalToBeginRebalance();
            if (beginRebalance)
            {
                sendAllAgentsToMasterNode();


            }
        }

        // if (getId() == _masterNodeID)
        // {
        //     double allNodesAgentPhasesTotalTime;    // 1st criteria: To define the load threshold to add or remove processes to active ones
        //     bool needToRebalance;                   // 2nd criteria: To check if a rebalancing is needed
        //     rebalance_receiveAndCheckAllNodesAgentPhasesTotalTime(getId(), allNodesAgentPhasesTotalTime, needToRebalance);

        //     MPI_Barrier(_activeProcessesComm);
        //     rebalance_readjustProcessesIfNecessary(getId(), allNodesAgentPhasesTotalTime, needToRebalance);
        // }
        // else
        // {
        //     rebalance_sendAgentPhasesTotalTime(_masterNodeID);

        //     MPI_Barrier(_activeProcessesComm);
        //     rebalance_receiveReadjustmentIfNecessary(_masterNodeID);
        // }
    }

} // namespace Engine