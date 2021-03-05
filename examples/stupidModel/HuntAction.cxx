
#include <HuntAction.hxx>

#include <World.hxx>
#include <Predator.hxx>
#include <Bug.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

HuntAction::HuntAction() {}

HuntAction::~HuntAction() {}

void HuntAction::execute( Engine::Agent & agent ) {	
	Engine::Agent * p_agent = agent.getWorld()-> getAgent(agent.getId());

	Predator& predator = (Predator&) agent;
	Engine::AgentsVector neighbours = agent.getWorld()->getNeighbours(p_agent, predator.getPredatorHuntDistance());
	Engine::Point2D<int> newPosition;
	if (neighbours.size() > 0) { 
		random_shuffle(neighbours.begin(),neighbours.end());
		Engine::AgentsVector::iterator neighbour = neighbours.begin();
		bool menjat = false;
		while (neighbour != neighbours.end() and not menjat) { // the predator looks around for bugs
			Engine::Agent* candidate = (neighbour->get());
			if (Bug * victim = dynamic_cast<Bug*>(candidate)) { // if he find a bug it eats it
				newPosition = candidate->getPosition();
				victim->remove();
				menjat = true;
			}
			neighbour++;
		}
		if (menjat) { // then the predator that has eaten moves to the bug's position
			agent.setPosition(newPosition);
		}
		// if the predator has only predators around it it doesn't move
	}
	
	else { // if the predator is isolated it moves randomly
		newPosition = agent.getPosition();
		Engine::World * world = agent.getWorld();
		int incX = Engine::GeneralState::statistics().getUniformDistValue(-1,1); 
		int incY = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
		newPosition._x += incX;
		newPosition._y += incY;
		if(world->checkPosition(newPosition)) {
			agent.setPosition(newPosition);
		}
	}
}

std::string HuntAction::describe() const {
	return "HuntAction";
}

} // namespace Examples
