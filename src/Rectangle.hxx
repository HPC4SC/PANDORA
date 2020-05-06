
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

#ifndef __Rectangle_hxx__
#define __Rectangle_hxx__

#include <sstream>
#include <cmath>

#include <cstdlib>

#include <Interval.hxx>
#include <Point2D.hxx>
#include <Size.hxx>

namespace Engine
{
// 2D Rectangle
template <typename Type> class Rectangle
{
public:
    Point2D<Type> _origin; //! Top left point.
    Size<Type> _size; //! Limit of the Rectangle.

    /**
     * @brief Construct a new Rectangle object with default parameters.
     * 
     */
    Rectangle( ) : _origin( -1, -1 ), _size( -1, -1 )
    {
    }

    /**
     * @brief Construct a new Rectangle object.
     * 
     * @param size Initial size of the Rectangle.
     * @param origin Initial origin of the Rectangle.
     */
    Rectangle( const Size<Type> & size, const Point2D<Type> & origin = Point2D<Type>( 0, 0 )) : _origin( origin ), _size( size )
    {
    }

    /**
     * @brief Construct a new Rectangle object.
     * 
     * @param left Left limit of the Rectangle.
     * @param top Top limit of the Rectangle.
     * @param right Right limit of the Rectangle.
     * @param bottom Bottom limit of the Rectangle.
     */
    Rectangle( const Type & left, const Type & top, const Type & right, const Type & bottom ) : _origin( left, top ), _size( 1+right-left, 1+bottom-top )
    {
    }

    /**
     * @brief Construct a new Rectangle object.
     * 
     * @param OX First interval.
     * @param OY Second interval.
     */
    Rectangle( const Interval<Type> & OX, const Interval<Type> & OY )
    {
        this->intervals2Rectangle( OX, OY );
    }

    /**
     * @brief Returts if "point" is contained by the Rectangle.
     * 
     * @param point Point2D we want to check.
     * @return true 
     * @return false 
     */
    bool contains( const Point2D<Type> & point ) const
    {
        if ( point._x < _origin._x || point._x >= _origin._x+_size._width )
        {
            return false;
        }
        if ( point._y < _origin._y || point._y >= _origin._y+_size._height )
        {
            return false;
        }
        return true;
    }

    /**
     * @brief Converts two intervals to a Rectangle.
     * 
     * @param iOX First Interval.
     * @param iOY Second Interval.
     */
    void intervals2Rectangle( const Interval<Type> & iOX, const Interval<Type> & iOY )
    {
        // Point2D origin = Point2D( iOX._min, iOY._min );
        _origin._x = iOX._min;
        _origin._y = iOY._min;

        // Point2D size = Point2D( iOX._max – iOX._min +1, iOY._max – iOY._min +1 );
        _size._width = iOX._max - iOX._min +1;
        _size._height = iOY._max - iOY._min +1;
    }

    /**
     * @brief Retruns if two Rectangles intersect.
     * 
     * @param other Rectangle to check.
     * @param result Rectangle resulting of the intersection.
     * @return true 
     * @return false 
     */
    bool intersection( const Rectangle<Type> other, Rectangle<Type> & result )
    {
        // Interval caracterization for Rectangle "this"
        Interval<Type> R1_OX( _origin._x, _origin._x + _size._width -1 );
        Interval<Type> R1_OY( _origin._y, _origin._y + _size._height -1 );

        // Interval caracterization for Rectangle "other"
        Interval<Type> R2_OX( other._origin._x, other._origin._x + other._size._width -1 );
        Interval<Type> R2_OY( other._origin._y, other._origin._y + other._size._height -1 );

        Interval<Type> Intersection_OX;
        if ( ! R1_OX.intersection( R2_OX, Intersection_OX ) )
        {
            return false;
        }


        Interval<Type> Intersection_OY;
        if ( ! R1_OY.intersection( R2_OY, Intersection_OY ) )
        {
            return false;
        }

        result.intervals2Rectangle( Intersection_OX, Intersection_OY );

        return true;
    }

    /**
     * @brief Shape of the Rectangle of output.
     * 
     * @param stream Stream of data.
     * @param rectangle Rectangle to be printed.
     * @return std::ostream& 
     */
    friend std::ostream & operator<<( std::ostream & stream, const Rectangle<Type> & rectangle )
    {
        return stream << "rect:" << rectangle._origin << "->" << Point2D<Type>( rectangle._origin._x+rectangle._size._width, rectangle._origin._y+rectangle._size._height ) - 1;
    }

    /**
     * @brief Assignation of a Rectangle.
     * 
     * @param rectangle Value assigned.
     * @return Rectangle<Type>& 
     */
    Rectangle<Type> & operator=( const Rectangle<Type> & rectangle )
    {
        _origin = rectangle._origin;
        _size = rectangle._size;
        return *this;
    }

    /**
     * @brief Equality between two Rectangles.
     * 
     * @param rectangle Rectangle to compare with.
     * @return true 
     * @return false 
     */
    bool operator==( const Rectangle<Type> & rectangle ) const
    {
        return _origin==rectangle._origin && _size==rectangle._size;
    }

    /**
     * @brief Non equality between two Rectangles.
     * 
     * @param rectangle Rectangle to compare with.
     * @return true 
     * @return false 
     */
    bool operator!=( const Rectangle<Type> & rectangle ) const
    {
        return !( (*this )==rectangle );
    }

    /**
     * @brief Clones the calling Rectangle.
     * 
     * @return Rectangle<Type> 
     */
    Rectangle<Type> clone( ) const
    {
        return Rectangle<int>( _size, _origin );
    }

    /**
     * @brief Return the leftmost X coordinate of the Rectangle
     * 
     * @return Type 
     */
    Type left( ) const{return _origin._x;}

    /**
     * @brief Return the rightmost X coordinate of the Rectangle
     * 
     * @return Type 
     */
    Type right( ) const{return _origin._x+_size._width-1;}

    /**
     * @brief Return the topmost Y coordinate of the Rectangle
     * 
     * @return Type 
     */
    Type top( ) const{return _origin._y;}

    /**
     * @brief Return the bottommost Y coordinate of the Rectangle
     * 
     * @return Type 
     */
    Type bottom( ) const{return _origin._y+_size._height-1;}

public:

    class const_iterator
    {
        Type _initialX; //! First X axis position.
        Point2D<Type> _data; //! Data type of the const_iterator
        Type _width; //! Number of position const_iterator can check.
    public:
        /**
         * @brief Construct a new const_iterator object.
         * 
         * @param origin Origin Point2D of the iterator.
         * @param width Number of position const_iterator can check
         */
        const_iterator( const Point2D<Type> & origin, const Type & width ) : _initialX( origin._x ), _data( origin ), _width( width )
        {
        }

        /**
         * @brief Construct a new const_iterator object.
         * 
         * @param other Const_iterator base of the new one.
         */
        const_iterator( const const_iterator & other ) : _initialX( other._initialX ), _data( other._data ), _width( other._width )
        {
        }

        /**
         * @brief Returns the _data.
         * 
         * @return const Point2D<Type>& 
         */
        const Point2D<Type> & operator*( ) const
        {
            return _data;
        }

        /**
         * @brief Retuns a pounter to _data.
         * 
         * @return const Point2D<Type>* 
         */
        const Point2D<Type>* operator->( ) const
        {
            return &_data;
        }

        /**
         * @brief Increments the value of _data._x by one unit.
         * 
         * @return const_iterator& 
         */
        const_iterator & operator++( )
        {
            if ( _data._x<( _initialX+_width-1 ))
            {
                _data._x++;
            }
            else
            {
                _data._x = _initialX;
                _data._y++;
            }
            return *this;
        }

        /**
         * @brief Increments the value od const_iterator by one unit.
         * 
         * @return const_iterator 
         */
        const_iterator operator++( int )
        {
            const_iterator tmp( *this );
            operator++( );
            return tmp;
        }

        /**
         * @brief Non equality operator between two const_iterators.
         * 
         * @param other Const_iterator we are comparing with.
         * @return true 
         * @return false 
         */
        bool operator!=( const const_iterator & other ) const
        {
            return ( _initialX!=other._initialX ) || ( _width!=other._width ) || ( _data != other._data );
        }
    };

    /**
     * @brief Returns a const_iterator pointing to the origin of the Rectangle.
     * 
     * @return const_iterator 
     */
    const_iterator begin( ) const
    {
        return const_iterator( _origin, _size._width );
    }

    /**
     * @brief Rtuens a const_iterator pointing to the origin of the Rectangle.
     * 
     * @return const_iterator 
     */
    const_iterator end( ) const
    {
        return const_iterator( _origin+Point2D<Type>( 0, _size._height ), _size._width );
    }
};

} // namespace Engine

#endif // __Rectangle_hxx__

