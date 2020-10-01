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

#ifndef __RNGUniformInt__hxx__
#define __RNGUniformInt__hxx__

#include "RNG.hxx"

#include <boost/random.hpp>

namespace Engine
{

    class RNGUniformInt : public RNG<int>
    {
        
        private: 
        
            typedef boost::mt19937 RandomEngine;

        protected:

            int _a;
            int _b;

            RandomEngine _randomGenerator;

            boost::uniform_int<> _distributionType;
	        boost::variate_generator< RandomEngine, boost::uniform_int<> > _nextRandomNumber;

        public:

            /**
             * @brief Constructs a new RNGUniform object.
             * 
             * @param seed const uint64_t&
             * @param a const int&
             * @param b const int&
             */
            RNGUniformInt(const uint64_t& seed, const int& a, const int& b);

            /**
             * @brief Constructs a new RNGUniform object for doubles.
             * 
             * @param seed const uint64_t&
             * @param a const double&
             * @param b const double&
             */
            //RNGUniform(const uint64_t& seed, const double& a, const double& b);

            /**
             * @brief Destroy the RNGUniform object.
             * 
             */
            virtual ~RNGUniformInt();

            /**
             * @brief Gets the _a parameter of the uniform distribution.
             * 
             * @return int
             */
            int getA() const;

            /**
             * @brief Gets the _b parameter of the uniform distribution.
             * 
             * @return int
             */
            int getB() const;

            /**
             * @brief Get a uniform distribution value.
             * 
             * @return int
             */
            int draw();

    };

}

#endif // __RNGUniformInt__hxx__