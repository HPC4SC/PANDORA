
#include <GrowAction.hxx>

#include <World.hxx>
#include <Bug.hxx>
#include <GeneralState.hxx>

namespace Examples
{

GrowAction::GrowAction()
{
}

GrowAction::~GrowAction()
{
}

void GrowAction::execute( Engine::Agent & agent )
{	
	Bug & bugAgent = (Bug &)agent;
	bugAgent.setColor(bugAgent.getSize());
}

std::string GrowAction::describe() const
{
	return "GrowAction";
}

} // namespace Examples