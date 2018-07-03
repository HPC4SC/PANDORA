#include <ReplacementAction.hxx>

#include <World.hxx>
#include <RandomAgent.hxx>
#include <GeneralState.hxx>
#include <string>

namespace Examples
{

ReplacementAction::ReplacementAction()
{
}

ReplacementAction::~ReplacementAction()
{
}

void ReplacementAction::execute( Engine::Agent & agent )
{	
	RandomAgent & randomAgent = (RandomAgent&)agent;
	if ((randomAgent.getCurrentAge() > randomAgent.getMaxAge()) or (randomAgent.getWealth() < 0)) {
		Engine::World * world = agent.getWorld();
		std::string childId = agent.getId() + "_child";
		agent.remove();
		int childWealth =  Engine::GeneralState::statistics().getUniformDistValue(5,25);
		int childVision =  Engine::GeneralState::statistics().getUniformDistValue(1,6);
		int childMetabolicRate =  Engine::GeneralState::statistics().getUniformDistValue(1,4);
		int childMaxAge =  Engine::GeneralState::statistics().getUniformDistValue(60,100);
		RandomAgent * child = new RandomAgent(childId,childWealth,childVision,childMetabolicRate,0,childMaxAge);
		world->addAgent(child);
		child->setRandomPosition();
	}
}

std::string ReplacementAction::describe() const
{
	return "ReplacementAction";
}

} // namespace Examples
