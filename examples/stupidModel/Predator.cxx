#include <Predator.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <HuntAction.hxx>

namespace Examples
{

Predator::Predator( const std::string & id ) : Agent(id){}

Predator::~Predator() {}

void Predator::selectActions() {
	Engine::World * world = this->getWorld();
	int step = world->getCurrentStep();
	if (step%2 != 0) {
		//std::cout << "I'm " << this->getId() << "and now I execute my actions" << std::endl;
		_actions.push_back(new HuntAction());
	}
}

}

