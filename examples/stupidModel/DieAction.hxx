
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
	// constructs a DieAction instance
	DieAction();
	// desroys a DieAction instance
	virtual ~DieAction();
	// code that de agent executes in a DieAction
	void execute( Engine::Agent & agent );
	// auxiliar method
	std::string describe() const;
};

} // namespace Examples

#endif 
