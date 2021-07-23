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
#include <Agent.hxx>
#include <typedefs.hxx>

namespace Engine
{

    class MPIMultiNode;

    class SaveState
    {
        
        protected:

            MPIMultiNode* _schedulerInstance;     //! Instance of the scheduler containing all the variables needed for checkpointing.
            
            std::string _fileNameCP;              //! Checkpoingint file name for this MPI process.

            struct RasterAttributes {
                int id;
                std::string name;
                bool serialize;

                std::string fileName;
                int layer;
                int minValue;
                int maxValue;
                bool hasColorTable;

                int colorTableSize;
                std::vector<ColorEntry> colorTable;

                int defaultValue;
                int currentMinValue;
                int currentMaxValue;
            };

            /**
             * @brief Loads all the world related data.
             * 
             * @param line std::string
             */
            void loadWorldInfo(std::string line);

            /**
             * @brief Gets a vector of tokens from 'line' after splitting them according to the 'delimiter'.
             * 
             * @param line const std::string&
             * @param delimiter const char&
             * @return std::vector<std::string> 
             */
            std::vector<std::string> getLineTokens(const std::string& line, const char& delimiter) const;

            /**
             * @brief Registers the owned areas of 'node', considering the 'tokens'. 'lineCounter' is used to know what of the 3 owned areas is in particular.
             * 
             * @param node MPINode&
             * @param lineCounter const int&
             * @param tokens const std::vector<std::string>&
             */
            void registerOwnedAreas(MPINode& node, const int& lineCounter, const std::vector<std::string>& tokens);

            /**
             * @brief Loads all the node related data.
             * 
             * @param cpFileStream std::ifstream&
             */
            void loadNodeInfo(std::ifstream& cpFileStream);

            /**
             * @brief Returns whether the raster to be loaded (next straight line in 'cpFileStream') is dynamic or not.
             * 
             * @param cpFileStream std::ifstream&
             * @return bool
             */
            bool isRasterToLoadDynamic(std::ifstream& cpFileStream) const;

            /**
             * @brief Returns in a std::string the 'rasterAttributes'.
             * 
             * @param rasterAttributes const RasterAttributes&
             * @param isDynamicRaster const bool&
             */
            std::string getPrintedRasterAttributes(const RasterAttributes& rasterAttributes, const bool& isDynamicRaster) const;

            /**
             * @brief Reads the basic attributes in 'tokens' for a raster to be registered.
             * 
             * @param cpFileStream std::ifstream&
             * @param rasterAttributes RasterAttributes&
             * @param isDynamicRaster const bool&
             */
            void readRasterBasicAttributes(std::ifstream& cpFileStream, RasterAttributes& rasterAttributes, const bool& isDynamicRaster) const;

            /**
             * @brief Sets the base class attributes (StaticRaster) for the 'raster', retrieving them from 'rasterAttributes'.
             * 
             * @param raster const StaticRaster&
             * @param rasterAttributes RasterAttributes&
             */
            void setBaseClassAttributes(StaticRaster& raster, const RasterAttributes& rasterAttributes);

            /**
             * @brief Creates a new static raster with the rasterAttriutes and registers it in the _schedulerInstance->_world.
             * 
             * @param rasterAttributes const RasterAttributes&
             */
            void createAndRegisterStaticRaster(const RasterAttributes& rasterAttributes);

            /**
             * @brief Creates a new dynamic raster with the rasterAttriutes and registers it in the _schedulerInstance->_world.
             * 
             * @param rasterAttributes const RasterAttributes&
             */
            void createAndRegisterDynamicRaster(const RasterAttributes& rasterAttributes);

            /**
             * @brief Reads the (basic) values for the raster identified by 'name'. It assumes a dynamic raster.
             * 
             * @param cpFileStream std::ifstream&
             * @param name const std::string&
             * @param isDynamicRaster const bool&
             */
            void readRasterValues(std::ifstream& cpFileStream, const std::string& name, const bool& isDynamicRaster);

            /**
             * @brief Loads all the raster related data.
             * 
             * @param cpFileStream std::ifstream&
             */
            void loadRasters(std::ifstream& cpFileStream);

            /**
             * @brief Gets the agent type based on its 'encodedAgent' stream.
             * 
             * @param encodedAgent const std::string&
             * @return std::string 
             */
            std::string getAgentType(const std::string& encodedAgent) const;

            /**
             * @brief Loads all the agent related data.
             * 
             * @param cpFileStream std::ifstream&
             */
            void loadAgents(std::ifstream& cpFileStream);

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
             * @brief Cleans the checkpointing directory, i.e. deletes all the files in it.
             * 
             */
            void cleanCheckpointingDirectory();

            /**
             * @brief Gets the number of checkpointing files in the checkpointing directory.
             * 
             * @return int 
             */
            int getNumberOfCheckpointingFiles();

            /**
             * @brief Initializes the checkpointing files in which the processes are going to write.
             * 
             * @param schedulerInstance MPIMultiNode&
             */
            void initCheckpointFileNames(MPIMultiNode& schedulerInstance);

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

            /**
             * @brief Checks whether a file identified by _fileNameCP exists in the corresponding directory to be properly loaded.
             * 
             * @return bool
             */
            bool myFileExists() const;

            /**
             * @brief Loads the checkpoint considering the current file at the indicated path.
             * 
             */
            void loadCheckpoint();

    };

} // namespace Engine

#endif //__SaveState_hxx__

