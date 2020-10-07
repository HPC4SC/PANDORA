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

#ifndef __RNGNormal__hxx__
#define __RNGNormal__hxx__

#include "RNG.hxx"

#include <boost/random.hpp>

namespace Engine
{

    class RNGNormal : public RNG<double>
    {
        
        private: 
        
            typedef boost::mt19937 RandomEngine;

        protected:

            double _mean;
            double _stdDev;

            RandomEngine _randomGenerator;
            boost::normal_distribution<> _distributionType;
	        boost::variate_generator< RandomEngine, boost::normal_distribution<> > _nextRandomNumber;

        public:

            /**
             * @brief Default constructor for RNGNormal (_seed = 0; _mean = 0, _stdDev = 1).
             * 
             */
            RNGNormal();

            /**
             * @brief Construct a new RNGNormal object.
             * 
             * @param seed const uint64_t&
             * @param mean const double&
             * @param stdDev const double&
             */
            RNGNormal(const uint64_t& seed, const double& mean, const double& stdDev);

            /**
             * @brief Destroy the RNGNormal object.
             * 
             */
            virtual ~RNGNormal();

            /**
             * @brief Get the mean of the Normal distribution instance.
             * 
             * @return float
             */
            double getMean() const;

            /**
             * @brief Get the mean of the Normal distribution instance.
             * 
             * @return float 
             */
            double getStdDev() const;

            /**
             * @brief Get a normal distribution value.
             * 
             * @return float 
             */
            double draw();

    };

}

#endif // __RNGNormal__hxx__