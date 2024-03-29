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

#include <Point2D.hxx>

namespace Test
{

class TestAgent: public Engine::Agent
{
	std::vector<int> _vectorInt; // MpiVectorAttribute
	std::vector<float> _vectorFloat; // MpiVectorAttribute
	bool _moveToDownLeft; // MpiBasicAttribute

public:
	TestAgent( const std::string & id, bool moveToDownLeft );
	virtual ~TestAgent();
	
	void updateState();

	std::vector<int> & getVectorInt();
	std::vector<float> & getVectorFloat();


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

