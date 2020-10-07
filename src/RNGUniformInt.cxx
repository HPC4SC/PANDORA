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

#include <RNGUniformInt.hxx>

namespace Engine
{

    RNGUniformInt::RNGUniformInt() : RNG(), _a(0), _b(1), _randomGenerator(_seed), _distributionType(_a, _b), _nextRandomNumber(_randomGenerator, _distributionType)
    {
    }

    RNGUniformInt::RNGUniformInt(const uint64_t& seed, const int& a, const int& b) : RNG(seed), _a(a), _b(b), _randomGenerator(_seed), _distributionType(_a, _b), _nextRandomNumber(_randomGenerator, _distributionType)
    {
    }

    RNGUniformInt::~RNGUniformInt()
    {
    }

    int RNGUniformInt::getA() const
    {
        return _a;
    }

    int RNGUniformInt::getB() const
    {
        return _b;
    }

    int RNGUniformInt::draw()
    {
        return _nextRandomNumber();
    }

}