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

#include <RNGNormal.hxx>

namespace Engine
{

    RNGNormal::RNGNormal(uint64_t seed, double mean, double stdDev) : RNG(seed), _mean(mean), _stdDev(stdDev), _randomGenerator(_seed), _dist(_mean, _stdDev), _nextRandomNumber(_randomGenerator, _dist)
    {  
    }

    RNGNormal::~RNGNormal() 
    {
    }
    
    double RNGNormal::getMean()
    {
        return _mean;
    }

    double RNGNormal::getStdDev()
    {
        return _stdDev;
    }

    double RNGNormal::draw() {
        return _nextRandomNumber();
    }
}