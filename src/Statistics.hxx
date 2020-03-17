
/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
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

#ifndef __Statistics_hxx__
#define __Statistics_hxx__

#include <boost/random.hpp>
#include <vector>
#include <map>
#include <tuple>

namespace Engine
{

class Statistics
{
private:
	typedef boost::mt19937 RandomEngine;
	typedef boost::variate_generator<boost::mt19937&,boost::gamma_distribution<> >  TypeGenerator;

	RandomEngine _randomGenerator; //! Random number generator.
	static const int _distributionSize = 100000;

	
	boost::uniform_int<> _randomNumbers; //! General random indexs.
	mutable boost::variate_generator< RandomEngine, boost::uniform_int<> > _nextRandomNumber;

    boost::uniform_01<RandomEngine> _next01Number;

	// TODO fix expo and normal distributions!
	std::vector<float> _exponentialDistribution; //! Exponential distribution.
	
	/**
	 * @brief Geretaes a exponential distribution.
	 * 
	 */
	void generateExponentialDistribution();

	std::vector<float> _normalDistribution; //! Normal distribution.
	
	/**
	 * @brief Geretaes a normal distribution.
	 * 
	 */
	void generateNormalDistribution();

	std::map<std::string, std::tuple<TypeGenerator,TypeGenerator,double> > _mapBetaDistributions;

public:
	/**
	 * @brief Construct a new Statistics instance.
	 * 
	 */
	Statistics();

	/**
	 * @brief Get an exponential distribution value.
	 * 
	 * @param min Minimum value.
	 * @param max Maximum value.
	 * @return float 
	 */
	float getExponentialDistValue( float min, float max ) const;

	/**
	 * @brief Get a normal distribution value.
	 * 
	 * @param min Minimum value.
	 * @param max Maximum value.
	 * @return float 
	 */
	float getNormalDistValueMinMax( float min, float max ) const;

	/**
	 * @brief Get a normal distribution value.
	 * 
	 * @param mean Mean value.
	 * @param sd Standerd deviation.
	 * @return float 
	 */
	float getNormalDistValue( float mean, float sd );

	/**
	 * @brief Return a random number following a power law distribution 
	 * 
	 * @param min Minimum value.
	 * @param max Maximum value.
	 * @param alpha Alpha value.
	 * @return float 
	 */
	float getPowerLawDistValue( float min, float max,float alpha );

	/**
	 * @brief Gets a uniform distribution value.
	 * 
	 * @param min Minimum value.
	 * @param max Maximum value.
	 * @return int 
	 */
	int getUniformDistValue( int min, int max ) const;
	
	/**
	 * @brief Uniform float distribution between 0 and 1.
	 * 
	 * @return float 
	 */
	float getUniformDistValue();
	
	/**
	 * @brief Gets a random number from /dev/urandom to be used as a seed.
	 * 
	 * @return uint64_t 
	 */
	uint64_t getNewSeed();

	/**
	 * @brief Adds a new beta distribution.
	 * 
	 * @param name Name of the distribution.
	 * @param alpha Alpha of the distribution.
	 * @param beta Beta of the distribtion.
	 * @param scale Scale of the distribution.
	 */
	void addBetaDistribution(std::string name, double alpha, double beta, double scale);

	/**
	 * @brief Get a beta distribution value.
	 * 
	 * @param name Name of the beta distribution.
	 * @return double 
	 */
	double getBetaDistributionValue(std::string name);

};

} // namespace Engine

#endif // __Statistics_hxx__

