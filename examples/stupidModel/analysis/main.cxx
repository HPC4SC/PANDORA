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

#include <Exception.hxx>
#include <SimulationRecord.hxx>

#include <analysis/GlobalAgentStats.hxx>
#include <analysis/AgentMean.hxx>
#include <analysis/AgentNum.hxx>
#include <analysis/AgentHistogram.hxx>
#include <iostream>

int main(int argc, char*argv[]) {
	try {
		Engine::SimulationRecord simRecord(1,true);
		simRecord.loadHDF5("../data/stupidModel.h5",false,true); // (ruta,raster,agent)
		
		PostProcess::GlobalAgentStats agentResults;
		agentResults.addAnalysis(new PostProcess::AgentNum());
		agentResults.addAnalysis(new PostProcess::AgentMean("size"));
		agentResults.apply(simRecord,"agents.csv","Bug");
		
		PostProcess::AgentHistogram population("population", 200);
		
		population.apply(simRecord,"NumAgents.csv", "Bug");
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
	
}


