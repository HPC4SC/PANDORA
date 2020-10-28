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

    void MPIAutoAdjustment::checkForRebalancingSpace()
    {
        if (getId() == _masterNodeID)
        {
            double allNodesAgentPhasesTotalTime;    // 1st criteria: To define the load threshold to add or remove processes to active ones
            bool needToRebalance;                   // 2nd criteria: To check if a rebalancing is needed
            rebalance_receiveAndCheckAllNodesAgentPhasesTotalTime(getId(), allNodesAgentPhasesTotalTime, needToRebalance);

            MPI_Barrier(_activeProcessesComm);
            rebalance_readjustProcessesIfNecessary(getId(), allNodesAgentPhasesTotalTime, needToRebalance);
        }
        else
        {
            rebalance_sendAgentPhasesTotalTime(_masterNodeID);

            MPI_Barrier(_activeProcessesComm);
            rebalance_receiveReadjustmentIfNecessary(_masterNodeID);
        }
    }

} // namespace Engine