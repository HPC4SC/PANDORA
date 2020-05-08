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

#include <TestAgent.hxx>
#include <World.hxx>
#include <Point2D.hxx>

namespace Test
{

TestAgent::TestAgent( const std::string & id ) : Agent(id)
{
}

TestAgent::~TestAgent()
{
}

void TestAgent::move()
{
	Engine::Point2D<int> newPosition = _position;
	newPosition._x--;
	newPosition._y--;

	// the gent will die just in the overlap between the 4 nodes
	if(newPosition._y==32)
	{
		remove();
		return;
	}
	
	if(_world->checkPosition(newPosition))
	{
		setPosition(newPosition);
	}
}

void TestAgent::updateState()
{
	move();
}

} // namespace Test

