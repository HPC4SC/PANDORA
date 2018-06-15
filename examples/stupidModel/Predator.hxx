
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
	// todo remove environment from here
	Predator( const std::string & id );
	virtual ~Predator();
	
	void selectActions();
	void updateState();
	void registerAttributes();
	void serialize();

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
