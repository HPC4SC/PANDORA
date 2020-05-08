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
 */

#ifndef __LoggerBase_hxx__
#define __LoggerBase_hxx__

#include <map>
#include <fstream>

namespace Engine
{

class Logger
{
private:
    typedef std::map <std::string, std::ofstream *> FilesMap;

    FilesMap _files; //! Map of different log files.
    std::string _logsDir; //! Route to the logs directories.
public:
    /**
     * @brief Construct a new Logger object.
     * 
     */
    Logger( );

    /**
     * @brief Destroy the Logger object.
     * 
     */
    virtual ~Logger( );
    
    /**
     * @brief Message to file.
     * 
     * @param fileName Name o fthe log file.
     * @return std::ofstream& 
     */
    std::ofstream & log( const std::string & fileName );

    /**
     * @brief Set the _logsDir attribute.
     * 
     * @param logsDir New logs directory.
     */
    void setLogsDir( const std::string & logsDir );
};

} // namespace Engine

#endif // __LoggerBase_hxx__

