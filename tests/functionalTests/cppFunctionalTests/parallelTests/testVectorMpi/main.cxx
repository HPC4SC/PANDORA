/*
 * Copyright (c) 2012
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

#include "TestWorld.hxx"
#include <Exception.hxx>
#include <Config.hxx>

#include <iostream>
#include <cstdlib>

// this test checks if vector data is correctly copied
// an agent is created with 2 vectors (float and int) of 200 positions
// data for each index is equal to the index + time step
// each time step the agnet modifies its vector
// afterwards World checks whether it is ok
int main(int argc, char *argv[])
{
	try
	{
		Test::TestWorld world(new Engine::Config(Engine::Size<int>(64,64), 5), world.useSpacePartition(1));
		world.initialize(argc, argv);
		world.run();
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
		return -1;
	}
	return 0;
}

