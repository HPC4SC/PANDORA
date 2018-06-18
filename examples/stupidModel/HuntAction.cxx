
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
	//mirar
	Engine::Agent * p_agent = agent.getWorld()-> getAgent(agent.getId());
	Engine::AgentsVector neighbours = agent.getWorld()->getNeighbours(p_agent,1,"all");
	Engine::Point2D<int> newPosition;
	if (not neighbours.empty()) {
		random_shuffle(neighbours.begin(),neighbours.end());
		Engine::AgentsVector::iterator neighbour = neighbours.begin();
		bool menjat = false;
		while (neighbour != neighbours.end() and not menjat) {
			Engine::Agent* candidate = (neighbour->get());
			if (Bug * victim = dynamic_cast<Bug*>(candidate)) {
				newPosition = candidate->getPosition();
				victim->kill();
				menjat = true;
			}
			neighbour++;
		}
		if (menjat) {
			agent.setPosition(newPosition);
		}
	}
	
	else {
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
