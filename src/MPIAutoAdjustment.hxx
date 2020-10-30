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

namespace Engine
{

    class MPIMultiNode;

    class MPIAutoAdjustment
    {

        protected:

            /** General structures **/
            const MPIMultiNode* _schedulerInstance;                 //! Instance of the scheduler containing all the variables needed for auto adjustment.

            /** MPI Data Structures **/
            std::list<MPI_Request*> _sendRequests;

            /** PROTECTED METHODS **/

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
             * @brief Sends the initial signals to all the worker nodes in order to begin the rebalancing.
             * 
             * @param beginRebalance const bool&
             */
            void sendSignalsToBeginRebalance(const bool& beginRebalance);

            /**
             * @brief Receives the initial signal from the master nodes in order to begin the rebalancing. It returns false if no rebalancing is finally required.
             * 
             * @return bool
             */
            bool receiveSignalToBeginRebalance() const;

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
             * @brief Receives the total time for all the agents' execution phases, for all the nodes, and computes their load differences. Then, it returns whether a rebalance is needed or not. Besides, it lets the total load from all the nodes in 'totalAgentPhasesTotalTime'.
             * 
             * @param masterNodeID const int&
             * @return bool
             */
            bool needToRebalance(const int& masterNodeID) const;

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
             * @brief Awakes from sleep all the necessary working nodes according the the 'numberOfRequestedProcesses'.
             * 
             * @param numberOfRequestedProcesses const int&
             */
            void awakeWorkingNodesToRepartition(const int& numberOfRequestedProcesses);

            /**
             * @brief Divides and tests a whole test step for the indicated 'numberOfProcessesToTest'.
             * 
             * @param numberOfProcessesToTest const int&
             * @return double 
             */
            double divideAndTestStep(const int& numberOfProcessesToTest);

            /**
             * @brief From the 'initialNumberOfProcesses', executes test steps in the direction marked by 't', returning the number of processes at the minimum cost.
             * 
             * @param initialNumberOfProcesses const int&
             * @param cost const double&
             * @param lambda const T&
             * @return int 
             */
            template <typename T> int getNumberOfProcessesAtMinimumCost(const int& initialNumberOfProcesses, const double& cost, const T& lambda);

            /**
             * @brief Explores the minimum cost performing a step test for different number of nodes. Returns the number of processes that gets a local minimum cost. NOTE: at the end of this function, the optimal number of processes is awaken.
             * 
             */
            void exploreMinimumCost();

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
             * @param schedulerInstance const MPIMultiNode&
             */
            void initAutoAdjustment(const MPIMultiNode& schedulerInstance);

            /**
             * @brief Rebalances the space if necessary.
             * 
             */
            void checkForRebalancingSpace();

    };

} // namespace Engine

#endif // __MPIAutoAdjustment_hxx__