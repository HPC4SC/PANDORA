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

    void OpenMPIMultiNodeLogs::printPartitionsBeforeMPI(const OpenMPIMultiNode& schedulerInstance) const
    {
        std::stringstream ss;
        ss << "TS = " << schedulerInstance.getWallTime() << ":" << std::endl;
        ss << "Number of real tasks: " << schedulerInstance._numTasks << std::endl;
        ss << "Number of working nodes: " << schedulerInstance._numTasks - int(not schedulerInstance._assignLoadToMasterNode) << std::endl;
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

        log_DEBUG(schedulerInstance._logFileNames.at(schedulerInstance.getId()), ss.str());
    }

    void OpenMPIMultiNodeLogs::printOwnNodeStructureAfterMPI(const OpenMPIMultiNode& schedulerInstance) const
    {
        std::stringstream ss;
        ss << "TS = " << schedulerInstance.getWallTime() << ":" << std::endl;
        ss << "OWN STRUCTURES AFTER MPI:" << std::endl;
        ss << "ownedAreaWithoutInnerOverlap: " << schedulerInstance._nodeSpace.ownedAreaWithoutInnerOverlap << std::endl;
        ss << "ownedArea: " << schedulerInstance._nodeSpace.ownedArea << std::endl;
        ss << "ownedAreaWithOuterOverlaps: " << schedulerInstance._nodeSpace.ownedAreaWithOuterOverlaps << std::endl;
        for (std::map<int, OpenMPIMultiNode::MPINode*>::const_iterator it = schedulerInstance._nodeSpace.neighbours.begin(); it != schedulerInstance._nodeSpace.neighbours.end(); ++it)
        {
            ss << "NeighbourID: " << it->first << "\tCoordinates: " << it->second->ownedArea << std::endl;
        }
        ss << std::endl;

        log_DEBUG(schedulerInstance._logFileNames.at(schedulerInstance.getId()), ss.str());
    }

    void OpenMPIMultiNodeLogs::printNeighbouringAgentsPerTypes(const OpenMPIMultiNode& schedulerInstance) const
    {
        std::stringstream ss;
        ss << "NEIGHBOURING AGENTS PER TYPE:" << std::endl;
        for (OpenMPIMultiNode::NeighbouringAgentsMap::const_iterator it = schedulerInstance._neighbouringAgents.begin(); it != schedulerInstance._neighbouringAgents.end(); ++it)
        {
            ss << "Type: " << it->first << std::endl;
            for (std::map<int, AgentsList>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) 
            {
                ss << "\tNode: " << it2->first << std::endl;
                ss << "\t  ";
                for (AgentsList::const_iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
                {
                    ss << it3->get()->getId() << ", ";
                }
                ss << std::endl;
            }
            ss << std::endl;
        }

        log_DEBUG(schedulerInstance._logFileNames.at(schedulerInstance.getId()), ss.str());
    }
    
    void OpenMPIMultiNodeLogs::printNodeAgents(const OpenMPIMultiNode& schedulerInstance) const
    {
        std::stringstream ss;
        ss << "TS = " << schedulerInstance.getWallTime() << ":" << std::endl;
        ss << "NODE AGENTS:" << std::endl;
        for (AgentsList::const_iterator it = schedulerInstance._world->beginAgents(); it != schedulerInstance._world->endAgents(); ++it)
        {
            ss << it->get()->getId() << ", ";
        }
        ss << std::endl;

        log_DEBUG(schedulerInstance._logFileNames.at(schedulerInstance.getId()), ss.str());
    }

    void OpenMPIMultiNodeLogs::printNodeRasters(const OpenMPIMultiNode& schedulerInstance) const
    {
        std::stringstream ss;
        ss << "TS = " << schedulerInstance.getWallTime() << ":" << std::endl;
        ss << "Total number of rasters = " << schedulerInstance._world->getNumberOfRasters() << std::endl;
        ss << "NODE STATIC RASTERS:" << std::endl;
        for (int i = 0; i < schedulerInstance._world->getNumberOfRasters(); ++i)
        {
            if (schedulerInstance._world->rasterExists(i) and not schedulerInstance._world->isRasterDynamic(i))
            {
                DynamicRaster* raster = static_cast<DynamicRaster*>(&(schedulerInstance._world->getStaticRaster(i)));
                ss << "Raster #" << i << ": " << schedulerInstance._world->getRasterNameFromID(i) << std::endl;
                ss << getRasterValues(*raster, schedulerInstance);
            }
        }
        ss << std::endl;

        ss << "NODE DYNAMIC RASTERS:" << std::endl;
        for (int i = 0; i < schedulerInstance._world->getNumberOfRasters(); ++i)
        {
            if (schedulerInstance._world->rasterExists(i) and schedulerInstance._world->isRasterDynamic(i))
            {
                DynamicRaster raster = schedulerInstance._world->getDynamicRaster(i);
                ss << "Raster #" << i << ": " << schedulerInstance._world->getRasterNameFromID(i) << std::endl;
                ss << getRasterValues(raster, schedulerInstance);
            }
            ss << std::endl;
        }

        ss << std::endl;
        log_DEBUG(schedulerInstance._logFileNames.at(schedulerInstance.getId()), ss.str());
    }

    /** PROTECTED METHODS **/

    std::string OpenMPIMultiNodeLogs::getRasterValues(const DynamicRaster& raster, const OpenMPIMultiNode& schedulerInstance) const
    {
        std::stringstream ss;
        for (int i = 0; i < raster.getSize().getHeight(); ++i)
        {
            for (int j = 0; j < raster.getSize().getWidth(); ++j)
            {
                Point2D<int> point = Point2D<int>(j, i);
                if (schedulerInstance._nodeSpace.ownedAreaWithOuterOverlaps.contains(point)) ss << raster.getValue(point) << " ";
                else ss << "-" << " ";
            }
            ss << std::endl;
        }
        return ss.str();
    }

} // namespace Engine