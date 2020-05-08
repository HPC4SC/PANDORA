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

#ifndef __TestAgent_hxx__
#define __TestAgent_hxx__

#include <Agent.hxx>
#include <string>

namespace Test
{

class TestAgent: public Engine::Agent
{	
	void move();

	bool _horizontalMovement; // MpiBasicAttribute
	bool _evenTurn; // MpiBasicAttribute

	void updateTurnInformation();
	void setEvenTurn( const bool & evenTurn );
public:
	TestAgent( const std::string & id, const bool & horizontalMovement );
	virtual ~TestAgent();
	
	bool isEvenTurn() const;
	void updateState();


	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	TestAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Test

#endif // __TestAgent_hxx__

