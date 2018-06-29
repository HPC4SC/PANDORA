
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
	ReplacementAction();
	virtual ~ReplacementAction();
	void execute( Engine::Agent & agent );
	std::string describe() const;
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

