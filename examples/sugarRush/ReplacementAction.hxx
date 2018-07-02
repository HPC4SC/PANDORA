
#ifndef __ReplacementAction_hxx__
#define __ReplacementAction_hxx__

#include <Action.hxx>
#include <string>

namespace Engine
{
	class Agent;
}

namespace Examples
{

class ReplacementAction : public Engine::Action
{
public:
	// creates a ReplacementAgent instance
	ReplacementAction();
	// destroys a ReplacementAgent instance
	virtual ~ReplacementAction();
	// code that each agent executes when performing a ReplacementAction
	void execute( Engine::Agent & agent );
	// auxiliar function
	std::string describe() const;
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

