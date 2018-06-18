
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
	DieAction();
	virtual ~DieAction();
	void execute( Engine::Agent & agent );
	std::string describe() const;
};

} // namespace Examples

#endif 
