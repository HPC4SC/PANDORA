
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
	// constructs a EatAction instance
	EatAction();
	// destroys a EatAction instance
	virtual ~EatAction();
	// code that de agent executes in a EatAction
	void execute( Engine::Agent & agent );
	// auxiliar method
	std::string describe() const;
	
};

} // namespace Examples

#endif // __EatAction_hxx__

