#ifndef __HuntAction_hxx__
#define __HuntAction_hxx__

#include <Action.hxx>
#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples
 {

class HuntAction : public Engine::Action {
	
public:
	// constructs a HuntAction instance
	HuntAction();
	// destroys a HuntAction instance
	virtual ~HuntAction();
	// code that de agent executes in a HuntAction
	void execute( Engine::Agent & agent );
	// auxiliar function
	std::string describe() const;
};

} // namespace Examples

#endif 
