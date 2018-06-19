
#ifndef _DieAction_hxx_
#define _DieAction_hxx_

#include <Action.hxx>
#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples 
{

class DieAction : public Engine::Action {
	
public:
	// construeix una instancia de DieAction
	DieAction();
	// destrueix una instancia de DieAction
	virtual ~DieAction();
	// accions que ha de seguir l'agent quan executa una DieAction
	void execute( Engine::Agent & agent );
	// funcio auxiliar no se pq serveix, Scheduler maybe?
	std::string describe() const;
};

} // namespace Examples

#endif 
