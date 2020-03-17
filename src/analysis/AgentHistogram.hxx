/*
 * Copyright (c) 2013
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

#ifndef __AgentHistogram_hxx__
#define __AgentHistogram_hxx__

#include <analysis/Output.hxx>
#include <vector>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess 
{

class AgentHistogram : public Output
{
private:
	std::string _attribute; //! Attribute to make the histogram.
	int _interval; //! Interval of steps that the histogram will take into account. 
	int _numStep; //! If a num step is passed the histogram will be generated from it. If not, the final result of the simulation will be stored.

	std::vector<int> _histogram; //! Values of the histogram.
public:
	/**
	 * @brief Construct a new AgentHistogram.
	 * 
	 * @param attribute Attribute to make the histogram.
	 * @param interval Interval of steps that the histogram will take into account. 
	 * @param numStep If a num step is passed the histogram will be generated from it. If not, the final result of the simulation will be stored.
	 * @param separator Separator of values in the output files.
	 */
	AgentHistogram( const std::string & attribute, int interval = 1, int numStep = -1, const std::string & separator = ";");

	/**
	 * @brief Destroy the AgentHistogram.
	 * 
	 */
	virtual ~AgentHistogram();

	/**
	 * @brief Processes needed to be executed before the computation of the agent.
	 * 
	 * @param simRecord Simulation record instance.
	 */
	void preProcess( const Engine::SimulationRecord & simRecord, const std::string & );

	/**
	 * @brief Generates the histogram of the given attribute of the agent.
	 * 
	 * @param agentRecord Agent record instance.
	 */
	void computeAgent( const Engine::AgentRecord & agentRecord );

	/**
	 * @brief Processes needed to be executed after the computation of the agent.
	 * 
	 * @param simRecord Simulation record instance.
	 */
	void postProcess( const Engine::SimulationRecord & simRecord, const std::string & );

	/**
	 * @brief Name of the instance.
	 * 
	 * @return std::string 
	 */
	std::string getName() const;
};

} // namespace PostProcess

#endif // __AgentHistogram_hxx__

