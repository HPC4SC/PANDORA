/*
 * Copyright ( c ) 2012
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

#ifndef __SaveState_hxx__
#define __SaveState_hxx__

#include <World.hxx>

namespace Engine
{

    class MPIMultiNode;

    class SaveState
    {
        
        const MPIMultiNode* _schedulerInstance;     //! Instance of the scheduler containing all the variables needed for checkpointing.
        
        std::string _fileNameCP;                    //! Checkpoingint file name for this MPI process.

        public:

            /**
             * @brief Construct a new SaveState object.
             *  
             */
            SaveState();

            /**
             * @brief Destroy the SaveState object.
             * 
             */
            virtual ~SaveState( );

            /**
             * @brief Initializes the checkpointing files in which the processes are going to write.
             * 
             * @param schedulerInstance const MPIMultiNode&
             */
            void initCheckpointFileNames(const MPIMultiNode& schedulerInstance);

            /**
             * @brief Saves the _nodeSpace data structure.
             * 
             */
            void saveNodeSpace() const;

            /**
             * @brief Saves the scheduler global attributes.
             * 
             */
            void saveSchedulerAttributes() const;

            /**
             * @brief Prepares the environement to save the current state of the simulation.
             * 
             */
            void startCheckpointing() const;

            /**
             * @brief Saves dynamic rasters in the checkpoint file stated in _config.
             * 
             */
            void saveRastersInCPFile();
            
            /**
             * @brief Saves all the agents of the current simulation in the checkpoint file stated in _config.
             * 
             */
            void saveAgentsInCPFile();

    };

} // namespace Engine

#endif //__SaveState_hxx__

