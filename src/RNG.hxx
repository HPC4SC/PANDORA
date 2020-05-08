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

#ifndef __RNG__hxx__
#define __RNG__hxx__

namespace Engine
{

    template <typename T>
    class RNG 
    {

        protected:

            typedef unsigned long long uint64_t;
            
            uint64_t _seed;

        public:
        
            /**
             * @brief Construct a new RNG object.
             * 
             */
            RNG(uint64_t seed) : _seed(seed) {}

            /**
             * @brief Destroy the RNG object.
             * 
             */
            virtual ~RNG() {}

            /**
             * @brief Get the _seed member of the instance.
             * 
             * @return uint64_t 
             */
            uint64_t getSeed() { return _seed; }

            /**
             * @brief Get a value of the implemented probability distribution.
             * 
             * @return T 
             */
            virtual T draw() = 0;

    };

}

#endif // __RNG__hxx__