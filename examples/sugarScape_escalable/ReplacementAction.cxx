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
	// checks if the agent is going to be removed
	if ((randomAgent.getCurrentAge() > randomAgent.getMaxAge()) or (randomAgent.getWealth() < 0)) {
		Engine::World * world = agent.getWorld();
		std::string childId = agent.getId() + "_child";
		
		// removes the agent form the simulation 
		agent.remove();

		// and creates anotherone with random attributes and position
		const RandomWorldConfig& randomConfig = (const RandomWorldConfig&) world->getConfig();
		int childWealth =  Engine::GeneralState::statistics().getUniformDistValue(randomConfig._minWealth,randomConfig._maxWealth);
		int childVision =  Engine::GeneralState::statistics().getUniformDistValue(randomConfig._minVision,randomConfig._maxVision);
		int childMetabolicRate =  Engine::GeneralState::statistics().getUniformDistValue(randomConfig._minMr,randomConfig._maxMr);
		int childMaxAge =  Engine::GeneralState::statistics().getUniformDistValue(randomConfig._minMAge,randomConfig._maxMAge);

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
