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

#ifndef __Output_hxx__
#define __Output_hxx__

#include <list>
#include <string>

namespace Engine
{
	class SimulationRecord;
	class AgentRecord;
}

namespace PostProcess
{

class Output
{
protected:
	std::string _separator; //! Separator between values on the output file.

	/**
	 * @brief Implemented in child class. Processes needed to be executed before the computation of the agent.
	 * 
	 * @param simRecord Simulation record instance.
	 * @param outputFile Route to the output file.
	 */
	virtual void preProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );

	/**
	 * @brief Implemented in child class. Processes needed to be executed after the computation of the agent.
	 * 
	 * @param simRecord Simulation record instance.
	 * @param outputFile Route to the output file.
	 */
	virtual void postProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );

	/**
	 * @brief Implemented in child class. Calculates the specified metric of the given attribute of the agent.
	 * 
	 * @param agentRecord Agent record instance.
	 */
	virtual void computeAgent( const Engine::AgentRecord & agentRecord );

public:
	/**
	 * @brief Construct a new Output instance.
	 * 
	 * @param separator Separator between values on the output file.
	 */
	Output( const std::string & separator );

	/**
	 * @brief Implemented in child class. Destroy the Output instance.
	 * 
	 */
	virtual ~Output();

	/**
	 * @brief Implemented in child. Calculates the specified metrics for the analysis of the simulation.
	 * 
	 * @param simRecord Simulation record instance.
	 * @param outputFile Route to the ooutput file.
	 * @param type Type of the agents.
	 */
	virtual void apply( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type );

	/**
	 * @brief Must be implemented in child. Gets the name of the class.
	 * 
	 * @return std::string 
	 */
	virtual std::string getName() const = 0;
};

} // namespace PostProcess

#endif // __Output_hxx__

