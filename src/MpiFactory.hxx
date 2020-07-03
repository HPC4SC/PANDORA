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
        typedef std::map< std::string, int> TypesMapNameToID;
        typedef std::map< int, std::string> TypesMapIDToName;

    private:
        
        static MpiFactory * _instance; //! Unique instance of the MpiFactory.

        /**
         * @brief Construct a new Mpi Factory object.
         * 
         */
        MpiFactory( );

        TypesMap _types; //! Map of the different Agent types.
        TypesMapNameToID _typesMapNameToID;
        TypesMapIDToName _typesMapIDToName;

    public:
        
        /**
         * @brief If there's no instance of MpiFactory one is created and retured. Otherwise the _instance member is returned.
         * 
         * @return MpiFactory* 
         */
        static MpiFactory * instance( );

        /**
         * @brief Destroy the Mpi Factory object.
         * 
         */
        virtual ~MpiFactory( );

        /**
         * @brief Method defined by script that adds custom mpi types for each agent.
         * 
         */
        void registerTypes( );
        
        /**
         * @brief Method to delete from mpi stack all the created types.
         * 
         */
        void cleanTypes( );

        /**
         * @brief Gets the MPI_Datatype from an agent type (TypesMap[typeString])
         * 
         * @param typeString std::string
         * @return MPIDatatype*
         */
        MPI_Datatype* getMPIType(std::string typeString);

        /**
         * @brief Create a Default Package object
         * 
         * @param type Type of the package created.
         * @return void* 
         */
        void * createDefaultPackage( const std::string & type );

        /**
         * @brief Creates and initializes a recieved Agent.
         * 
         * @param type Type of the Agent.
         * @param package Data of the Agent.
         * @return Agent* 
         */
        Agent * createAndFillAgent( const std::string & type, void * package );
        
        /**
         * @brief Returns the first position of the typesMap.
         * 
         * @return TypesMap::iterator.
         */
        TypesMap::iterator beginTypes( );

        /**
         * @brief Returns the last position of the typesMap.
         * 
         * @return TypesMap::iterator 
         */
        TypesMap::iterator endTypes( );

        /**
         * @brief Gets the ID of the agent type from its name 'typeName'.
         * 
         * @param typeName const std::string&
         * @return int 
         */
        int getIDFromTypeName(const std::string& typeName) const;

        /**
         * @brief Gets the name of the agent type from its ID 'typeID'.
         * 
         * @param typeID const int&
         * @return std::string 
         */
        std::string getNameFromTypeID(const int& typeID) const;

    };
} // namespace Engine
#endif // __MpiFactory_hxx__
