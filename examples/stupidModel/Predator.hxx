
#ifndef __Predator_hxx__
#define __Predator_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples
{

class Predator : public Engine::Agent
{

public:
	// creates a Predator agent
	Predator( const std::string & id );
	// destroys a Predator agent
	virtual ~Predator();
	
	// selects wich actions and in wich order each Predator agent
	void selectActions();
	
	void updateState( );
	void updateKnowledge( );
	
	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Predator( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __RandomAgent_hxx__
