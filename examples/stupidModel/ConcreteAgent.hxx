
#ifndef __ConcreteAgent_hxx__
#define __ConcreteAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples
{

class ConcreteAgent : public Engine::Agent
{

public:
	// crea un agent del tipus Predator
	ConcreteAgent( const std::string & id );
	// destrueix un agent del tipus Predator
	virtual ~ConcreteAgent();
	
	// selecciona quines accions i en quin ordre executa cada Agent de tipus Predator
	void selectActions();
	
	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	ConcreteAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __RandomAgent_hxx__
