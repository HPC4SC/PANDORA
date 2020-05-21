#include <ReplacementAction.hxx>

#include <RandomWorldConfig.hxx>	
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
	// checks if the agent is going to be removed
	if ((randomAgent.getCurrentAge() > randomAgent.getMaxAge()) or (randomAgent.getWealth() < 0)) {
		Engine::World * world = agent.getWorld();
		std::string childId = agent.getId() + "_child";
		
		// removes the agent form the simulation 
		agent.remove();

		// and creates anotherone with random attributes and position
		const RandomWorldConfig& randomConfig = (const RandomWorldConfig&) world->getConfig();
		int childWealth =  Engine::GeneralState::statistics().getUniformDistValue(randomConfig.getMinWealth(),randomConfig.getMaxWealth());
		int childVision =  Engine::GeneralState::statistics().getUniformDistValue(randomConfig.getMinVision(),randomConfig.getMaxVision());
		int childMetabolicRate =  Engine::GeneralState::statistics().getUniformDistValue(randomConfig.getMinMr(),randomConfig.getMaxMr());
		int childMaxAge =  Engine::GeneralState::statistics().getUniformDistValue(randomConfig.getMinMAge(),randomConfig.getMaxMAge());

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
