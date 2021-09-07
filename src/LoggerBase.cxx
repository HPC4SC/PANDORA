
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

#include <LoggerBase.hxx>

#include <boost/filesystem.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

namespace Engine
{

Logger::Logger( ) : _logsDir( "./logs" )
{
}

Logger::~Logger( )
{
    for ( FilesMap::iterator it=_files.begin( ); it!=_files.end( ); ++it )
    {
         std::ofstream * file = it->second;
         file->close();
         delete file;
    }
}

std::string Logger::getKeyRelativePath(const std::string& key) const
{
    std::stringstream fileName;
    if (not _logsDir.empty())
        fileName << _logsDir << "/";
    fileName << key << ".log";

    return fileName.str();
}

void Logger::closeFile(const std::string& fileName)
{
    FilesMap::iterator it = _files.find(fileName);
    if (it != _files.end())
    {
        std::ofstream* file = it->second;
        file->close();
        delete file;

        _files.erase(it);
    }
}

std::ofstream & Logger::log( const std::string & fileName )
{
    FilesMap::iterator it = _files.find( fileName );
    std::ofstream * file = 0;
    // create a new file if it is closed
    if ( it==_files.end( ) )
    {
        if (not _logsDir.empty())
            boost::filesystem::create_directory(_logsDir);
        
        _files.insert( make_pair( fileName, new std::ofstream( getKeyRelativePath(fileName).c_str( ) ) ));
        it = _files.find( fileName );
    }
    file = it->second;
    return *file;
}

void Logger::setLogsDir( const std::string & logsDir )
{
    _logsDir = logsDir;
    if ( !_logsDir.empty( ) )
    {
        boost::filesystem::create_directories( _logsDir );
    }
}

bool Logger::checkFileExistance(const std::string& key) const
{
    std::ifstream file(getKeyRelativePath(key));
    return file.good();
}

std::string Logger::buildFileRelativePath(const std::string& key) const
{
    return getKeyRelativePath(key);
}

} // namespace Engine

