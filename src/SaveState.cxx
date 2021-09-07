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

#include <SaveState.hxx>

#include <Logger.hxx>
#include <Exception.hxx>
#include <DynamicRaster.hxx>
#include <StaticRaster.hxx>
#include <MPIMultiNodeLogs.hxx>
#include <CheckpointingFactory.hxx>

#include <boost/filesystem.hpp>

namespace Engine
{
    
    SaveState::SaveState() : _periodicCPCounter(1)
    {
    }

    SaveState::~SaveState()
    {
    }

    int SaveState::getPeriodicCPCounter() const
    {
        return _periodicCPCounter;
    }

    void SaveState::increasePeriodicCPCounter()
    {
        _periodicCPCounter += 1;
    }

    void SaveState::initCheckpointFileNames(MPIMultiNode& schedulerInstance)
    {
        _schedulerInstance = &schedulerInstance;

        std::string fileNameCP_base = _schedulerInstance->_world->getConfig().getFileNameCP();
        _fileNameCP = CreateStringStream(fileNameCP_base << "_" << _schedulerInstance->getId()).str();
    }

    void SaveState::resetCPFile()
    {
        Engine::GeneralState::loggerCP().closeFile(_fileNameCP);
    }

    void SaveState::cleanCheckpointingDirectory()
    {
        for (boost::filesystem::directory_iterator it(_schedulerInstance->_world->getConfig().getDirectoryCP()); it != boost::filesystem::directory_iterator(); ++it)
        {
            bool needToBeDeleted = true;
            for (int i = 0; i < _schedulerInstance->_numTasks; ++i)
            {            
                if (it->path().filename().string().find(std::to_string(i)) != std::string::npos) needToBeDeleted = false;
            }

            if (needToBeDeleted)
                boost::filesystem::remove(it->path());
        }
    }

    int SaveState::getNumberOfCheckpointingFiles()
    {
        int numberOfFiles = 0;
        for (boost::filesystem::directory_iterator it(_schedulerInstance->_world->getConfig().getDirectoryCP()); it != boost::filesystem::directory_iterator(); ++it)
        {
            if (it->path().extension().string() == ".log") ++numberOfFiles;
        }
        return numberOfFiles;
    }

    void SaveState::saveWorldData() const
    {
        log_CP(_fileNameCP, CreateStringStream(_schedulerInstance->_world->getCurrentStep() << " (time:" << _schedulerInstance->_world->getWallTime() << ")\n").str());
        log_CP(_fileNameCP, CreateStringStream(_schedulerInstance->_world->getWorldData() << "\n").str());
    }

    void SaveState::saveNodeSpace() const
    {
        Rectangle<int> ownedAreaWithoutInnerOverlap = _schedulerInstance->_nodeSpace.ownedAreaWithoutInnerOverlap;
        Rectangle<int> ownedArea = _schedulerInstance->_nodeSpace.ownedArea;
        Rectangle<int> ownedAreaWithOuterOverlap = _schedulerInstance->_nodeSpace.ownedAreaWithOuterOverlap;

        log_CP(_fileNameCP, CreateStringStream(ownedAreaWithoutInnerOverlap.left() << " " << ownedAreaWithoutInnerOverlap.top() << " " << ownedAreaWithoutInnerOverlap.right() << " " << ownedAreaWithoutInnerOverlap.bottom() << "\n").str());
        log_CP(_fileNameCP, CreateStringStream(ownedArea.left() << " " << ownedArea.top() << " " << ownedArea.right() << " " << ownedArea.bottom() << "\n").str());
        log_CP(_fileNameCP, CreateStringStream(ownedAreaWithOuterOverlap.left() << " " << ownedAreaWithOuterOverlap.top() << " " << ownedAreaWithOuterOverlap.right() << " " << ownedAreaWithOuterOverlap.bottom() << "\n").str());

        for (std::map<int, MPINode*>::const_iterator it = _schedulerInstance->_nodeSpace.neighbours.begin(); it != _schedulerInstance->_nodeSpace.neighbours.end(); ++it)
        {
            int neighbourID = it->first;
            MPINode* neighbourNode = it->second;

            Rectangle<int> ownedAreaWithoutInnerOverlap_neighbour = neighbourNode->ownedAreaWithoutInnerOverlap;
            Rectangle<int> ownedArea_neighbour = neighbourNode->ownedArea;
            Rectangle<int> ownedAreaWithOuterOverlap_neighbour = neighbourNode->ownedAreaWithOuterOverlap;

            log_CP(_fileNameCP, CreateStringStream("neighbourID: " << neighbourID << "\n").str());
            log_CP(_fileNameCP, CreateStringStream(ownedAreaWithoutInnerOverlap_neighbour.left() << " " << ownedAreaWithoutInnerOverlap_neighbour.top() << " " << ownedAreaWithoutInnerOverlap_neighbour.right() << " " << ownedAreaWithoutInnerOverlap_neighbour.bottom() << "\n").str());
            log_CP(_fileNameCP, CreateStringStream(ownedArea_neighbour.left() << " " << ownedArea_neighbour.top() << " " << ownedArea_neighbour.right() << " " << ownedArea_neighbour.bottom() << "\n").str());
            log_CP(_fileNameCP, CreateStringStream(ownedAreaWithOuterOverlap_neighbour.left() << " " << ownedAreaWithOuterOverlap_neighbour.top() << " " << ownedAreaWithOuterOverlap_neighbour.right() << " " << ownedAreaWithOuterOverlap_neighbour.bottom() << "\n").str());
        }

        for (std::map<int, Rectangle<int>>::const_iterator it = _schedulerInstance->_nodeSpace.innerSubOverlaps.begin(); it != _schedulerInstance->_nodeSpace.innerSubOverlaps.end(); ++it)
        {
            int suboverlapID = it->first;
            Rectangle<int> suboverlap = it->second;

            log_CP(_fileNameCP, CreateStringStream("suboverlapID: " << suboverlapID << " " << suboverlap.left() << " " << suboverlap.top() << " " << suboverlap.right() << " " << suboverlap.bottom() << "\n").str());
        }

        for (std::map<int, std::list<int>>::const_iterator it = _schedulerInstance->_nodeSpace.innerSubOverlapsNeighbours.begin(); it != _schedulerInstance->_nodeSpace.innerSubOverlapsNeighbours.end(); ++it)
        {
            int suboverlapID = it->first;

            std::stringstream ss;
            ss << "suboverlapID_neighbours: " << suboverlapID << " ";
            for (std::list<int>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                int neighbourID = *it2;
                ss << neighbourID << " ";
            }
            ss << "\n";
            log_CP(_fileNameCP, ss.str());
        }
    }

    void SaveState::saveSchedulerAttributes() const
    {
        log_CP(_fileNameCP, CreateStringStream("masterNodeID: " << _schedulerInstance->_masterNodeID << "\n").str());
    }

    void SaveState::startCheckpointing() const
    {
        log_CP(_fileNameCP, CreateStringStream("Step_finished:\n").str());
        saveWorldData();
        log_CP(_fileNameCP, CreateStringStream("END_DATA\n").str());

        log_CP(_fileNameCP, CreateStringStream("Node_data:\n").str());
        saveNodeSpace();
        saveSchedulerAttributes();
        log_CP(_fileNameCP, CreateStringStream("END_DATA\n").str());
    }

    void SaveState::saveRastersInCPFile()
    {
        int numberOfRasters = _schedulerInstance->_world->getNumberOfRasters();

        log_CP(_fileNameCP, CreateStringStream("Rasters_data:\n").str());
        log_CP(_fileNameCP, CreateStringStream("Number_of_rasters: " << numberOfRasters << "\n").str());
        for (int i = 0; i < numberOfRasters; ++i)
        {
            if (not _schedulerInstance->_world->isRasterDynamic(i))
            {
                Rectangle<int> wholeBoundaries = Rectangle<int>(_schedulerInstance->_world->getConfig().getSize());

                StaticRaster& staticRaster = _schedulerInstance->_world->getStaticRaster(i);
                log_CP(_fileNameCP, CreateStringStream("STATIC\n").str());
                log_CP(_fileNameCP, CreateStringStream(staticRaster.getRasterGeneralInfo() << "\n").str());
                log_CP(_fileNameCP, CreateStringStream("VALUES: " << staticRaster.getSize().getWidth() << " " << staticRaster.getSize().getHeight() << "\n").str());
                log_CP(_fileNameCP, CreateStringStream(staticRaster.getRasterValues(wholeBoundaries)).str());
            }
            else
            {
                Rectangle<int> knownBoundaries = _schedulerInstance->_world->getBoundaries();

                DynamicRaster& dynamicRaster = _schedulerInstance->_world->getDynamicRaster(i);
                log_CP(_fileNameCP, CreateStringStream("DYNAMIC\n").str());
                log_CP(_fileNameCP, CreateStringStream(dynamicRaster.getRasterGeneralInfo() << "\n").str());
                log_CP(_fileNameCP, CreateStringStream("VALUES: " << dynamicRaster.getSize().getWidth() << " " << dynamicRaster.getSize().getHeight() << "\n").str());
                log_CP(_fileNameCP, CreateStringStream(dynamicRaster.getRasterValues(knownBoundaries)).str());
                log_CP(_fileNameCP, CreateStringStream("MAX_VALUES: " << dynamicRaster.getSize().getWidth() << " " << dynamicRaster.getSize().getHeight() << "\n").str());
                log_CP(_fileNameCP, CreateStringStream(dynamicRaster.getRasterMaxValues(knownBoundaries)).str());
            }
        }
        log_CP(_fileNameCP, CreateStringStream("END_DATA\n").str());
    }

    void SaveState::saveAgentsInCPFile()
    {
        log_CP(_fileNameCP, CreateStringStream("Agents_data (" << _schedulerInstance->_world->getNumberOfAgents() << "):\n").str());
        for (AgentsMap::const_iterator it = _schedulerInstance->_world->beginAgents(); it != _schedulerInstance->_world->endAgents(); ++it)
        {
            AgentPtr agentPtr = it->second;

            log_CP(_fileNameCP, CreateStringStream(agentPtr->encodeAllAttributesInString() << "\n").str());
        }
        log_CP(_fileNameCP, CreateStringStream("END_DATA\n").str());
    }

    bool SaveState::myFileExists() const
    {
        return Engine::GeneralState::loggerCP().checkFileExistance(_fileNameCP);
    }

    std::vector<std::string> SaveState::getLineTokens(const std::string& line, const char& delimiter) const
    {
        std::stringstream lineSS(line);

        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(lineSS, token, delimiter))
            tokens.push_back(token);

        return tokens;
    }

    void SaveState::loadWorldInfo(std::ifstream& cpFileStream)
    {
        int lineCounter = 1;

        std::string line;
        while (std::getline(cpFileStream, line))
        {
//std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::loadWorldInfo " << line << "\n").str();
            if (line.find("END_DATA") != std::string::npos) break;

            if (lineCounter == 1)
            {
                std::vector<std::string> tokens = getLineTokens(line, ' ');

                int currentStep = std::stoi(tokens[0]);
                _schedulerInstance->_world->setCurrentStep(currentStep + 1);
            }
            else if (lineCounter == 2)
                CheckpointingFactory::instance().setWorldData(_schedulerInstance->_world, line);

            ++lineCounter;
        }
    }

    void SaveState::registerOwnedAreas(MPINode& node, const int& lineCounter, const std::vector<std::string>& tokens)
    {
        Rectangle<int> rectangle = Rectangle<int>(std::stoi(tokens[0]), std::stoi(tokens[1]), std::stoi(tokens[2]), std::stoi(tokens[3]));

        if (lineCounter == 1)       node.ownedAreaWithoutInnerOverlap = rectangle;
        else if (lineCounter == 2)  node.ownedArea = rectangle;
        else if (lineCounter == 3)  node.ownedAreaWithOuterOverlap = rectangle;
    }

    void SaveState::loadNodeInfo(std::ifstream& cpFileStream)
    {
        int lineCounter = 1;

        std::string line;
        while (std::getline(cpFileStream, line))
        {
//std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::loadNodeInfo " << line << "\n").str();
            if (line.find("END_DATA") != std::string::npos) break;
            
            std::vector<std::string> tokens = getLineTokens(line, ' ');

            if (lineCounter == 1 or lineCounter == 2 or lineCounter == 3)
                registerOwnedAreas(_schedulerInstance->_nodeSpace, lineCounter, tokens);
            else if (lineCounter >= 4)
            {
                if (tokens[0].compare("neighbourID:") == 0)
                {
                    int neighbourID = std::stoi(tokens[1]);
                    _schedulerInstance->_nodeSpace.neighbours[neighbourID] = new MPINode();

                    int lineCounter2 = 1;
                    while (lineCounter2 <= 3)
                    {
                        std::getline(cpFileStream, line);
                        tokens = getLineTokens(line, ' ');

//std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::loadNodeInfo (2) " << line << "\n").str();

                        registerOwnedAreas(*(_schedulerInstance->_nodeSpace.neighbours.at(neighbourID)), lineCounter2, tokens);

                        ++lineCounter2;
                    }
                }
                else if (tokens[0].compare("suboverlapID:") == 0)
                {
                    int suboverlapID = std::stoi(tokens[1]);

                    Rectangle<int> rectangle = Rectangle<int>(std::stoi(tokens[2]), std::stoi(tokens[3]), std::stoi(tokens[4]), std::stoi(tokens[5]));
                    _schedulerInstance->_nodeSpace.innerSubOverlaps[suboverlapID] = rectangle;
                }
                else if (tokens[0].compare("suboverlapID_neighbours:") == 0)
                {
                    int suboverlapID = std::stoi(tokens[1]);

                    _schedulerInstance->_nodeSpace.innerSubOverlapsNeighbours[suboverlapID] = std::list<int>();
                    for (int i = 2; i < tokens.size(); ++i)
                        _schedulerInstance->_nodeSpace.innerSubOverlapsNeighbours.at(suboverlapID).push_back(std::stoi(tokens[i]));
                }
                else if (tokens[0].compare("masterNodeID:") == 0)
                {
                    int masterNodeID = std::stoi(tokens[1]);
                    _schedulerInstance->_masterNodeID = masterNodeID;
                }
            }

            ++lineCounter;
        }
//std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::loadNodeInfo GETTING OUT FUNCTION...\n").str();
    }

    bool SaveState::isRasterToLoadDynamic(std::ifstream& cpFileStream) const
    {
        std::string line;
        std::getline(cpFileStream, line);

// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::isRasterToLoadDynamic line: " << line << "\n").str();

        bool isDynamicRaster = false;
        if (line.compare("DYNAMIC") == 0) isDynamicRaster = true;
        return isDynamicRaster;
    }

    std::string SaveState::getPrintedRasterAttributes(const RasterAttributes& rasterAttributes, const bool& isDynamicRaster) const
    {
        std::stringstream ss;
        ss << "[Process #" << _schedulerInstance->getId() << "] PRINTING rasterAttributes...\n";
        ss <<   "\trasterAttributes.id: " << rasterAttributes.id << "\n" <<
                "\trasterAttributes.name: " << rasterAttributes.name << "\n" << 
                "\trasterAttributes.serialize: " << rasterAttributes.serialize << "\n" <<
                "\trasterAttributes.fileName: " << rasterAttributes.fileName << "\n" <<
                "\trasterAttributes.layer: " << rasterAttributes.layer << "\n" <<
                "\trasterAttributes.minValue: " << rasterAttributes.minValue << "\n" <<
                "\trasterAttributes.maxValue: " << rasterAttributes.maxValue << "\n" <<
                "\trasterAttributes.hasColorTable: " << rasterAttributes.hasColorTable << "\n" <<
                "\trasterAttributes.colorTableSize: " << rasterAttributes.colorTableSize << "\n";

        if (isDynamicRaster)
            ss <<   "\trasterAttributes.defaultValue: " << rasterAttributes.defaultValue << "\n" <<
                    "\trasterAttributes.currentMinValue: " << rasterAttributes.currentMinValue << "\n" <<
                    "\trasterAttributes.currentMaxValue: " << rasterAttributes.currentMaxValue << "\n";

        return ss.str();
    }

    void SaveState::readRasterBasicAttributes(std::ifstream& cpFileStream, RasterAttributes& rasterAttributes, const bool& isDynamicRaster) const
    {
        std::string line;
        std::vector<std::string> tokens;

        std::getline(cpFileStream, line);
        tokens = getLineTokens(line, '|');

// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::readRasterBasicAttributes line: " << line << "\n").str();

        rasterAttributes.id = std::stoi(tokens[0]);
        rasterAttributes.name = tokens[1];
        rasterAttributes.serialize = std::stoi(tokens[2]);

        rasterAttributes.fileName = tokens[3];
        rasterAttributes.layer = std::stoi(tokens[4]);
        rasterAttributes.minValue = std::stoi(tokens[5]);
        rasterAttributes.maxValue = std::stoi(tokens[6]);
        rasterAttributes.hasColorTable = std::stoi(tokens[7]);

        rasterAttributes.colorTableSize = std::stoi(tokens[8]);
        for (int j = 0; j < rasterAttributes.colorTableSize; ++j)
        {
            std::vector<std::string> colorTableSplit = getLineTokens(tokens[8 + j + 1], ' ');
            
            ColorEntry colorTable;
            colorTable._r  = (short) std::stoi(colorTableSplit[0]);
            colorTable._g = (short) std::stoi(colorTableSplit[1]);
            colorTable._b = (short) std::stoi(colorTableSplit[2]);
            colorTable._alpha = (short) std::stoi(colorTableSplit[3]);

            rasterAttributes.colorTable.push_back(colorTable);
        }

        if (isDynamicRaster)
        {
            int initialIndexForDynamicAttributes = 8 + rasterAttributes.colorTableSize + 1;
            rasterAttributes.defaultValue = std::stoi(tokens[initialIndexForDynamicAttributes]);
            rasterAttributes.currentMinValue = std::stoi(tokens[initialIndexForDynamicAttributes + 1]);
            rasterAttributes.currentMaxValue = std::stoi(tokens[initialIndexForDynamicAttributes + 2]);
        }

//std::cout << getPrintedRasterAttributes(rasterAttributes, isDynamicRaster);
    }

    void SaveState::setBaseClassAttributes(StaticRaster& raster, const RasterAttributes& rasterAttributes)
    {
        raster.setFileName(rasterAttributes.fileName);
        raster.resize(_schedulerInstance->_world->getConfig().getSize());
        raster.setMinValue(rasterAttributes.minValue);
        raster.setMaxValue(rasterAttributes.maxValue);
        raster.setLayer(rasterAttributes.layer);

        std::vector<ColorEntry> colorTable = rasterAttributes.colorTable;
        raster.setColorTable(rasterAttributes.hasColorTable, rasterAttributes.colorTableSize);
        for (int i = 0; i < colorTable.size(); ++i)
            raster.addColorEntry(i, colorTable[i]._r, colorTable[i]._g, colorTable[i]._b, colorTable[i]._alpha);
    }

    void SaveState::createAndRegisterStaticRaster(const RasterAttributes& rasterAttributes)
    {
        _schedulerInstance->_world->registerStaticRaster(rasterAttributes.name, rasterAttributes.serialize, rasterAttributes.id);
        StaticRaster& staticRaster = _schedulerInstance->_world->getStaticRaster(rasterAttributes.name);

        setBaseClassAttributes(staticRaster, rasterAttributes);
    }

    void SaveState::createAndRegisterDynamicRaster(const RasterAttributes& rasterAttributes)
    {
        _schedulerInstance->_world->registerDynamicRaster(rasterAttributes.name, rasterAttributes.serialize, rasterAttributes.id);
        DynamicRaster& dynamicRaster = _schedulerInstance->_world->getDynamicRaster(rasterAttributes.name);

        setBaseClassAttributes(dynamicRaster, rasterAttributes);

        dynamicRaster.setDefaultValue(rasterAttributes.defaultValue);
        dynamicRaster.setCurrentMinValue(rasterAttributes.currentMinValue);
        dynamicRaster.setCurrentMaxValue(rasterAttributes.currentMaxValue);
    }

    void SaveState::readRasterValues(std::ifstream& cpFileStream, const std::string& name, const bool& isDynamicRaster)
    {
// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::readRasterValues INIT\n").str();
        StaticRaster& staticRaster = _schedulerInstance->_world->getStaticRaster(name);

        std::string line;
        std::vector<std::string> tokens;
        
        std::getline(cpFileStream, line);
        tokens = getLineTokens(line, ' ');

        bool readingMaxValues = false;
        if (tokens[0].compare("MAX_VALUES:") == 0) readingMaxValues = true;

        if (tokens[0].compare("VALUES:") == 0 or tokens[0].compare("MAX_VALUES:") == 0)
        {
            int width = std::stoi(tokens[1]);
            int height = std::stoi(tokens[2]);

// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::readRasterValues width: " << width << "\theight: " << height << "\n").str();

            for (int i = 0; i < width; ++i)
            {
                std::getline(cpFileStream, line);
                tokens = getLineTokens(line, ' ');

// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::readRasterValues line " << i << " (raster = " << name << "): " << line << "\n").str();

                for (int j = 0; j < tokens.size(); ++j)
                {
                    if (tokens[j].compare("*") != 0)
                    {
                        int value = std::stoi(tokens[j]);
                        Point2D<int> position = Point2D<int>(j, i);

                        if (not isDynamicRaster) 
                            _schedulerInstance->_world->getStaticRaster(name).setValue(position, value);
                        else
                        {
                            if (readingMaxValues) _schedulerInstance->_world->getDynamicRaster(name).setMaxValue(position, value);
                            else _schedulerInstance->_world->getDynamicRaster(name).setValue(position, value);
                        }
                    }
                }
            }
        }
    }

    void SaveState::loadRasters(std::ifstream& cpFileStream)
    {
// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] loadRasters LOADING RASTERS\n").str();
        std::string line;
        std::vector<std::string> tokens;

        while (std::getline(cpFileStream, line))
        {
            if (line.find("END_DATA") != std::string::npos) break;
            
            tokens = getLineTokens(line, ' ');
            if (tokens[0].compare("Number_of_rasters:") == 0)
            {
                int numberOfRastersToLoad = std::stoi(tokens[1]);
// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::loadRasters numberOfRastersToLoad: " << numberOfRastersToLoad << "\n").str();
                for (int i = 0; i < numberOfRastersToLoad; ++i)
                {
                    bool isDynamicRaster = isRasterToLoadDynamic(cpFileStream);

                    RasterAttributes rasterAttributes;
                    std::vector<std::vector<int>> values, maxValues;

// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::loadRasters GOING TO readRasterBasicAttributes FOR RASTER: " << i << "\n").str();

                    readRasterBasicAttributes(cpFileStream, rasterAttributes, isDynamicRaster);
// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] SaveState::loadRasters AFTER readRasterBasicAttributes()\tisDynamicRaster: " << isDynamicRaster << "\n").str();
                    if (not isDynamicRaster)
                        createAndRegisterStaticRaster(rasterAttributes);
                    else
                        createAndRegisterDynamicRaster(rasterAttributes);
                    readRasterValues(cpFileStream, rasterAttributes.name, isDynamicRaster);
                }
            }
        }
// std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] loadRasters RASTERS LOADED\n").str();
    }
    
    std::string SaveState::getAgentType(const std::string& encodedAgent) const
    {
        unsigned int typePosition = encodedAgent.find_first_of("_");
        return encodedAgent.substr(0, typePosition);
    }

    void SaveState::loadAgents(std::ifstream& cpFileStream)
    {
        std::string line;
        while (std::getline(cpFileStream, line))
        {
            if (line.find("END_DATA") != std::string::npos) break;

            std::string agentType = getAgentType(line);
            Agent* agent = CheckpointingFactory::instance().decodeAndFillAgent(agentType, line);

            _schedulerInstance->_world->addAgent(agent);
        }
    }

    void SaveState::loadCheckpoint()
    {
        std::string fileToLoad = Engine::GeneralState::loggerCP().buildFileRelativePath(_fileNameCP);

        std::ifstream cpFileStream(fileToLoad);
        std::string line;

        while (std::getline(cpFileStream, line))
        {
//std::cout << CreateStringStream("[Process #" << _schedulerInstance->getId() << "] loadCheckpoint::line " << line << "\n").str();
            if (line.find("Step_finished") != std::string::npos)
                loadWorldInfo(cpFileStream);
            else if (line.find("Node_data") != std::string::npos)
                loadNodeInfo(cpFileStream);
            else if (line.find("Rasters_data") != std::string::npos)
                loadRasters(cpFileStream);
            else if (line.find("Agents_data") != std::string::npos)
                loadAgents(cpFileStream);
        }

        _schedulerInstance->_world->updateDiscreteStateStructures();
    }

} // namespace Engine