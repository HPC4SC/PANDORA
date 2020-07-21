
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

#ifndef __Exception_hxx__
#define __Exception_hxx__

#include <exception>
#include <string>

namespace Engine
{

class Exception : public std::exception
{
public:
    #define CreateStringStream(VALUE) static_cast<std::ostringstream&&>(std::ostringstream() << VALUE)

private:
    std::string _error; //! Error message of the exception.
    
public:
    /**
     * @brief Construct a new Exception.
     * 
     * @param error Error message.
     */
    Exception( const std::string & error ) : _error( error ){}

    /**
     * @brief Destroy the Exception object.
     * 
     */
    virtual ~Exception( ) throw( ) {}

    /**
     * @brief Throws the capturated exception.
     * 
     * @return const char* 
     */
    virtual const char* what( ) const throw( )
    {
        return _error.c_str( );
    }

};

} // namespace Engine

#endif // __Exception_hxx__

