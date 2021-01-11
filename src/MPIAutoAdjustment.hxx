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
            void sendDataRequestToNode(const void* data, const int& numberOfElements, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag, const MPI_Comm& mpiComm);

            /**
             * @brief Sends a 'signal' to all the working nodes with the tag 'messageTag'.
             * 
             * @param signal const int&
             * @param messageTag messageTag
             */
            void sendSignalToAllWorkingNodes(const int& signal, const int& messageTag);

            /**
             * @brief Returns the total time for all the agent phases for the calling node.
             * 
             * @return double 
             */
            double getAgentPhasesTotalTime() const;

            /**
             * @brief Checks whether the parameters indicated for the rebalancing are valid (>= 0). If not, it returns false. True otherwise.
             * 
             * @return bool
             */
            bool validParametersForRebalancing() const;

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
             * @brief Receives all the agents from all the active working nodes.
             * 
             */
            void receiveAllAgentsFromWorkingNodes();

            /**
             * @brief Estimates the total cost for 'numberOfProcessesToEstimate' nodes, using the 'executingAgentsEstimatedTime' which is the first term of the estimating equation. If the 'numberOfProcessesToEstimate' is < 1 or > the maximum number of the available MPI processes, it returns DBL_MAX.
             * 
             * @param numberOfProcessesToEstimate const int&
             * @param executingAgentsEstimatedTime const double&
             * @param isPartitioningSuitable bool&
             * @return double 
             */
            double estimateTotalCost(const int& numberOfProcessesToEstimate, const double& executingAgentsEstimatedTime, bool& isPartitioningSuitable);

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
             * @brief Explores the minimum cost performing a step test for different number of nodes. If a rebalance is needed, it returns true (false otherwise). If the minimum cost is gotten with the current _numTasks, this function returns false (true otherwise, and the optimal number of processes is let in 'numberOfProcessesAtMinimumCost'). 
             * 
             * @param agentPhasesAVGTime const double&
             * @param numberOfProcessesAtMinimumCost int&
             * @return bool
             */
            bool exploreMinimumCost(const double& agentPhasesAVGTime, int& numberOfProcessesAtMinimumCost);

            /**
             * @brief Sends the new number of processes 'numberOfProcesses' to all the working nodes.
             * 
             * @param newNumberOfProcesses const int&
             */
            void sendNumberOfProcessesToWorkingNodes(const int& newNumberOfProcesses);

            /**
             * @brief Awakes from sleep all the necessary nodes according the the 'numberOfRequestedProcesses', according to the incoming rebalance.
             * 
             * @param numberOfRequestedProcesses const int&
             */
            void awakeNodesIfNecessary(const int& numberOfRequestedProcesses); 

            /**
             * @brief Does the masters tasks for rebalancing.
             * 
             * @param neededToRebalance bool&
             * @param newNumberOfProcesses int& 
             */
            void doMasterForRebalance(bool& neededToRebalance, int& newNumberOfProcesses);

            /**
             * @brief Receives from the masterNode an eventType with tag 'messageTag', which returns as an integer.
             * 
             * @param messageTag const int&
             * @return int
             */
            int receiveSignalFromMasterWithTag(const int& messageTag) const;

            /**
             * @brief (For the workers nodes) Waits and attends for the master requests, which is evaluating whether a rebalancing is required.
             * 
             */
            void waitForMasterNeedsToRebalance();

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
             * @brief Receives the new number of processes 'newNumberOfProcesses' from the master node.
             * 
             * @param newNumberOfProcesses int&
             */
            void receiveNumberOfProcessesFromMasterNode(int& newNumberOfProcesses);

            /**
             * @brief Does the workers tasks for rebalancing.
             * 
             * @param neededToRebalance bool&
             * @param newNumberOfProcesses int&
             */
            void doWorkersForRebalance(bool& neededToRebalance, int& newNumberOfProcesses);

            /**
             * @brief Saves the current partitioning scheme at 'spaces'.
             * 
             * @param spaces MPINodesMap&
             */
            void saveSpaces(MPINodesMap& spaces);

            /**
             * @brief Sends the partitioning scheme 'spaces' to all the nodes. The 'numberOfProcessesToIntercommunicate' must be specified, since for instance for a scenario of 4->2 nodes, 4 nodes must communicate with each other (to send their agents and rasters to the new 2 partitions), not just the incoming 2.
             * 
             * @param spaces const MPINodesMap&
             * @param numberOfProcessesToIntercommunicate const int&
             */
            void sendSpacesToAllNodes(const MPINodesMap& spaces, const int& numberOfProcessesToIntercommunicate);

            /**
             * @brief Receives the partitioning scheme performed by the master node. It lets the new partitioning in 'spaces'.
             * 
             * @param spaces MPINodesMap&
             */
            void receiveSpacesFromMasterNode(MPINodesMap& spaces) const;

            /**
             * @brief Fills up the 'spaces' data structure with the overlap areas and their corresponding neighbours.
             * 
             * @param newSpaces const MPINodesMap&
             */
            void generateSpacesOverlapsAndNeighbours(MPINodesMap& spaces) const;

            /**
             * @brief Prints the partitioning scheme at 'spaces'. The 'oldType' param indicates whether if they are the old or the new spaces to print.
             * 
             * @param spaces const MPINodesMap&
             * @param oldType const bool&
             */
            void printSpaces(const MPINodesMap& spaces, const bool& oldType) const;

            /**
             * @brief If the calling node has its own space, it removes the non-belonging agents considering the 'spaces'.at(getId()) area.
             * 
             * @param spaces const MPINodesMap&
             */
            void removeNonBelongingAgentsToMPINode(const MPINodesMap& spaces);

            /**
             * @brief Removes the non-needed agents in the master node (since it had them all in order perform the rebalance).
             * 
             * @param oldSpaces const MPINode&
             */
            void removeMasterNodeNoNNeededAgents(const MPINodesMap& oldSpaces);

            /**
             * @brief Updates the partitioning data structures for the calling node.
             * 
             * @param newSpaces const MPINodesMap&
             */
            void updateOwnStructures(const MPINodesMap& newSpaces);

            /**
             * @brief Initializes the map 'agentsByTypeAndNode' considering the 'totalNumberOfSendingNodes'.
             * 
             * @param agentsByTypeAndNode std::map<int, std::map<std::string, AgentsList>>&
             * @param totalNumberOfSendingNodes const int&
             */
            void initializeAgentsToSendMap(std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode, const int& totalNumberOfSendingNodes) const;

            /**
             * @brief Gets the node IDs of the spaces physically containing the 'position'.
             * 
             * @param position const Point2D<int>&
             * @param spaces const MPINodesMap&
             * @return std::set<int> 
             */
            std::set<int> getNodesContainingPosition(const Point2D<int>& position, const MPINodesMap& spaces) const;

            /**
             * @brief Prints the 'agentsByTypeAndNode' data structure.
             * 
             * @param agentsByTypeAndNode const std::map<int, std::map<std::string, AgentsList>>&
             */
            void printAgentsByTypeAndNodeToSend(const std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode) const;

            /**
             * @brief Sends the agents in agentsByTypeAndNode->second->second to their corresponding nodes agentsByTypeAndNode->first.
             * 
             * @param agentsByTypeAndNode const std::map<int, std::map<std::string, AgentsList>>&
             */
            void sendAgentsInMap(const std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode);

            /**
             * @brief Sends those agents that the calling node contains to the rest of the nodes if necessary (according to the new scheme 'newSpaces').
             * 
             * @param newSpaces const MPINodesMap&
             * @param oldSpaces const MPINodesMap&
             */
            void sendAgentsToOtherNodesIfNecessary(const MPINodesMap& newSpaces, const MPINodesMap& oldSpaces);

            /**
             * @brief Receives the agents that the rest of the nodes have sent to the calling node.
             * 
             * @param numberOfNodesToReceiveFrom const int&
             */
            void receiveAgentsFromOtherNodesIfNecessary(const int& numberOfNodesToReceiveFrom);

            /**
             * @brief Initializes the map 'rastersValuesByNode' considering the 'totalNumberOfSendingNodes'.
             * 
             * @param rastersValuesByNode std::map<int, MapOfValuesByRaster>&
             * @param totalNumberOfSendingNodes const int&
             */
            void initializeRasterValuesToSendMap(std::map<int, MapOfValuesByRaster>& rastersValuesByNode, const int& totalNumberOfSendingNodes) const;

            /**
             * @brief Sends the raster cells in rastersValuesByNode->second->second to their corresponding nodes rastersValuesByNode->first.
             * 
             * @param rastersValuesByNode const std::map<int, MapOfValuesByRaster>&
             */
            void sendRastersInMap(const std::map<int, MapOfValuesByRaster>& rastersValuesByNode);

            /**
             * @brief Sends those raster cells that the calling node contains to the rest of the nodes if necessary (according to the new scheme 'newSpaces').
             * 
             * @param newSpaces const MPINodesMap&
             * @param oldSpaces const MPINodesMap&
             */
            void sendRastersToOtherNodesIfNecessary(const MPINodesMap& newSpaces, const MPINodesMap& oldSpaces);

            /**
             * @brief Receives the rasters cells that the rest of the nodes have sent to the calling node.
             * 
             * @param numberOfNodesToReceiveFrom const int&
             */
            void receiveRastersFromOtherNodesIfNecessary(const int& numberOfNodesToReceiveFrom);

            /**
             * @brief If the calling node has its own space, it removes the non-belonging raster cells considering the 'spaces'.at(getId()) area.
             * 
             * @param mpiNode const MPINodesMap&
             */
            void removeNonBelongingRasterCellsToMPINode(const MPINodesMap& spaces);

            /**
             * @brief Rebalances the current space with 'numberOfProcesses'. To do so, the nodes send the needed agents and raster cells among them, working the master and the workers together.
             * 
             * @param numberOfProcesses const int&
             */
            void rebalanceAgentsAndRastersAmongNodes(const int& numberOfProcesses);

            /**
             * @brief Writes the final state of the rebalancing just performed in the log file of each process.
             * 
             */
            void writeStateAfterRebalanceInLog();

            /**
             * @brief Puts the non-needed workers to sleep.
             * 
             * @param newNumberOfProcesses const int&
             */
            void putNonNeededWorkersToSleep(const int& newNumberOfProcesses);

            /**
             * @brief Synchronizes the active processes after the rebalancing, if necessary.
             * 
             */
            void synchronizeProcesses();

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