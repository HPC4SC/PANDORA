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

#ifndef __Size_hxx__
#define __Size_hxx__

#include <sstream>
#include <cmath>

namespace Engine
{

    template <typename Type> class Size
    {
        public:
            Type _width; //! Width of the rectangle.
            Type _height; //! Height of the rectangle.

            /**
             * @brief Construct a new Size object
             * 
             */
            Size( ) : _width( 0 ), _height( 0 )
            {
            }

            /**
             * @brief Construct a new Size object
             * 
             * @param width width of the Size object
             * @param height height of the Size object
             */
            Size( const Type & width, const Type & height ) : _width( width ), _height( height )
            {
            }

            /**
             * @brief Get the _width member.
             * 
             * @return const Type& 
             */
            const Type& getWidth() const
            {
                return _width;
            }

            /**
             * @brief Get the _height member.
             * 
             * @return const Type& 
             */
            const Type& getHeight() const
            {
                return _height;
            }

            /**
             * @brief Clones the calling size.
             * 
             * @return Size<Type> 
             */
            Size<Type> clone( ) const
            {
                return Size<Type>( _width, _height );
            }

            /**
             * @brief Shape output of Size.
             * 
             * @param stream Data stream.
             * @param size Point to be printed.
             * @return std::ostream& 
             */
            friend std::ostream & operator<<( std::ostream & stream, const Size<Type> & size )
            {
                return stream << size._width << "/" << size._height;
            }

            /**
             * @brief Check if the calling size is equal to the given one.
             * 
             * @param size Size compated with.
             * @param delta Error of the comparison.
             * @return true 
             * @return false 
             */
            bool isEqual( const Size<Type> & size, const double & delta = 0.0001 ) const
            {
                return distance( size )<=delta;
            }

            /**
             * @brief Euclidean Distance between sizes
             * 
             * @param size Size we want to konw the distance with.
             * @return double 
             */
            double distance( const Size<Type> & size ) const
            {
                double diffWidth = _width-size._width;
                double diffHeight = _height-size._height;
                return sqrt( diffWidth*diffWidth+diffHeight*diffHeight );
            }

            /**
             * @brief Equality of two sizes.
             * 
             * @param size Size to compare with.
             * @return true 
             * @return false 
             */
            bool operator==( const Size<Type> & size ) const
            {
                return isEqual( size );
            }

            /**
             * @brief Non equality of two sizes.
             * 
             * @param size Size to compare with.
             * @return true 
             * @return false 
             */
            bool operator!=( const Size<Type>& size ) const
            {
                return !isEqual( size );
            }

            /**
             * @brief Adds the given value to the size components.
             * 
             * @param value Added value.
             * @return Size<Type> 
             */
            Size<Type> operator+( const Type & value ) const
            {
                return Size<Type>( _width+value, _height+value );
            }

            /**
             * @brief Substracts the given value to the size components.
             * 
             * @param value Substracted value.
             * @return Size<Type> 
             */
            Size<Type> operator-( const Type & value ) const
            {
                return Size<Type>( _width-value, _height-value );
            }

            /**
             * @brief Multiplies by the given value to the size components.
             * 
             * @param value Multiplied by value.
             * @return Size<Type> 
             */
            Size<Type> operator*( const Type & value ) const
            {
                return Size<Type>( _width*value, _height*value );
            }

            /**
             * @brief Devided by the given value to the size components.
             * 
             * @param value Devided by value.
             * @return Size<Type> 
             */
            Size<Type> operator/( const Type & value ) const
            {
                return Size<Type>( _width/value, _height/value );
            }

            /**
             * @brief Adds the given point value to the size components.
             * 
             * @param point Added point.
             * @return Size<Type> 
             */
            Size<Type> operator+( const Size<Type> & point ) const
            {
                return Size<Type>( _width+point._width, _height+point._height );
            }

            /**
             * @brief Substracts the given point value to the size components.
             * 
             * @param point Substracted point.
             * @return Size<Type> 
             */
            Size<Type> operator-( const Size<Type> & point ) const
            {
                return Size<Type>( _width-point._width, _height-point._height );
            }

            /**
             * @brief Multiplies by the given value to the size components.
             * 
             * @param point Multiplied by point.
             * @return Size<Type> 
             */
            Size<Type> operator*( const Size<Type> & point ) const
            {
                return Size<Type>( _width*point._width, _height*point._height );
            }

            /**
             * @brief Devided by the given value to the size components.
             * 
             * @param point Devided by point.
             * @return Size<Type> 
             */
            Size<Type> operator/( const Size<Type> & point ) const
            {
                return Size<Type>( _width/point._width, _height/point._height );
            }
            
    };

} // namespace Engine

#endif // __Size_hxx__

