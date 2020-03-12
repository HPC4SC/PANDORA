/*
 * Copyright ( c ) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
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

#ifndef __RasterLoader_hxx__
#define __RasterLoader_hxx__

#include <string>
#include <Rectangle.hxx>

namespace Engine
{
    class StaticRaster;
    class World;

    class RasterLoader
    {
    public:
        /**
         * @brief Construct a new RasterLoader object.
         * 
         */
        RasterLoader( );

        /**
         * @brief Destroy the RasterLoader object.
         * 
         */
        virtual ~RasterLoader( );

        /**
         * @brief Load a GDAL file conforming World position.
         * 
         * @param raster Raster that will store the input information.
         * @param fileName Route to the file holding the information.
         * @param definedBoundaries Boundaries of the raster.
         */
        void fillGDALRaster( StaticRaster & raster, const std::string & fileName, const Rectangle<int> & definedBoundaries = Rectangle<int>( ) );

        /**
         * @brief Load an HDF5 conforming adjusting raster to data, or to World position if not null.
         * 
         * @param raster Raster that will store the input information.
         * @param fileName Name of the file.
         * @param pathToData Route to the file holding the information.
         * @param world Pointer to the world of ther raster.
         */
        void fillHDF5RasterDirectPath( StaticRaster & raster, const std::string & fileName, const std::string & pathToData, World * world );

        /**
         * @brief Load an HDF5 from a serialized dynamic raster at a given time step.
         * 
         * @param raster Raster that will store the input information.
         * @param fileName Name of the file.
         * @param rasterName Name of the raster.
         * @param step Current step of the simulation.
         * @param world Pointer to the world of ther raster.
         */
        void fillHDF5Raster( StaticRaster & raster, const std::string & fileName, const std::string & rasterName, int step, World * world = 0 );

        /**
         * @brief Load an HDF5 from a serialized static raster.
         * 
         * @param raster Raster that will store the input information.
         * @param fileName Name of the file.
         * @param rasterName Name of the raster.
         * @param world Pointer to the world of ther raster.
         */
        void fillHDF5Raster( StaticRaster & raster, const std::string & fileName, const std::string & rasterName, World * world = 0 );
    };
} // namespace Engine
#endif // __RasterLoader_hxx__
