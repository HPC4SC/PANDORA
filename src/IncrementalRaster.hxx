
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
#ifndef __IncrementalRaster_hxx__
#define __IncrementalRaster_hxx__

#include <DynamicRaster.hxx>
#include <map>
#include <Size.hxx>

namespace Engine
{

class IncrementalRaster : public DynamicRaster
{

private:
    typedef std::map<Point2D<int>, int> ChangeTable;
    typedef ChangeTable::const_iterator    ChangeIterator;

    ChangeTable _changes; //! Changes made in the raster.
    const DynamicRaster * _baseRaster; //! Base of the current raster.
    int    _currentMinValue; //! Current maximum value of the raster.
    int    _currentMaxValue; //! Current minimum value of the raster.

public:
    /**
     * @brief Construct a new IncrementalRaster object.
     * 
     */
    IncrementalRaster( );

    /**
     * @brief Construct a new Incremental Raster object.
     * 
     * @param baseRaster DynamicRaster, base of the IncrementalRatser.
     */
    IncrementalRaster( const DynamicRaster& baseRaster );

    /**
     * @brief Construct a new Incremental Raster object.
     * 
     * @param other IncrementalRaster, base of the IncrementalRatser.
     */
    IncrementalRaster( const IncrementalRaster& other );

    /**
     * @brief Destroy the Incremental Raster object.
     * 
     */
    virtual ~IncrementalRaster( );

    /**
     * @brief Resize the IncrementalRaster.
     * 
     * @param size New size of the raster.
     */
    virtual void  resize(  const Size<int> & size );

    /**
     * @brief Set the value of the specified position.
     * 
     * @param pos Position to be updated.
     * @param value New Value.
     */
    void setValue( const Point2D<int> & pos, int value );

    /**
     * @brief Get the value of the raster in the specified position.
     * 
     * @param pos Position to check.
     * @return const int& 
     */
    const int& getValue( const Point2D<int> & pos ) const;
    int getMaxValue( const Point2D<int> & position ) const;

    int            getCurrentMinValue( ) const { return _currentMinValue; }
    int            getCurrentMaxValue( ) const { return _currentMaxValue; }

    //virtual    void        updateCurrentMinMaxValues( );

    ChangeIterator        firstChange( ) const { return _changes.begin( ); }
    ChangeIterator        endOfChanges( ) const { return _changes.end( ); }
    Size<int> getSize( ) const;

    bool operator==( const IncrementalRaster& other ) const;
    bool operator!=( const IncrementalRaster& other ) const
    {
        return !( this->operator==( other ));
    }

    bool operator<( const IncrementalRaster& other ) const
    {
        return _changes.size( ) < other._changes.size( );
    }

};

}

#endif // IncrementalRaster.hxx
