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

#ifndef __MpiFactory_hxx__
#define __MpiFactory_hxx__

#include <string>
#include <mpi.h>
#include <map>

namespace Engine
{
    class Agent;

    // this class is a factory method that creates Mpi related code
    // source for the factory is automatically created from a python script in particular examples
    class MpiFactory
    {
    public:
        typedef std::map< std::string, MPI_Datatype *> TypesMap;

    private:
        static MpiFactory * _instance;

        MpiFactory( );

        TypesMap _types;

    public:
        static MpiFactory * instance( );
        virtual ~MpiFactory( );

        /**
         * @brief method defined by script that adds custom mpi types for each agent.
         * 
         */
        void registerTypes( );
        
        /**
         * @brief method to delete from mpi stack all the created types.
         * 
         */
        void cleanTypes( );

        void * createDefaultPackage( const std::string & type );
        Agent * createAndFillAgent( const std::string & type, void * package );
        
        /**
         * @brief returns the first position of the typesMap.
         * 
         * @return TypesMap::iterator.
         */
        TypesMap::iterator beginTypes( );

        /**
         * @brief returns the last position of the typesMap.
         * 
         * @return TypesMap::iterator 
         */
        TypesMap::iterator endTypes( );
    };
} // namespace Engine
#endif // __MpiFactory_hxx__
