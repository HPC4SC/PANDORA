
#include <GrowAction.hxx>

#include <World.hxx>
#include <Bug.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

GrowAction::GrowAction() {}

GrowAction::~GrowAction() {}

void GrowAction::execute( Engine::Agent & agent ) {	
	if (agent.exists()) {
		Bug & bugAgent = (Bug &)agent;
		bugAgent.setColor(bugAgent.getSize());
		std::cout << "I'm " << agent.getId() << " and my size is: " << bugAgent.getSize() << std::endl;
	}
}

std::string GrowAction::describe() const {
	return "GrowAction";
}

} // namespace Examples
