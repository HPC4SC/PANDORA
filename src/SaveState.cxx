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

#include <boost/filesystem.hpp>

namespace Engine
{
    
    SaveState::SaveState()
    {
    }

    SaveState::~SaveState()
    {
    }

    void SaveState::initCheckpointFileNames(const MPIMultiNode& schedulerInstance)
    {
        _schedulerInstance = &schedulerInstance;

        std::string fileNameCP_base = _schedulerInstance->_world->getConfig().getFileNameCP();
        _fileNameCP = CreateStringStream(fileNameCP_base << "_" << _schedulerInstance->getId()).str();
    }

    void SaveState::saveNodeSpace() const
    {
        log_CP(_fileNameCP, CreateStringStream(_schedulerInstance->_nodeSpace.ownedAreaWithoutInnerOverlap).str());
        log_CP(_fileNameCP, CreateStringStream(_schedulerInstance->_nodeSpace.ownedArea).str());
        log_CP(_fileNameCP, CreateStringStream(_schedulerInstance->_nodeSpace.ownedAreaWithOuterOverlap).str());
        for (std::map<int, MPINode*>::const_iterator it = _schedulerInstance->_nodeSpace.neighbours.begin(); it != _schedulerInstance->_nodeSpace.neighbours.end(); ++it)
        {
            int neighbourID = it->first;
            MPINode* neighbourNode = it->second;

            log_CP(_fileNameCP, CreateStringStream(neighbourID).str());
            log_CP(_fileNameCP, CreateStringStream(neighbourNode->ownedAreaWithoutInnerOverlap).str());
            log_CP(_fileNameCP, CreateStringStream(neighbourNode->ownedArea).str());
            log_CP(_fileNameCP, CreateStringStream(neighbourNode->ownedAreaWithOuterOverlap).str());
        }

        for (std::map<int, Rectangle<int>>::const_iterator it = _schedulerInstance->_nodeSpace.innerSubOverlaps.begin(); it != _schedulerInstance->_nodeSpace.innerSubOverlaps.end(); ++it)
        {
            int suboverlapID = it->first;
            Rectangle<int> suboverlap = it->second;

            log_CP(_fileNameCP, CreateStringStream(suboverlapID << " " << suboverlap).str());
        }

        for (std::map<int, std::list<int>>::const_iterator it = _schedulerInstance->_nodeSpace.innerSubOverlapsNeighbours.begin(); it != _schedulerInstance->_nodeSpace.innerSubOverlapsNeighbours.end(); ++it)
        {
            int suboverlapID = it->first;

            std::stringstream ss;
            ss << suboverlapID << "|";
            for (std::list<int>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                int neighbourID = *it2;
                ss << suboverlapID << " ";
            }
            log_CP(_fileNameCP, ss.str());
        }
    }

    void SaveState::saveSchedulerAttributes() const
    {
        log_CP(_fileNameCP, CreateStringStream( _schedulerInstance->_masterNodeID << " " << _schedulerInstance->_numberOfActiveProcesses << " ").str());
    }

    void SaveState::startCheckpointing() const
    {
        log_CP(_fileNameCP, CreateStringStream("Step finished: " << _schedulerInstance->_world->getCurrentStep() << " (time:" << _schedulerInstance->_world->getWallTime() << ")").str());

        saveNodeSpace();
        saveSchedulerAttributes();
    }

    void SaveState::saveRastersInCPFile()
    {
        int numberOfRasters = _schedulerInstance->_world->getNumberOfRasters();
        Rectangle<int> knownBoundaries = _schedulerInstance->_world->getBoundariesWithoutOverlaps();

        log_CP(_fileNameCP, CreateStringStream("Number_of_rasters:" << numberOfRasters).str());
        for (int i = 0; i < numberOfRasters; ++i)
        {
            if (not _schedulerInstance->_world->isRasterDynamic(i))
            {
                StaticRaster& staticRaster = _schedulerInstance->_world->getStaticRaster(i);
                log_CP(_fileNameCP, CreateStringStream("STATIC").str());
                log_CP(_fileNameCP, CreateStringStream(staticRaster.getRasterGeneralInfo()).str());
                log_CP(_fileNameCP, CreateStringStream("\nVALUES:").str());
                log_CP(_fileNameCP, CreateStringStream(staticRaster.getRasterValues(knownBoundaries)).str());
            }
            else
            {
                DynamicRaster& dynamicRaster = _schedulerInstance->_world->getDynamicRaster(i);
                log_CP(_fileNameCP, CreateStringStream("DYNAMIC").str());
                log_CP(_fileNameCP, CreateStringStream(dynamicRaster.getRasterGeneralInfo()).str());
                log_CP(_fileNameCP, CreateStringStream("\nVALUES:").str());
                log_CP(_fileNameCP, CreateStringStream(dynamicRaster.getRasterValues(knownBoundaries)).str());
                log_CP(_fileNameCP, CreateStringStream("\nMAX VALUES:").str());
                log_CP(_fileNameCP, CreateStringStream(dynamicRaster.getRasterMaxValues(knownBoundaries)).str());
            }
            log_CP(_fileNameCP, CreateStringStream("").str());
        }
    }

    void SaveState::saveAgentsInCPFile()
    {
        for (AgentsMap::const_iterator it = _schedulerInstance->_world->beginAgents(); it != _schedulerInstance->_world->endAgents(); ++it)
        {
            AgentPtr agentPtr = it->second;

            log_CP(_fileNameCP, CreateStringStream(agentPtr->encodeAllAttributesInString()).str());
        }
    }

} // namespace Engine