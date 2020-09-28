
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

#ifndef __StaticRaster_hxx__
#define __StaticRaster_hxx__

#include <Point2D.hxx>
#include <Size.hxx>
#include <vector>

namespace Engine
{
class World;
class RasterLoader;

struct ColorEntry
{
    short _r;
    short _g;
    short _b;
    short _alpha;

    bool operator==( const ColorEntry& other ) const {
        return _r == other._r && _g == other._g && _b == other._b && _alpha == other._alpha;
    }
    bool operator!=( const ColorEntry& other ) const { return !( *this == other ); }
};

//! this class is used to load a static raster map. Values can't be modified, and it won't be serialized each time step ( only one time )
class StaticRaster
{

    friend class RasterLoader;

protected:


    // Variables for raster creation (MPI mainly)
    int _id;
    std::string _name;
    bool _serialize;
    std::string _fileName;

    // Class variables
    std::vector<std::vector<int>> _values;          //! Matrix of values of the raster.
    std::vector<std::vector<int>> _discreteValues;  //! Matrix of discrete values of the raster (current step 'i', not 'i+1').

    int _layer;                                     //! Layer which this raster belongs to.

    int _minValue;                                  //! Minimum value of the raster.
    int _maxValue;                                  //! Maximum value of the raster.

    bool _hasColorTable; //! True the raster has a color table, otherwise a color table is not set.
    std::vector< ColorEntry > _colorTable; //! Color pallet of the raster.

public:

    /**
     * @brief Construct a new Static Raster object.
     * 
     */
    StaticRaster( );

    /**
     * @brief Construct a new Static Raster object.
     * 
     * @param id const int&
     * @param name const std::string&
     * @param serialize const bool&
     */
    StaticRaster(const int& id, const std::string& name, const bool& serialize);

    /**
     * @brief Destroy the Static Raster object
     * 
     */
    virtual ~StaticRaster( );

    /**
     * @brief Gets the _id (index in World) member.
     * 
     * @return int 
     */
    int getID() const;

    /**
     * @brief Gets the _name member.
     * 
     * @return std::string 
     */
    std::string getName() const;

    /**
     * @brief Get the _fileName member.
     * 
     * @return std::string 
     */
    std::string getFileName() const;

    /**
     * @brief Gets the _layer member.
     * 
     * @return const int& 
     */
    const int& getLayer() const;

    /**
     * @brief Get the _serialize member.
     * 
     * @return bool
     */
    bool getSerialize() const;

    /**
     * @brief Set the _fileName member.
     * 
     * @param fileName Set the _fileName member.
     */
    void setFileName(const std::string& fileName);

    /**
     * @brief Sets the _layer member.
     * 
     * @param layer 
     */
    void setLayer(const int& layer);

    /**
     * @brief Equality operator between two rasters.
     * 
     * @param other Raster to compare with.
     * @return true 
     * @return false 
     */
    bool operator==( const StaticRaster& other ) const; 

    /**
     * @brief Non-equality operator between two rasters.
     * 
     * @param other Raster to compare with.
     * @return true 
     * @return false 
     */
    bool operator!=( const StaticRaster& other ) const;

    /**
     * @brief Changes raster size.
     * 
     * @param size Represents the new dimesions for the raster area.
     */
    virtual void resize( const Size<int> & size );
    
    /**
     * @brief Changes the size of the _discreteValues member.
     * 
     * @param size const Size<int>&
     */
    virtual void resizeDiscrete(const Size<int>& size);

    /**
     * @brief Gets the value in the cell located by parameter "position". Returns -1 if "position" is out of the area of the raster.
     * 
     * @param position Position of the raster whose value we need.
     * @return const int& 
     */
    virtual const int & getValue( const Point2D<int>& position ) const;

    /**
     * @brief Gets the 'position' in _discreteValues attribute.
     * 
     * @param position const Point2D<int>&
     * @return const int& 
     */
    virtual const int& getDiscreteValue(const Point2D<int>& position) const;

    /**
     * @brief Returns size of the raster codifying the horizontal and vertical dimensions in a Size object.
     * 
     * @return Size<int> 
     */
    virtual Size<int> getSize( ) const;
     
    /**
     * @brief Gets attribute _maxValue.
     * 
     * @return const int& 
     */
    virtual const int & getMaxValue( ) const;
    
    /**
     * @brief Gets the '_minValue' attribute.
     * 
     * @return const int& 
     */
    const int & getMinValue( ) const;

    /**
     * @brief Gets the avg value for the 'matrixOfValues' representing a raster.
     * 
     * @param matrixOfValues const std::vector<std::vector<int>>&
     * @return float 
     */
    float getAvgValueBase(const std::vector<std::vector<int>>& matrixOfValues) const;

    /**
     * @brief Gets the average value of the raster.
     * 
     * @return float 
     */
    float getAvgValue( ) const;
    
    /**
     * @brief Gets the average value for the raster for its _discreteValues.
     * 
     * @return float 
     */
    float getAvgValueDiscrete() const;

    /**
     * @brief Updates the new minumim and maximum values of the raster.
     * 
     */
    void updateMinMaxValues( );

    /**
     * @brief Set the color table.
     * 
     * @param hasColorTable Specifies if a color table is already set.
     * @param size Size of the raster.
     */
    void setColorTable( bool hasColorTable, int size );

    /**
     * @brief Adds a color entry to the _colorTable.
     * 
     * @param index Index of the new entry.
     * @param r Red component.
     * @param g Green component.
     * @param b Blue component.
     * @param alpha Aplha of the entry.
     */
    void addColorEntry( int index, short r, short g, short b, short alpha );
    
    /**
     * @brief Get the length of the raster color table.
     * 
     * @return int 
     */
    int getNumColorEntries( ) const;

    /**
     * @brief True if a color table is set. False otehrwise.
     * 
     * @return true 
     * @return false 
     */
    bool hasColorTable( ) const;

    /**
     * @brief Get the color entry in the index position.
     * 
     * @param index Position of the checked entry.
     * @return ColorEntry 
     */
    ColorEntry getColorEntry( int index ) const;

    /**
     * @brief Updates the member matrix _discreteValues, copying all values in _continuousValues to it.
     * 
     */
    void copyContinuousValuesToDiscreteOnes();

};

} // namespace Engine

#endif // __StaticRaster_hxx__

