
#ifndef __GrowAction_hxx__
#define __GrowAction_hxx__

#include <Action.hxx>
#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples
 {

class GrowAction : public Engine::Action {
	
public:
	GrowAction();
	virtual ~GrowAction();
	void execute( Engine::Agent & agent );
	std::string describe() const;
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

