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

#ifndef __OpenMPIMultiNodeLogs_hxx__
#define __OpenMPIMultiNodeLogs_hxx__

#include <OpenMPIMultiNode.hxx>
#include <DynamicRaster.hxx>
#include <StaticRaster.hxx>

namespace Engine
{

    class OpenMPIMultiNode;

    class OpenMPIMultiNodeLogs
    {
        protected:

            std::string getRasterValues(const DynamicRaster& raster, const OpenMPIMultiNode& schedulerInstance) const;

            std::map<int, std::string> _logFileNames;               //! Names of the log files for each of the MPI processes.

        public:

            /**
             * @brief Construct a new OpenMPIMultiNodeLogs object.
             * 
             */
            OpenMPIMultiNodeLogs();

            /**
             * @brief Destroy the OpenMPIMultiNodeLogs object.
             * 
             */
            virtual ~OpenMPIMultiNodeLogs();

            /**
             * @brief Initializes the logs in which the processes are going to write.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void initLogFileNames(const OpenMPIMultiNode& schedulerInstance);

            /**
             * @brief Prints nodes partitioning and neighbours for each one.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printPartitionsBeforeMPI(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints the nodes structure (ID + Coordinates) in _nodesSpace.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printOwnNodeStructureAfterMPI(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints the neighbourhoods (agents belonging to nodes).
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printNeighbouringAgentsPerTypes(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints the agents for the current node executing this method.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printNodeAgents(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints the rasters for the current node executing this method.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printNodeRasters(const OpenMPIMultiNode& schedulerInstance) const;

    };

} // namespace Engine

#endif // __OpenMPIMultiNodeLogs_hxx__