
#ifndef __Predator_hxx__
#define __Predator_hxx__

#include <ConcreteAgent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples
{

class Predator : public Examples::ConcreteAgent
{

public:
	// crea un agent del tipus Predator
	Predator( const std::string & id );
	// destrueix un agent del tipus Predator
	virtual ~Predator();
	
	// selecciona quines accions i en quin ordre executa cada Agent de tipus Predator
	void selectActions();
	

};

} // namespace Examples

#endif // __RandomAgent_hxx__
