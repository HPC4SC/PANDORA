
/*
 * Copyright (c) 2013
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACI-N
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

#ifndef __Analysis_hxx__
#define __Analysis_hxx__

#include <string>
#include <vector>

#include <SimulationRecord.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess 
{

class Analysis
{
protected:
	std::string _name; //! Name of the simulation.
	std::vector<long double> _results; //! Results of the simulation.
	bool _writeResults; //! If _writeResults is false _results will not be written in csv file. It is used for analysis that produce other results (i.e. AgentHDFtoSHP parser).
public:
	/**
	 * @brief Construct a new Analysis.
	 * 
	 * @param name Name of the simulation.
	 * @param writeResults If _writeResults is false _results will not be written in csv file. It is used for analysis that produce other results (i.e. AgentHDFtoSHP parser).
	 */
	Analysis( const std::string & name, bool writeResults = true );

	/**
	 * @brief Destroy the Analysis.
	 * 
	 */
	virtual ~Analysis();

	/**
	 * @brief Set the NumTimeSteps.
	 * 
	 * @param numTimeSteps Steps of the simulation.
	 */
	void setNumTimeSteps( int numTimeSteps );

	/**
	 * @brief Get the _name attribute.
	 * 
	 * @return const std::string& 
	 */
	const std::string & getName() const;

	/**
	 * @brief Processes needed to be executed before the computation of the agent.
	 * 
	 */
	virtual void preProcess();

	/**
	 * @brief Processes needed to be executed after the computation of the agent.
	 * 
	 */
	virtual void postProcess(){};

	/**
	 * @brief Get the resuts in the specified timeStep.
	 * 
	 * @param timeStep Time step we want to check for the result.
	 * @return long double 
	 */
	long double getResult( int timeStep ) const;

	/**
	 * @brief True if the results must be written in a output file, otherwise the resulsts must not be written.
	 * 
	 * @return true 
	 * @return false 
	 */
	bool writeResults(){return _writeResults;}	
};

class RasterAnalysis : public Analysis
{
public:
	/**
	 * @brief Construct a new RasterAnalysis instance.
	 * 
	 * @param name Name od the class.
	 * @param writeResults If true the results will be writen. Otherwise the results are not writen.
	 */
	RasterAnalysis( const std::string & name, bool writeResults = true ) : Analysis(name, writeResults)
	{
	}

	/**
	 * @brief Implemented in child class. Destroy the Raster Analysis instance.
	 * 
	 */
	virtual ~RasterAnalysis(){}

	/**
	 * @brief Must be implemented in chlid class. Compute the specified metric for the value raster.
	 * 
	 * @param rasterHistory Values of the raster in the simulation.
	 */
	virtual void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory ) = 0;
};

class AgentAnalysis : public Analysis
{
public:
	/**
	 * @brief Construct a new AgentAnalysis instance.
	 * 
	 * @param name Name of the class.
	 * @param writeResults If true the results will be writen. Otherwise the results are not writen.
	 */
	AgentAnalysis( const std::string & name, bool writeResults = true ) : Analysis(name, writeResults)
	{
	}

	/**
	 * @brief Destroy the AgentAnalysis instance.
	 * 
	 */
	virtual ~AgentAnalysis(){}

	/**
	 * @brief Must be implemented in chlid class. Compute the specified metric for the value agent.
	 * 
	 */
	virtual void computeAgent( const Engine::AgentRecord & ) = 0;
};

} // namespace PostProcess

#endif // __Analysis_hxx__
