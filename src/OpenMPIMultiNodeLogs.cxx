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

#include <OpenMPIMultiNodeLogs.hxx>

#include <Logger.hxx>

namespace Engine {

    /** PUBLIC METHODS **/

    OpenMPIMultiNodeLogs::OpenMPIMultiNodeLogs()
    {
    }

    OpenMPIMultiNodeLogs::~OpenMPIMultiNodeLogs()
    {
    }

    void OpenMPIMultiNodeLogs::initLogFileNames(const OpenMPIMultiNode& schedulerInstance)
    {
        for (int i = 0; i < schedulerInstance._numTasks; ++i)
        {
            std::stringstream ssMPIProcesses;
            ssMPIProcesses << "MPIProcess_" << i;
            _logFileNames[i] = ssMPIProcesses.str();

            std::stringstream ssInstrumentation;
            ssInstrumentation << "InstrumentationProcess_" << i;
            _instrumentationLogFileNames[i] = ssInstrumentation.str();
        }
    }

    void OpenMPIMultiNodeLogs::writeInDebugFile(const std::string& message, const OpenMPIMultiNode& schedulerInstance)
    {
        log_DEBUG(_logFileNames.at(schedulerInstance.getId()), message);
    }

    std::string OpenMPIMultiNodeLogs::getString_PartitionsBeforeMPI(const OpenMPIMultiNode& schedulerInstance) const
    {
        std::stringstream ss;
        //ss << "TS = " << schedulerInstance.getWallTime() << ":" << std::endl;
        ss << "Number of real tasks: " << schedulerInstance._numTasks << std::endl;
        ss << "Number of working nodes: " << schedulerInstance._numTasks << std::endl;
        ss << "Overlap size: " << schedulerInstance._overlapSize << std::endl << std::endl;
        ss << "PARTITIONS BEFORE MPI:" << std::endl;
        for (std::map<int, OpenMPIMultiNode::MPINode>::const_iterator it = schedulerInstance._mpiNodesMapToSend.begin(); it != schedulerInstance._mpiNodesMapToSend.end(); ++it) 
        {
            ss << "Partition: " << it->first << "\tCoordinates: " << it->second.ownedArea << std::endl;
            for (std::map<int, OpenMPIMultiNode::MPINode*>::const_iterator it2 = it->second.neighbours.begin(); it2 != it->second.neighbours.end(); ++it2)
            {
                ss << "\tNeighbour " << it2->first << ": " << it2->second->ownedArea << std::endl;
            }
            ss << std::endl;
        }

        return ss.str();
    }

    std::string OpenMPIMultiNodeLogs::getString_OwnNodeStructureAfterMPI(const OpenMPIMultiNode& schedulerInstance) const
    {
        std::stringstream ss;
        //ss << "TS = " << schedulerInstance.getWallTime() << ":" << std::endl;
        ss << "OWN STRUCTURES AFTER MPI:" << std::endl;
        ss << "ownedAreaWithoutInnerOverlap: " << schedulerInstance._nodeSpace.ownedAreaWithoutInnerOverlap << std::endl;
        ss << "ownedArea: " << schedulerInstance._nodeSpace.ownedArea << std::endl;
        ss << "ownedAreaWithOuterOverlaps: " << schedulerInstance._nodeSpace.ownedAreaWithOuterOverlaps << std::endl;
        for (std::map<int, OpenMPIMultiNode::MPINode*>::const_iterator it = schedulerInstance._nodeSpace.neighbours.begin(); it != schedulerInstance._nodeSpace.neighbours.end(); ++it)
        {
            ss << "NeighbourID: " << it->first << "\tCoordinates: " << it->second->ownedArea << std::endl;
        }
        ss << std::endl;

        ss << "Inner sub overlaps: " << std::endl;
        for (std::map<int, Rectangle<int>>::const_iterator it = schedulerInstance._nodeSpace.innerSubOverlaps.begin(); it != schedulerInstance._nodeSpace.innerSubOverlaps.end(); ++it)
        {
            ss << "Overlap type: " << it->first << " " << it->second << std::endl;
        }
        ss << std::endl;

        ss << "Inner sub overlaps neighbours: " << std::endl;
        for (std::map<int, std::list<int>>::const_iterator it = schedulerInstance._nodeSpace.innerSubOverlapsNeighbours.begin(); it != schedulerInstance._nodeSpace.innerSubOverlapsNeighbours.end(); ++it)
        {
            ss << "Neighbours for sub overlap " << it->first << ": ";
            for (std::list<int>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
                ss << *it2 << ", ";
            ss << std::endl;
        }
        ss << std::endl;

        return ss.str();
    }
    
    std::string OpenMPIMultiNodeLogs::getString_NodeAgents(const OpenMPIMultiNode& schedulerInstance, const bool& fullDescription) const
    {
        std::stringstream ss;
        //ss << "TS = " << schedulerInstance.getWallTime() << ":" << std::endl;
        ss << "NODE AGENTS (" << schedulerInstance._world->getNumberOfAgents() << "):" << std::endl;
        for (AgentsMap::const_iterator it = schedulerInstance._world->beginAgents(); it != schedulerInstance._world->endAgents(); ++it)
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

    std::string OpenMPIMultiNodeLogs::getString_NodeRasters(const OpenMPIMultiNode& schedulerInstance, const bool& discrete) const
    {
        std::stringstream ss;
        //ss << "TS = " << schedulerInstance.getWallTime() << ":" << std::endl;
        ss << "Total number of rasters = " << schedulerInstance._world->getNumberOfRasters() << std::endl;

        if (discrete) ss << "NODE STATIC RASTERS / DISCRETE VALUES:" << std::endl;
        else ss << "NODE STATIC RASTERS / CONTINUOUS VALUES:" << std::endl;

        for (int i = 0; i < schedulerInstance._world->getNumberOfRasters(); ++i)
        {
            if (schedulerInstance._world->rasterExists(i) and not schedulerInstance._world->isRasterDynamic(i))
            {
                DynamicRaster* raster = static_cast<DynamicRaster*>(&(schedulerInstance._world->getStaticRaster(i)));
                ss << "Raster #" << i << ": " << schedulerInstance._world->getRasterNameFromID(i) << std::endl;
                ss << getRasterValues(*raster, schedulerInstance, discrete);
            }
        }
        ss << std::endl;

        if (discrete) ss << "NODE DYNAMIC RASTERS / DISCRETE VALUES:" << std::endl;
        else ss << "NODE DYNAMIC RASTERS / CONTINUOUS VALUES:" << std::endl;

        for (int i = 0; i < schedulerInstance._world->getNumberOfRasters(); ++i)
        {
            if (schedulerInstance._world->rasterExists(i) and schedulerInstance._world->isRasterDynamic(i))
            {
                DynamicRaster raster = schedulerInstance._world->getDynamicRaster(i);
                ss << "Raster #" << i << ": " << schedulerInstance._world->getRasterNameFromID(i) << std::endl;
                ss << getRasterValues(raster, schedulerInstance, discrete);
            }
            ss << std::endl;
        }
        ss << std::endl;

        return ss.str();
    }

    std::string OpenMPIMultiNodeLogs::getString_AgentsMatrix(const OpenMPIMultiNode& schedulerInstance, const bool& printAllMatrix) const
    {
        std::stringstream ss;
        ss << "AGENTS MATRIX:" << std::endl;
        const AgentsMatrix& agentsMatrix = schedulerInstance._world->getAgentsMatrix();
        for (int i = 0; i < agentsMatrix.size(); ++i)
        {
            for (int j = 0; j < agentsMatrix[i].size(); ++j)
            {
                Point2D<int> point = Point2D<int>(j, i);
                if (printAllMatrix or schedulerInstance._nodeSpace.ownedAreaWithOuterOverlaps.contains(point))
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

    void OpenMPIMultiNodeLogs::printPartitionsBeforeMPIInDebugFile(const OpenMPIMultiNode& schedulerInstance) const
    {
        log_DEBUG(_logFileNames.at(schedulerInstance.getId()), getString_PartitionsBeforeMPI(schedulerInstance));
    }

    void OpenMPIMultiNodeLogs::printOwnNodeStructureAfterMPIInDebugFile(const OpenMPIMultiNode& schedulerInstance) const
    {
        log_DEBUG(_logFileNames.at(schedulerInstance.getId()), getString_OwnNodeStructureAfterMPI(schedulerInstance));
    }

    void OpenMPIMultiNodeLogs::printNodeAgentsInDebugFile(const OpenMPIMultiNode& schedulerInstance, const bool& fullDescription) const
    {
        log_DEBUG(_logFileNames.at(schedulerInstance.getId()), getString_NodeAgents(schedulerInstance, fullDescription));
    }

    void OpenMPIMultiNodeLogs::printNodeRastersInDebugFile(const OpenMPIMultiNode& schedulerInstance) const
    {
        log_DEBUG(_logFileNames.at(schedulerInstance.getId()), getString_NodeRasters(schedulerInstance, false));
    }

    void OpenMPIMultiNodeLogs::printNodeRastersDiscreteInDebugFile(const OpenMPIMultiNode& schedulerInstance) const
    {
        log_DEBUG(_logFileNames.at(schedulerInstance.getId()), getString_NodeRasters(schedulerInstance, true));
    }

    void OpenMPIMultiNodeLogs::printAgentsMatrixInDebugFile(const OpenMPIMultiNode& schedulerInstance, const bool& printAllMatrix) const
    {
        log_DEBUG(_logFileNames.at(schedulerInstance.getId()), getString_AgentsMatrix(schedulerInstance, printAllMatrix));
    }

    void OpenMPIMultiNodeLogs::printInstrumentation(const OpenMPIMultiNode& schedulerInstance, const std::string& message)
    {
        log_DEBUG(_instrumentationLogFileNames.at(schedulerInstance.getId()), message);
    }

    /** PROTECTED METHODS **/

    std::string OpenMPIMultiNodeLogs::getRasterValues(const DynamicRaster& raster, const OpenMPIMultiNode& schedulerInstance, const bool& discrete) const
    {
        std::stringstream ss;
        for (int i = 0; i < raster.getSize().getHeight(); ++i)
        {
            for (int j = 0; j < raster.getSize().getWidth(); ++j)
            {
                Point2D<int> point = Point2D<int>(j, i);
                if (schedulerInstance._nodeSpace.ownedAreaWithOuterOverlaps.contains(point)) 
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