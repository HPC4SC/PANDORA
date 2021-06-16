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
        std::string fileNameCP = _world->getConfig().getFileNameCP();

        log_CP(fileNameCP, CreateStringStream("Step finished: " << _world->getCurrentStep() << "\n").str());
    }

    std::string SaveState::getRasterValues(const DynamicRaster& raster) const
    {
        std::stringstream ss;
        for (int i = 0; i < raster.getSize().getHeight(); ++i)
        {
            for (int j = 0; j < raster.getSize().getWidth(); ++j)
            {
                Point2D<int> point = Point2D<int>(j, i);
                if (_world->pointStrictlyBelongsToWorld(point)) 
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

    void SaveState::saveRastersInCPFile()
    {
        std::string fileNameCP = _world->getConfig().getFileNameCP() + std::to_string(_world->getId());

        for (int i = 0; i < _world->getNumberOfRasters(); ++i)
        {
            DynamicRaster& raster;
            if (_world->isRasterDynamic(i)) raster = _world->getDynamicRaster(i);
            else raster = (DynamicRaster&) _world->getStaticRaster(i);

            log_CP(fileNameCP, CreateStringStream("RasterInfo: " << raster.getID() << "," << raster.getName() << "," << raster.getSerialize() << "\n").str());
            log_CP(fileNameCP, CreateStringStream(getRasterValues << "\n\n").str());
        }
    }

    void SaveState::saveAgentsInCPFile()
    {

    }

} // namespace Engine