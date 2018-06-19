
#ifndef __EatAction_hxx__
#define __EatAction_hxx__

#include <Action.hxx>
#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples 
{

class EatAction : public Engine::Action {
	
public:
	// construeix una instancia d'EatAction
	EatAction();
	// destrueix una instancia d'EatAction
	virtual ~EatAction();
	// accions que ha de seguir l'agent quan executa una EatAction
	void execute( Engine::Agent & agent );
	// funcio auxiliar no se pq serveix, Scheduler maybe?
	std::string describe() const;
	
};

} // namespace Examples

#endif // __EatAction_hxx__

