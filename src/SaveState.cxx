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
    
    SaveState::SaveState() : _world(0)
    {
    }

    SaveState::~SaveState()
    {
    }

    void SaveState::setWorld(World* world)
    {
        _world = world;
    }

    void SaveState::initCheckpointing()
    {
        _fileNameCP = _world->getConfig().getFileNameCP();

        log_CP(_fileNameCP, CreateStringStream("Step finished: " << _world->getCurrentStep() << " " << _world->getWallTime()).str());
    }

    void SaveState::saveRastersInCPFile()
    {
        int numberOfRasters = _world->getNumberOfRasters();
        Rectangle<int> knownBoundaries = _world->getBoundariesWithoutOverlaps();

        log_CP(_fileNameCP, CreateStringStream(numberOfRasters).str());
        for (int i = 0; i < numberOfRasters; ++i)
        {
            if (not _world->isRasterDynamic(i))
            {
                StaticRaster& staticRaster = _world->getStaticRaster(i);
                log_CP(_fileNameCP, CreateStringStream("STATIC").str());
                log_CP(_fileNameCP, CreateStringStream(staticRaster.getRasterGeneralInfo()).str());
                log_CP(_fileNameCP, CreateStringStream("\nVALUES:").str());
                log_CP(_fileNameCP, CreateStringStream(staticRaster.getRasterValues(knownBoundaries)).str());
            }
            else
            {
                DynamicRaster& dynamicRaster = _world->getDynamicRaster(i);
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
        for (AgentsMap::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            AgentPtr agentPtr = it->second;

            log_CP(_fileNameCP, CreateStringStream(agentPtr->encodeAllAttributesInString()).str());
        }
    }

} // namespace Engine