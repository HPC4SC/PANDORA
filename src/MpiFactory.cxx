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

#include <MpiFactory.hxx>

namespace Engine
{
    MpiFactory * MpiFactory::_instance = 0;

    MpiFactory * MpiFactory::instance( )
    {
        if ( !_instance )
        {
            _instance = new MpiFactory( );
        }
        return _instance;
    }

    MpiFactory::MpiFactory( )
    {
    }

    MpiFactory::~MpiFactory( )
    {
    }

    void MpiFactory::cleanTypes( )
    {
        for ( TypesMap::iterator it=_types.begin( ); it!=_types.end( ); ++it )
        {
            MPI_Datatype * type = it->second;
            MPI_Type_free( type );
        }
    }

    MPI_Datatype* MpiFactory::getMPIType(std::string typeString)
    {
        return _types.at(typeString);
    }

    MpiFactory::TypesMap::iterator MpiFactory::beginTypes( )
    {
        return _types.begin( );
    }

    MpiFactory::TypesMap::iterator MpiFactory::endTypes( )
    {
        return _types.end( );
    }

    int MpiFactory::getIDFromTypeName(const std::string& typeName) const
    {
        return _typesMapNameToID.at(typeName);
    }

    std::string MpiFactory::getNameFromTypeID(const int& typeID) const
    {
        return _typesMapIDToName.at(typeID);
    }

} // namespace Engine
