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

#ifndef __TestWorld_hxx__
#define __TestWorld_hxx__

#include <World.hxx>

namespace Engine
{
	class Config;
    class SpacePartition;
}

namespace Test
{

class TestWorld : public Engine::World
{
    Engine::SpacePartition * _testScheduler;

	void createAgents();
	void stepEnvironment();
public:
	TestWorld( Engine::Config * config, Engine::Scheduler * scheduler = 0);
	virtual ~TestWorld();
};

} // namespace Test 


#endif // __TestWorld_hxx__

