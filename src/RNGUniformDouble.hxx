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

#ifndef __RNGUniformDouble__hxx__
#define __RNGUniformDouble__hxx__

#include "RNG.hxx"

#include <boost/random.hpp>

namespace Engine
{

    class RNGUniformDouble : public RNG<double>
    {
        
        private: 
        
            typedef boost::mt19937 RandomEngine;

        protected:

            double _a;
            double _b;

            RandomEngine _randomGenerator;

            boost::uniform_real<> _distributionType;
	        boost::variate_generator< RandomEngine, boost::uniform_real<> > _nextRandomNumber;

        public:

            /**
             * @brief Default constructor for RNGUniformDouble (_seed = 0; _a = 0, _b = 1).
             * 
             */
            RNGUniformDouble();

            /**
             * @brief Constructs a new RNGUniform object.
             * 
             * @param seed const uint64_t&
             * @param a const double&
             * @param b const double&
             */
            RNGUniformDouble(const uint64_t& seed, const double& a, const double& b);

            /**
             * @brief Destroy the RNGUniform object.
             * 
             */
            virtual ~RNGUniformDouble();

            /**
             * @brief Gets the _a parameter of the uniform distribution.
             * 
             * @return int
             */
            double getA() const;

            /**
             * @brief Gets the _b parameter of the uniform distribution.
             * 
             * @return int
             */
            double getB() const;

            /**
             * @brief Get a uniform distribution value.
             * 
             * @return int
             */
            double draw();

    };

}

#endif // __RNGUniformDouble__hxx__