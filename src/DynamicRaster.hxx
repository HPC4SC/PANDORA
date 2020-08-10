
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

#ifndef __DynamicRaster_hxx__
#define __DynamicRaster_hxx__

#include <Point2D.hxx>
#include <Size.hxx>
#include <StaticRaster.hxx>

namespace Engine
{

//! DynamicRaster adds mechanisms to modify the values of the raster map. It is serialized each time step.
class DynamicRaster : public StaticRaster
{
private:

    // Variables for raster creation (MPI mainly)
    int _defaultValue;

    // Class variables
    std::vector< std::vector<int> > _maxValues; //! Matrix of the maximun values of the raster.
    int    _currentMaxValue; //! Current maximum value of the raster.
    int    _currentMinValue; //! Current minimum value of the raster.

public:
    /**
     * @brief Construct a new Dynamic Raster object.
     * 
     */
    DynamicRaster( );

    /**
     * @brief Construct a new Dynamic Raster object.
     * 
     * @param id const id&
     * @param name const std::string&
     * @param serialize const bool&
     */
    DynamicRaster(const int& id, const std::string name, const bool& serialize);

    /**
     * @brief Destroy the Dynamic Raster object
     * 
     */
    virtual ~DynamicRaster( );

    /**
     * @brief Get the _defaultValue member.
     * 
     * @return int 
     */
    int getDefaultValue() const;

    bool operator==( const DynamicRaster& other ) const; //! Equality operators
    bool operator!=( const DynamicRaster& other ) const; //! Equality operators

    /**
     * @brief Increases each cell value by 1 if it is under the maximum allowed.
     * 
     */
    void updateRasterIncrement( );
    
    /**
     * @brief Assigns to each cell in raster the max value allowed for it.
     * 
     */
    void updateRasterToMaxValues( );

    /**
     * @brief Gets attribute _maxValue.
     * 
     * @return const int& 
     */
    const int & getMaxValue( ) const;

    /**
     * @brief Reads the maximum allowed value in the cell located by parameter "position". Returns -1 if "position" is out of the area of the raster.
     * 
     * @param position Position of the raster checked.
     * @return int 
     */
    virtual int getMaxValue( const Point2D<int>& position ) const;

    /**
     * @brief Assigns the value "value" to the cell located by parameter "position". Does nothing if "position" is out of the area of the raster.
     * 
     * @param position Position where the maxValue is set.
     * @param value New maxValue.
     */
    virtual void setValue( const Point2D<int>& position, int value );
    
    /**
     * @brief Changes the maximum value allowed in the cell located by parameter "position" to the new amount "value". 
     * Does nothing if "position" is out of the area of the raster.
     * 
     * @param position Position where the maxValue is set.
     * @param value New maxValue.
     */
    void setMaxValue( const Point2D<int>& position, int value );

    /**
     * @brief  Initializes the components of vector '_values' to defaultValue, and to maxValue the ones from vector _maxValue.
     * 
     * @param minValue Minimum value of the raster.
     * @param maxValue Maximum value of the raster.
     * @param defaultValue Default value of the raster.
     */
    void setInitValues( int minValue, int maxValue, int defaultValue );
    //! 
    /**
     * @brief Sets new value for attribute _maxValue of the raster.
     * 
     * @param maxValue New value of _maxValue.
     */
    void setMaxValue( const int & maxValue );
    
    /**
     * @brief Sets new value for attribute minValue of the raster.
     * 
     * @param minValue New value of _minValue.
     */
    void setMinValue( const int & minValue );

    /**
     * @brief Resizes the Raster with the new size.
     * 
     * @param size New raster size.
     */
    void resize( const Size<int> & size );

    /**
     * @brief Sets the _currentMaxValue value to the maximum numeric limit and _currentMinValue to the minimum numeric limit.
     * 
     */
    void updateCurrentMinMaxValues( );

    /**
     * @brief Get the _currentMinValue.
     * 
     * @return int 
     */
    int  getCurrentMinValue( ) const { return _currentMinValue; }

    /**
     * @brief Get the _currentMaxValue.
     * 
     * @return int 
     */
    int  getCurrentMaxValue( ) const { return _currentMaxValue; }

    friend class RasterLoader;
};

} // namespace Engine

#endif // __DynamicRaster_hxx__

