
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

#ifndef __Point2D_hxx__
#define __Point2D_hxx__

#include <sstream>
#include <cmath>

namespace Engine
{

//! Basic pointing object
/*!
  This class contains x/y coordinates, as well as needed utilities ( arithmetics, distance, etc. )
  It is templatised by the type of coordinates
*/
template <typename Type> class Point2D
{
public:
    Type _x; //! X axis position.
    Type _y; //! Y axis position.

    /**
     * @brief Construct a new Point2D object with _x = 0 and _y = 0.
     * 
     */
    Point2D( ) : _x( 0 ), _y( 0 )
    {
    }

    /**
     * @brief Construct a new Point2D object with _x = x and _y = y.
     * 
     * @param x Initial x axis position.
     * @param y Initial y axis position.
     */
    Point2D( const Type & x, const Type & y ) : _x( x ), _y( y )
    {
    }

    /**
     * @brief Clones the calling Point2D.
     * 
     * @return Point2D<Type> 
     */
    Point2D<Type> clone( ) const
    {
        return Point2D<Type>( _x, _y );
    }

    /**
     * @brief Shape output of Point2D.
     * 
     * @param stream Data stream.
     * @param point Point to be printed.
     * @return std::ostream& 
     */
    friend std::ostream & operator<<( std::ostream & stream, const Point2D<Type> & point )
    {
        return stream << point._x << "/" << point._y;
    }

    /**
     * @brief Equality within an square area around the caller object.
     * 
     * @param point is considered equal if it falls in the rectangle ( (_x-delta, _y-delta )..( _x+delta, _y+delta )). 
     * Assuming ( (a, b )..( c, d )) a rectangle with top left corner ( a, b ) and bottom right corner ( c, d ).
     * @param delta Error.
     * @return true 
     * @return false 
     */
    bool isEqual( const Point2D<Type> & point, const double & delta = 0.0001 ) const
    {
        return distance( point )<=delta;
    }

    /**
     * @brief Point2D Assignation.
     * 
     * @param point Value of the Point2D
     * @return Point2D<Type>& 
     */
    Point2D<Type> & operator=( const Point2D<Type> & point )
    {
        _x = point._x;
        _y = point._y;
        return *this;
    }

    /**
     * @brief Equality of two points.
     * 
     * @param point Point2D to compare with.
     * @return true 
     * @return false 
     */
    bool operator==( const Point2D<Type> & point ) const
    {
        return isEqual( point );
    }

    /**
     * @brief Non equality of two poits.
     * 
     * @param point Point2D to compare with.
     * @return true 
     * @return false 
     */
    bool operator!=( const Point2D<Type>& point ) const
    {
        return !isEqual( point );
    }

    /**
     * @brief Sum coordinates of the param to the coordinates of the caller object.
     * 
     * @param point Point2D to sum.
     * @return Point2D<Type>& 
     */
    Point2D<Type> & operator+=( const Point2D<Type> & point )
    {
        _x += point._x;
        _y += point._y;
        return *this;
    }

    /**
     * @brief Substraction coordinates of the param to the coordinates of the caller object.
     * 
     * @param point Point2D to substract.
     * @return Point2D<Type>& 
     */
    Point2D<Type> & operator-=( const Point2D<Type> & point )
    {
        _x -= point._x;
        _y -= point._y;
        return *this;
    }

    /**
     * @brief Sum arithmethic operation of a Point2D and a gerneral type.
     * 
     * @param value Value to be summed.
     * @return Point2D<Type> 
     */
    Point2D<Type> operator+( const Type & value ) const
    {
        return Point2D<Type>( _x+value, _y+value );
    }

    /**
     * @brief Substraction arithmethic operation of a Point2D and a gerneral type.
     * 
     * @param value Value to be substracted.
     * @return Point2D<Type> 
     */
    Point2D<Type> operator-( const Type & value ) const
    {
        return Point2D<Type>( _x-value, _y-value );
    }

    /**
     * @brief Multiplication arithmethic operation of a Point2D and a gerneral type.
     * 
     * @param value Value to be multiplied.
     * @return Point2D<Type> 
     */
    Point2D<Type> operator*( const Type & value ) const
    {
        return Point2D<Type>( _x*value, _y*value );
    }

    /**
     * @brief Division arithmethic operation of a Point2D and a gerneral type.
     * 
     * @param value Value to be devied by.
     * @return Point2D<Type> 
     */
    Point2D<Type> operator/( const Type & value ) const
    {
        return Point2D<Type>( _x/value, _y/value );
    }

    /**
     * @brief Sum arithmethic operation of two Point2D.
     * 
     * @param point Point2D to be summed.
     * @return Point2D<Type> 
     */
    Point2D<Type> operator+( const Point2D<Type> & point ) const
    {
        return Point2D<Type>( _x+point._x, _y+point._y );
    }

    /**
     * @brief Substraction arithmethic operation of two Point2D.
     * 
     * @param point Point2D to be substracted.
     * @return Point2D<Type> 
     */
    Point2D<Type> operator-( const Point2D<Type> & point ) const
    {
        return Point2D<Type>( _x-point._x, _y-point._y );
    }

    /**
     * @brief Multiplication arithmethic operation of two Point2D.
     * 
     * @param point Point2D to be multiplied.
     * @return Point2D<Type> 
     */
    Point2D<Type> operator*( const Point2D<Type> & point ) const
    {
        return Point2D<Type>( _x*point._x, _y*point._y );
    }

    /**
     * @brief Division arithmethic operation of two Point2D.
     * 
     * @param point Point2D to be devied by.
     * @return Point2D<Type> 
     */
    Point2D<Type> operator/( const Point2D<Type> & point ) const
    {
        return Point2D<Type>( _x/point._x, _y/point._y );
    }

    /**
     * @brief Checks if the calling point is lesser than the given one.
     * 
     * @param point Point2D to compare with.
     * @return true 
     * @return false 
     */
    bool operator<( const Point2D<Type>& point ) const
    {
        return ( _x < point._x ) || ( (_x == point._x ) && ( _y < point._y ) );
    }

    /**
     * @brief Metric between two Point2D.
     * 
     * @param point Point2D needed.
     * @return Type 
     */
    Type dot( const Point2D<Type> & point ) const
    {
        return _x*point._x + _y*point._y;
    }

    /**
     * @brief Euclidean Distance between points.
     * 
     * @param point Point2D to check the distance with.
     * @return double 
     */
    double distance( const Point2D<Type> & point ) const
    {
        double valueX = _x-point._x;
        double valueY = _y-point._y;
        return sqrt( valueX*valueX + valueY*valueY );
    }
};

} // namespace Engine

#endif // __Point2D_hxx__

