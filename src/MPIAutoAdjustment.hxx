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

#ifndef __MPIAutoAdjustment_hxx__
#define __MPIAutoAdjustment_hxx__

#include <MPIMultiNode.hxx>

#include <mpi.h>
#include <set>

namespace Engine
{

    class MPIMultiNode;

    class MPIAutoAdjustment
    {

        protected:

            /** General structures **/
            MPIMultiNode* _schedulerInstance;       //! Instance of the scheduler containing all the variables needed for auto adjustment.

            double _currentAgentPhasesAVGTime;

            uint64_t _cpuTicks;                     //! CPU base clock ticks

            /** MPI Data Structures **/
            std::list<MPI_Request*> _sendRequests;

            /** PROTECTED METHODS **/

            /**
             * @brief Get the current CPU clock ticks.
             * 
             * @return uint64_t 
             */
            uint64_t getTicks(void);

            /**
             * @brief Sends a non-blocking request of 'numberOfElements', from 'data' typed as 'mpiDataType, to 'destinationNode', tagged with 'tag', by the default communicator MPI_COMM_WORLD.
             * 
             * @param data void*
             * @param numberOfElements const int&
             * @param mpiDatatype const MPI_Datatype&
             * @param destinationNode const int&
             * @param tag const int&
             * @param mpiComm const MPI_Comm&
             */
            void sendDataRequestToNode(void* data, const int& numberOfElements, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag, const MPI_Comm& mpiComm);

            /**
             * @brief Sends a 'signal' to all the working nodes with the tag 'messageTag'.
             * 
             * @param signal const int&
             * @param messageTag messageTag
             */
            void sendSignalToAllWorkingNodes(const int& signal, const int& messageTag);

            /**
             * @brief Receives from the masterNode an eventType with tag 'messageTag', which returns as an integer.
             * 
             * @param messageTag const int&
             * @return int
             */
            int receiveSignalFromMasterWithTag(const int& messageTag) const;

            /**
             * @brief Returns the total time for all the agent phases for the calling node.
             * 
             * @return double 
             */
            double getAgentPhasesTotalTime() const;

            /**
             * @brief Returns true if the last added time in 'nodesTime' is >= MaximumLoadPerNode. Also, returns true if the last time added in 'nodesTime' (nodesTime[nodesTime.size() - 1]) exceeds the maximum percentage of unbalance with some of the previously added times.
             * 
             * @param nodesTime const std::vector<double>&
             * @return bool
             */
            bool needToRebalanceByLoad(const std::vector<double>& nodesTime) const;

            /**
             * @brief (For the master node) Receives the total time for all the agents' execution phases, for all the nodes, and computes their load differences. Then, it returns whether a rebalance is needed or not, letting the average in 'agentPhasesAVGTime'.
             * 
             * @param agentPhasesAVGTime double&
             * @return bool
             */
            bool needToRebalance(double& agentPhasesAVGTime);

            /**
             * @brief (For the workers nodes) Waits and attends for the master requests, which is evaluating whether a rebalancing is required.
             * 
             * @return bool
             */
            int waitForMasterNeedsToRebalance();

            /**
             * @brief Receives all the agents from all the active working nodes.
             * 
             */
            void receiveAllAgentsFromWorkingNodes();

            /**
             * @brief Gets all the agents of the calling node and let them classified by type in 'agentsByType'.
             * 
             * @param agentsByType std::map<std::string, AgentsList>&
             */
            void getAllOwnedAreaAgentsByType(std::map<std::string, AgentsList>& agentsByType) const;

            /**
             * @brief Send all the agents of the calling node to the master node.
             * 
             */
            void sendAllAgentsToMasterNode();

            /**
             * @brief Estimates the total cost for 'numberOfProcessesToEstimate' nodes, using the 'executingAgentsEstimatedTime' which is the first term of the estimating equation. If the 'numberOfProcessesToEstimate' is < 1 or > the maximum number of the available MPI processes, it returns DBL_MAX.
             * 
             * @param numberOfProcessesToEstimate const int&
             * @param executingAgentsEstimatedTime const double&
             * @return double 
             */
            double estimateTotalCost(const int& numberOfProcessesToEstimate, const double& executingAgentsEstimatedTime);

            /**
             * @brief Explores possibilities of improvement the average cost trying to double the number of processes in each trial. When it founds a local minimum it stops and returns that number of processes.
             * 
             * @param initialNumberOfProcesses const int&
             * @param totalCost const double
             * @param executingAgentsEstimatedTime const double&
             * @return int 
             */
            int exploreUp(const int& initialNumberOfProcesses, const double& totalCost, const double& executingAgentsEstimatedTime);

            /**
             * @brief Explores possibilities of improvement the average cost trying to double the number of processes in each trial. When it founds a local minimum it stops and returns that number of processes.
             * 
             * @param initialNumberOfProcesses const int&
             * @param totalCost const double
             * @param executingAgentsEstimatedTime const double&
             * @return int 
             */
            int exploreDown(const int& initialNumberOfProcesses, const double& totalCost, const double& executingAgentsEstimatedTime);

            /**
             * @brief Explores the minimum cost performing a step test for different number of nodes. Returns the number of processes that gets a local minimum cost.
             * 
             * @return int
             */
            int exploreMinimumCost();

            /**
             * @brief Sends the new number of processes 'numberOfProcesses' to all the working nodes.
             * 
             * @param newNumberOfProcesses const int&
             */
            void sendNumberOfProcessesToWorkingNodes(const int& newNumberOfProcesses) const;

            /**
             * @brief Receives the new number of processes 'newNumberOfProcesses' from the master node.
             * 
             * @param newNumberOfProcesses const int&
             */
            void receiveNumberOfProcessesFromMasterNode(const int& newNumberOfProcesses);

            /**
             * @brief Awakes from sleep all the necessary nodes according the the 'numberOfRequestedProcesses', according to the incoming rebalance.
             * 
             * @param numberOfRequestedProcesses const int&
             */
            void awakeNodesIfNecessary(const int& numberOfRequestedProcesses); 

            /**
             * @brief Saves the current partitioning status at 'oldSpaces'.
             * 
             * @param spaces MPIMultiNode::MPINodesMap&
             */
            void saveCurrentSpaces(MPINodesMap & spaces) const;

            /**
             * @brief Sends all the just computed partitioning spaces to all nodes in order to let know them whether they need to send to other nodes or discard their in-local-memory agents and rasters. It lets the new partitioning in 'newSpaces'.
             * 
             * @param newSpaces MPIMultiNode::MPINodesMap&
             */
            void sendAllNewSpacesToAllNodes(MPINodesMap& newSpaces) const;

            /**
             * @brief Receives the new spaces resulting from the last partitining performed by the master node. It lets the new partitioning in 'newSpaces'.
             * 
             * @param newSpaces MPIMultiNode::MPINodesMap&
             */
            void receiveNewSpacesFromMasterNode(MPINodesMap& newSpaces) const;

            /**
             * @brief Fills up the 'newSpaces' data structure with the overlap areas.
             * 
             * @param newSpaces const MPIMultiNode::MPINodesMap&
             */
            void fillNewSpacesStructures(MPINodesMap& newSpaces) const;

            /**
             * @brief Initializes the map 'agentsByTypeAndNode' considering the 'totalNumberOfSendingNodes'.
             * 
             * @param agentsByTypeAndNode std::map<int, std::map<std::string, AgentsList>>&
             * @param totalNumberOfSendingNodes const int&
             */
            void initializeAgentsToSendMap(std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode, const int& totalNumberOfSendingNodes) const;

            /**
             * @brief Gets the node IDs of the spaces physically containing the agent.
             * 
             * @param agent 
             * @param spaces 
             * @return std::set<int> 
             */
            std::set<int> getNodesContainingAgent(const Agent& agent, const MPINodesMap& spaces) const;

            /**
             * @brief Sends the agents in agentsToSendByNode->second to their corresponding nodes agentsToSendByNode->first.
             * 
             * @param agentsToSendByNode const std::map<int, AgentsList>&
             */
            void sendAgentsInMap(const std::map<int, AgentsList>& agentsToSendByNode);

            /**
             * @brief Sends those agents that the calling node contains to the rest of the nodes if necessary (according to the new scheme 'newSpaces').
             * 
             * @param newSpaces const MPIMultiNode::MPINodesMap&
             * @param oldSpaces const MPIMultiNode::MPINodesMap&
             */
            void sendAgentsToOtherNodesIfNecessary(const MPINodesMap& newSpaces, const MPINodesMap& oldSpaces);

            /**
             * @brief Receives the agents that the rest of the nodes have sent to the calling node.
             * 
             * @param numberOfNodesToReceiveFrom const int&
             */
            void receiveAgentsFromOtherNodesIfNecessary(const int& numberOfNodesToReceiveFrom);

            /**
             * @brief Sends those raster cells that the calling node contains to the rest of the nodes if necessary (according to the new scheme 'newSpaces').
             * 
             * @param newSpaces const MPIMultiNode::MPINodesMap&
             * @param oldSpaces const MPIMultiNode::MPINodesMap&
             */
            void sendRastersToOtherNodesIfNecessary(const MPIMultiNode::MPINodesMap& newSpaces, const MPIMultiNode::MPINodesMap& oldSpaces);

            /**
             * @brief Receives the rasters cells that the rest of the nodes have sent to the calling node.
             * 
             * @param numberOfNodesToReceiveFrom const int&
             */
            void receiveRastersFromOtherNodesIfNecessary(const int& numberOfNodesToReceiveFrom);

            /**
             * @brief Removes the non-belonging agents considering the current space of the calling node.
             * 
             * @param newSpaces const MPIMultiNode::MPINodesMap&
             */
            void removeNonBelongingAgents(const MPIMultiNode::MPINodesMap& newSpaces);

            /**
             * @brief Updates the current step (_world::_step) for the calling node.
             * 
             * @param newNumberOfProcesses const int&
             */
            void updateCurrentStep(const int& newNumberOfProcesses);

            /**
             * @brief Updates the partitioning data structures for the calling node.
             * 
             * @param newSpaces const MPIMultiNode::MPINodesMap&
             */
            void updateOwnStructures(const MPIMultiNode::MPINodesMap& newSpaces);

            /**
             * @brief Puts the non-needed workers to sleep.
             * 
             * @param newNumberOfProcesses const int&
             */
            void putNonNeededWorkersToSleep(const int& newNumberOfProcesses);

            /**
             * @brief Rebalances the current space with 'numberOfProcesses'.
             * 
             * @param numberOfProcesses const int&
             */
            void rebalance(const int& numberOfProcesses);

        public:

            /**
             * @brief Construct a new MPIAutoAdjustment object.
             * 
             */
            MPIAutoAdjustment();

            /**
             * @brief Destroy the MPIAutoAdjustment object.
             * 
             */
            virtual ~MPIAutoAdjustment();

            /**
             * @brief Initializes this instance.
             * 
             * @param schedulerInstance MPIMultiNode&
             */
            void initAutoAdjustment(MPIMultiNode& schedulerInstance);

            /**
             * @brief Rebalances the space if necessary.
             * 
             */
            void checkForRebalancingSpace();

    };

} // namespace Engine

#endif // __MPIAutoAdjustment_hxx__