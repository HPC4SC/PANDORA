
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

#ifndef __Action_hxx__
#define __Action_hxx__

#include <string>

namespace Engine
{
    class Agent;

    class Action
    {

        public:
            Action( );
            virtual ~Action( );
            
            /**
             * @brief Code that will be run by the agent. Must be implemented in child class.
             * 
             * @param agent Agent that will perform the Action.
             */
            virtual void execute( Agent & agent ) = 0;

            /**
             * @brief Describes the type of Action executed. implemented in child class.
             * 
             * @return std::string 
             */
            virtual std::string describe( ) const;
    };
} // namespace Engine
#endif // __Action_hxx__

