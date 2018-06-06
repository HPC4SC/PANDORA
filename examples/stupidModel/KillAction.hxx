
#ifndef _KillAction_hxx_
#define _KillAction_hxx_

#include <Action.hxx>
#include <string>

namespace Engine {
	class Agent;
}

namespace Examples {

class KillAction : public Engine::Action {
	
public:
	KillAction();
	virtual ~KillAction();
	void execute( Engine::Agent & agent );
	std::string describe() const;
};

} // namespace Examples

#endif 
