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
	HuntAction();
	virtual ~HuntAction();
	void execute( Engine::Agent & agent );
	std::string describe() const;
};

} // namespace Examples

#endif 
