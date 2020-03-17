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

#ifndef __IndividualStats_hxx__
#define __IndividualStats_hxx__

#include <analysis/Output.hxx>
#include <list>
#include <fstream>
#include <iomanip>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess
{

class IndividualStats : public Output 
{

private:

	int _numStep; //! Number of step to check.
	std::ofstream _file; //! Output file.

	std::list<std::string> _attributes; //! Attributes of the agent.

	/**
	 * @brief Checks whether attribute key is already stored in _attributes or not.
	 * 
	 * @param key the name of the attribute to be checked
	 * @return bool
	 */
	bool attributeToStore( const std::string & key );

public:

	/**
	 * @brief Set a list of attributes and store for numStep time step, including agent id.
	 * 
	 * @param numStep Number of step to check.
	 * @param separator Separator between values on the output file.
	 */
	IndividualStats( int numStep, const std::string & separator=";");

	/**
	 * @brief Destroy the IndividualStats instance.
	 * 
	 */
	virtual ~IndividualStats();

	/**
	 * @brief Prints the individual agents in the output file.
	 * 
	 * @param agentRecord Agent record instance.
	 */
	void computeAgent( const Engine::AgentRecord & agentRecord );

	/**
	 * @brief Add one attribut eto the _attributes list.
	 * 
	 * @param attribute Attribute to add.
	 */
	void addAttribute( const std::string & attribute );

	/**
	 * @brief Processes needed to be executed after the computation of the agent.
	 * 
	 * @param simRecord Simulation record instance.
	 * @param outputFile Route to the output file.
	 */
	void preProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );

	/**
	 * @brief Processes needed to be executed after the computation of the agent.
	 * 
	 * @param simRecord Simulation record instance.
	 * @param outputFile Route to the output file.
	 */
	void postProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );

	/**
	 * @brief Gets the name of the class.
	 * 
	 * @return std::string 
	 */
	std::string getName() const;

};

} // namespace PostProcess

#endif // __IndividualStats_hxx__

