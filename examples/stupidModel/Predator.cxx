#include <Predator.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <HuntAction.hxx>

namespace Examples
{

Predator::Predator( const std::string & id, const int& maxPredatorHuntDistance) : Agent(id), _maxPredatorHuntDistance(maxPredatorHuntDistance)
{}

Predator::~Predator() {}

void Predator::selectActions() {
	Engine::World * world = this->getWorld();
	int step = world->getCurrentStep();
	/* the predators must be executed after the bugs.
	 * therefore they are executed in odd timesteps
	 */
	if (step%2 != 0) {
		_actions.push_back(new HuntAction());
	}
}

int Predator::getPredatorHuntDistance() const
{
	return _maxPredatorHuntDistance;
}

void Predator::updateState( ) {}
void Predator::updateKnowledge( ) {}

}

