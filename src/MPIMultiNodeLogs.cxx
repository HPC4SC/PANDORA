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

#include <MPIMultiNodeLogs.hxx>

#include <Logger.hxx>
#include <Exception.hxx>

namespace Engine {

    /** PUBLIC METHODS **/

    MPIMultiNodeLogs::MPIMultiNodeLogs()
    {
    }

    MPIMultiNodeLogs::~MPIMultiNodeLogs()
    {
    }

    void MPIMultiNodeLogs::initFilesName(const MPIMultiNode& schedulerInstance)
    {
        _schedulerInstance = &schedulerInstance;

        _logFileName = CreateStringStream("MPIProcess_" << _schedulerInstance->getId()).str();
        _instrumentationLogFileName = CreateStringStream("InstrumentationProcess_" << _schedulerInstance->getId()).str();
    }

    void MPIMultiNodeLogs::writeInDebugFile(const std::string& message)
    {
        log_DEBUG(_logFileName, message);
    }

    std::string MPIMultiNodeLogs::getString_PartitionsBeforeMPI() const
    {
        std::stringstream ss;
        //ss << "TS = " << _schedulerInstance->getWallTime() << ":" << std::endl;
        ss << "Number of real tasks: " << _schedulerInstance->_numTasks << std::endl;
        ss << "Number of working nodes: " << _schedulerInstance->_numTasks << std::endl;
        ss << "Overlap size: " << _schedulerInstance->_overlapSize << std::endl << std::endl;
        ss << "PARTITIONS BEFORE MPI:" << std::endl;
        for (MPINodesMap::const_iterator it = _schedulerInstance->_mpiNodesMapToSend.begin(); it != _schedulerInstance->_mpiNodesMapToSend.end(); ++it) 
        {
            ss << "Partition: " << it->first << "\tCoordinates: " << it->second.ownedArea << std::endl;
            for (std::map<int, MPINode*>::const_iterator it2 = it->second.neighbours.begin(); it2 != it->second.neighbours.end(); ++it2)
            {
                ss << "\tNeighbour " << it2->first << ": " << it2->second->ownedArea << std::endl;
            }
            ss << std::endl;
        }

        return ss.str();
    }

    std::string MPIMultiNodeLogs::getString_OwnNodeStructure() const
    {
        std::stringstream ss;
        //ss << "TS = " << _schedulerInstance->getWallTime() << ":" << std::endl;
        ss << "ownedAreaWithoutInnerOverlap: " << _schedulerInstance->_nodeSpace.ownedAreaWithoutInnerOverlap << std::endl;
        ss << "ownedArea: " << _schedulerInstance->_nodeSpace.ownedArea << std::endl;
        ss << "ownedAreaWithOuterOverlap: " << _schedulerInstance->_nodeSpace.ownedAreaWithOuterOverlap << std::endl;
        for (std::map<int, MPINode*>::const_iterator it = _schedulerInstance->_nodeSpace.neighbours.begin(); it != _schedulerInstance->_nodeSpace.neighbours.end(); ++it)
        {
            ss << "NeighbourID: " << it->first << "\townedArea:\t\t\t\t\t\t" << it->second->ownedArea << std::endl;
            ss << "\t\t\t\townedAreaWithOuterOverlap:\t\t" << it->second->ownedAreaWithOuterOverlap << std::endl;
            ss << "\t\t\t\townedAreaWithoutInnerOverlap:\t" << it->second->ownedAreaWithoutInnerOverlap << std::endl;
        }
        ss << std::endl;

        ss << "Inner sub overlaps: " << std::endl;
        for (std::map<int, Rectangle<int>>::const_iterator it = _schedulerInstance->_nodeSpace.innerSubOverlaps.begin(); it != _schedulerInstance->_nodeSpace.innerSubOverlaps.end(); ++it)
        {
            ss << "Overlap type: " << it->first << " " << it->second << std::endl;
        }
        ss << std::endl;

        ss << "Inner sub overlaps neighbours: " << std::endl;
        for (std::map<int, std::list<int>>::const_iterator it = _schedulerInstance->_nodeSpace.innerSubOverlapsNeighbours.begin(); it != _schedulerInstance->_nodeSpace.innerSubOverlapsNeighbours.end(); ++it)
        {
            ss << "Neighbours for sub overlap " << it->first << ": ";
            for (std::list<int>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
                ss << *it2 << ", ";
            ss << std::endl;
        }
        ss << std::endl;

        return ss.str();
    }
    
    std::string MPIMultiNodeLogs::getString_NodeAgents(const bool& fullDescription) const
    {
        std::stringstream ss;
        //ss << "TS = " << _schedulerInstance->getWallTime() << ":" << std::endl;
        ss << "NODE AGENTS (" << _schedulerInstance->_world->getNumberOfAgents() << "):" << std::endl;
        for (AgentsMap::const_iterator it = _schedulerInstance->_world->beginAgents(); it != _schedulerInstance->_world->endAgents(); ++it)
        {
            AgentPtr agentPtr = it->second;
            if (agentPtr.get()->exists())
            {
                if (fullDescription)
                    ss << agentPtr.get() << std::endl;
                else
                    ss << agentPtr.get()->getId() << ", ";
            }
        }
        ss << std::endl;

        return ss.str();
    }

    std::string MPIMultiNodeLogs::getString_NodeRasters(const bool& discrete) const
    {
        std::stringstream ss;
        //ss << "TS = " << _schedulerInstance->getWallTime() << ":" << std::endl;
        ss << "Total number of rasters = " << _schedulerInstance->_world->getNumberOfRasters() << std::endl;

        if (discrete) ss << "NODE STATIC RASTERS / DISCRETE VALUES:" << std::endl;
        else ss << "NODE STATIC RASTERS / CONTINUOUS VALUES:" << std::endl;

        for (int i = 0; i < _schedulerInstance->_world->getNumberOfRasters(); ++i)
        {
            if (_schedulerInstance->_world->rasterExists(i) and not _schedulerInstance->_world->isRasterDynamic(i))
            {
                DynamicRaster* raster = static_cast<DynamicRaster*>(&(_schedulerInstance->_world->getStaticRaster(i)));
                ss << "Raster #" << i << ": " << _schedulerInstance->_world->getRasterNameFromID(i) << std::endl;
                ss << getRasterValues(*raster, discrete);
            }
        }
        ss << std::endl;

        if (discrete) ss << "NODE DYNAMIC RASTERS / DISCRETE VALUES:" << std::endl;
        else ss << "NODE DYNAMIC RASTERS / CONTINUOUS VALUES:" << std::endl;

        for (int i = 0; i < _schedulerInstance->_world->getNumberOfRasters(); ++i)
        {
            if (_schedulerInstance->_world->rasterExists(i) and _schedulerInstance->_world->isRasterDynamic(i))
            {
                DynamicRaster raster = _schedulerInstance->_world->getDynamicRaster(i);
                ss << "Raster #" << i << ": " << _schedulerInstance->_world->getRasterNameFromID(i) << std::endl;
                ss << getRasterValues(raster, discrete);
            }
            ss << std::endl;
        }
        ss << std::endl;

        return ss.str();
    }

    std::string MPIMultiNodeLogs::getString_AgentsMatrix(const bool& printAllMatrix) const
    {
        std::stringstream ss;
        ss << "AGENTS MATRIX:" << std::endl;
        const AgentsMatrix& agentsMatrix = _schedulerInstance->_world->getAgentsMatrix();
        for (int i = 0; i < agentsMatrix.size(); ++i)
        {
            for (int j = 0; j < agentsMatrix[i].size(); ++j)
            {
                Point2D<int> point = Point2D<int>(j, i);
                if (printAllMatrix or _schedulerInstance->_nodeSpace.ownedAreaWithOuterOverlap.contains(point))
                {
                    if (agentsMatrix[j][i].empty()) ss << "-----";

                    bool first = true;
                    for (AgentsMap::const_iterator it = agentsMatrix[j][i].begin(); it != agentsMatrix[j][i].end(); ++it)
                    {
                        if (not first) ss << "+";
                        ss << it->second.get()->getId();

                        first = false;
                    }
                }
                else ss << "-";
                ss << "\t";
            }
            ss << "\n";
        }
        return ss.str();
    }

    void MPIMultiNodeLogs::printPartitionsBeforeMPIInDebugFile() const
    {
        log_DEBUG(_logFileName, getString_PartitionsBeforeMPI());
    }

    void MPIMultiNodeLogs::printOwnNodeStructureInDebugFile() const
    {
        log_DEBUG(_logFileName, getString_OwnNodeStructure());
    }

    void MPIMultiNodeLogs::printNodeAgentsInDebugFile(const bool& fullDescription) const
    {
        log_DEBUG(_logFileName, getString_NodeAgents(fullDescription));
    }

    void MPIMultiNodeLogs::printNodeRastersInDebugFile() const
    {
        log_DEBUG(_logFileName, getString_NodeRasters(false));
    }

    void MPIMultiNodeLogs::printNodeRastersDiscreteInDebugFile() const
    {
        log_DEBUG(_logFileName, getString_NodeRasters(true));
    }

    void MPIMultiNodeLogs::printAgentsMatrixInDebugFile(const bool& printAllMatrix) const
    {
        log_DEBUG(_logFileName, getString_AgentsMatrix(printAllMatrix));
    }

    void MPIMultiNodeLogs::printInstrumentation(const std::string& message)
    {
        log_DEBUG(_instrumentationLogFileName, message);
    }

    /** PROTECTED METHODS **/

    std::string MPIMultiNodeLogs::getRasterValues(const DynamicRaster& raster, const bool& discrete) const
    {
        std::stringstream ss;
        for (int i = 0; i < raster.getSize().getHeight(); ++i)
        {
            for (int j = 0; j < raster.getSize().getWidth(); ++j)
            {
                Point2D<int> point = Point2D<int>(j, i);
                if (_schedulerInstance->_nodeSpace.ownedAreaWithOuterOverlap.contains(point)) 
                {
                    if (discrete) ss << raster.getDiscreteValue(point);
                    else ss << raster.getValue(point);
                    ss << " ";
                }
                else ss << "-" << " ";
            }
            ss << std::endl;
        }
        return ss.str();
    }

} // namespace Engine