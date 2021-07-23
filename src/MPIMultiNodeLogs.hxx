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

#ifndef __MPIMultiNodeLogs_hxx__
#define __MPIMultiNodeLogs_hxx__

#include <MPIMultiNode.hxx>
#include <DynamicRaster.hxx>
#include <StaticRaster.hxx>

namespace Engine
{

    class MPIMultiNode;

    class MPIMultiNodeLogs
    {
        protected:

            const MPIMultiNode* _schedulerInstance;                     //! Instance of the scheduler containing all the variables needed for logging.

            std::string _logFileName;                                   //! Logging file name for this MPI process.
            std::string _instrumentationLogFileName;                    //! Instrumentation file name for this MPI process.

            /**
             * @brief Get the 'raster' string in matrix format for the node calling this function. 'discrete' indicates whether discrete or continuous values are requested.
             * 
             * @param raster const DynamicRaster&
             * @param discrete const bool&
             * @return std::string 
             */
            std::string getRasterValues(const DynamicRaster& raster, const bool& discrete) const;

        public:

            /**
             * @brief Construct a new MPIMultiNodeLogs object.
             * 
             */
            MPIMultiNodeLogs();

            /**
             * @brief Destroy the MPIMultiNodeLogs object.
             * 
             */
            virtual ~MPIMultiNodeLogs();

            /**
             * @brief Initializes the logs in which the processes are going to write.
             * 
             * @param schedulerInstance const MPIMultiNode&
             */
            void initFilesName(const MPIMultiNode& schedulerInstance);

            /**
             * @brief Writes 'message' in the corresponding MPI process log file.
             * 
             * @param message const std::sting&
             */
            void writeInDebugFile(const std::string& message);

            /**
             * @brief Gets the string representing: the nodes partitioning and neighbours for each one.
             * 
             * @return std::string 
             */
            std::string getString_PartitionsBeforeMPI() const;

            /**
             * @brief Gets the string representing: the nodes structure (ID + Coordinates) in _nodesSpace.
             * 
             * @return std::string 
             */
            std::string getString_OwnNodeStructure() const;

            /**
             * @brief Gets the string representing: the agents for the current node executing this method.
             * 
             * @param fullDescription const bool&
             * @return std::string 
             */
            std::string getString_NodeAgents(const bool& fullDescription = false) const;

            /**
             * @brief Gets the string representing: the rasters for the current node executing this method. 'discrete' indicates whether discrete or normal values are requested.
             * 
             * @param discrete const bool&
             * @return std::string 
             */
            std::string getString_NodeRasters(const bool& discrete = false) const;

            /**
             * @brief Gets the string representing: the matrix of agents (_agentsMatrix member) for the current node executing this method. If 'printAllMatrix' == false (default) then it just prints the agents within the _nodeSpace.ownedAreaWithOuterOverlaps area of the calling process.
             * 
             * @param printAllMatrix const bool&
             * @return std::string 
             */
            std::string getString_AgentsMatrix(const bool& printAllMatrix = false) const;

            /**
             * @brief Prints in the node's debug file: the nodes partitioning and neighbours for each one.
             * 
             */
            void printPartitionsBeforeMPIInDebugFile() const;

            /**
             * @brief Prints in the node's debug file: the nodes structure (ID + Coordinates) in _nodesSpace.
             * 
             */
            void printOwnNodeStructureInDebugFile() const;
            
            /**
             * @brief Prints in the node's debug file: the agents for the current node executing this method.
             * 
             */
            void printNodeAgentsInDebugFile(const bool& fullDescription = false) const;

            /**
             * @brief Prints in the node's debug file: the rasters for the current node executing this method.
             * 
             */
            void printNodeRastersInDebugFile() const;

            /**
             * @brief Prints in the node's debug file: the discrete state of the rasters for the current node executing this method.
             * 
             */
            void printNodeRastersDiscreteInDebugFile() const;

            /**
             * @brief Prints in the node's debug file: the matrix of agents (_agentsMatrix member) for the current node executing this method.
             * 
             * @param printAllMatrix const bool&
             */
            void printAgentsMatrixInDebugFile(const bool& printAllMatrix = false) const;

            /**
             * @brief Prints 'message' in the corresponding instrumentation log file of the calling process.
             * 
             * @param message const std::string&
             */
            void printInstrumentation(const std::string& message);

    };

} // namespace Engine

#endif // __MPIMultiNodeLogs_hxx__