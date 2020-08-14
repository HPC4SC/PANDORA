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

            /**
             * @brief Get the 'raster' string in matrix format for the node calling this function. 'discrete' indicates whether discrete or continuous values are requested.
             * 
             * @param raster const DynamicRaster&
             * @param schedulerInstance const OpenMPIMultiNode&
             * @param discrete const bool&
             * @return std::string 
             */
            std::string getRasterValues(const DynamicRaster& raster, const OpenMPIMultiNode& schedulerInstance, const bool& discrete) const;

            std::map<int, std::string> _logFileNames;                   //! Names of the log files for each of the MPI processes.
            std::map<int, std::string> _instrumentationLogFileNames;    //! Names of the instrumentation log files.

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
             * @brief Writes 'message' in the corresponding MPI process log file.
             * 
             * @param message const std::sting&
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void writeInDebugFile(const std::string& message, const OpenMPIMultiNode& schedulerInstance);

            /**
             * @brief Gets the string representing: the nodes partitioning and neighbours for each one.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             * @return std::string 
             */
            std::string getString_PartitionsBeforeMPI(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Gets the string representing: the nodes structure (ID + Coordinates) in _nodesSpace.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             * @return std::string 
             */
            std::string getString_OwnNodeStructureAfterMPI(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Gets the string representing: the neighbourhoods (agents belonging to nodes).
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             * @return std::string 
             */
            std::string getString_NeighbouringAgentsPerTypes(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Gets the string representing: the agents for the current node executing this method.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             * @param fullDescription const bool&
             * @return std::string 
             */
            std::string getString_NodeAgents(const OpenMPIMultiNode& schedulerInstance, const bool& fullDescription) const;

            /**
             * @brief Gets the string representing: the rasters for the current node executing this method.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             * @param discrete const bool&
             * @return std::string 
             */
            std::string getString_NodeRasters(const OpenMPIMultiNode& schedulerInstance, const bool& discrete) const;

            /**
             * @brief Gets the string representing: the matrix of agents (_agentsMatrix member) for the current node executing this method.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             * @return std::string 
             */
            std::string getString_AgentsMatrix(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints in the node's debug file: the nodes partitioning and neighbours for each one.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printPartitionsBeforeMPIInDebugFile(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints in the node's debug file: the nodes structure (ID + Coordinates) in _nodesSpace.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printOwnNodeStructureAfterMPIInDebugFile(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints in the node's debug file: the neighbourhoods (agents belonging to nodes).
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printNeighbouringAgentsPerTypesInDebugFile(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints in the node's debug file: the agents for the current node executing this method.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printNodeAgentsInDebugFile(const OpenMPIMultiNode& schedulerInstance, const bool& fullDescription = false) const;

            /**
             * @brief Prints in the node's debug file: the rasters for the current node executing this method.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printNodeRastersInDebugFile(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints in the node's debug file: the discrete state of the rasters for the current node executing this method.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printNodeRastersDiscreteInDebugFile(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints in the node's debug file: the matrix of agents (_agentsMatrix member) for the current node executing this method.
             * 
             * @param schedulerInstance const OpenMPIMultiNode&
             */
            void printAgentsMatrixInDebugFile(const OpenMPIMultiNode& schedulerInstance) const;

            /**
             * @brief Prints 'message' in the corresponding instrumentation log file of the calling process.
             * 
             * @param schedulerInstance const OpenMPIMultiNode& schedulerInstance
             * @param message const std::string&
             */
            void printInstrumentation(const OpenMPIMultiNode& schedulerInstance, const std::string& message);

    };

} // namespace Engine

#endif // __OpenMPIMultiNodeLogs_hxx__