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

#ifndef __CheckpointingFactory_hxx__
#define __CheckpointingFactory_hxx__

#include <World.hxx>

#include <string>
#include <map>

namespace Engine
{
    class Agent;

    class CheckpointingFactory
    {

        private:

            /**
             * @brief Construct a new CheckpointingFactory object.
             * 
             */
            CheckpointingFactory();

        public:

            /**
             * @brief Creates and then returns an instance of CheckpointingFactory if it does not exist yet. Otherwise, the _instance member is returned.
             * 
             * @return CheckpointingFactory&
             */
            static CheckpointingFactory& instance();

            /**
             * @brief Destroy the CheckpointingFactory object.
             * 
             */
            virtual ~CheckpointingFactory();

            /**
             * @brief Creates an agent from the 'encodedAgent' info and returns it.
             * 
             * @param encodedAgent const std::string&
             * @param type const std::string&
             * @return Agent* 
             */
            Agent* decodeAndFillAgent(const std::string& type, const std::string& encodedAgent);

            /**
             * @brief Sets the 'world' data whcn loading a checkpoint.
             * 
             * @param world World*
             * @param encodedWorldData const std::string&
             */
            void setWorldData(World* world, const std::string& encodedWorldData);

    };

} // namespace Engine

#endif // __CheckpointingFactory_hxx__