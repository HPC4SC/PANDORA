
#include <Bug.hxx>
#include <MoveAction.hxx>
#include <EatAction.hxx>
#include <DieAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

Bug::Bug( const std::string & id, const int& maxMovement, const int &maxConsumptionRate, const int& survivalProbability, const int &size) : Agent(id), _maxBugMovement(maxMovement), _maxConsumptionRate(maxConsumptionRate), _survivalProbability(survivalProbability), _size(size)
{
	this->_exists = true;
}

Bug::~Bug() {}

void Bug::selectActions() {
	/* 
	 * the actions are pushed into the list of actions 
	 * that the agent must perform in the codeig order
 	 */
	Engine::World * world = this->getWorld();
	int step = world->getCurrentStep();
	/* the bugs must be executed before the predateors.
	 * therefore they are executed in even timesteps
	 */
	if (step%2 == 0) { 
		_actions.push_back(new MoveAction());
		_actions.push_back(new EatAction());
		_actions.push_back(new DieAction());
	}
}

void Bug::registerAttributes() {
	registerIntAttribute("size");
}

void Bug::serialize() {
	serializeAttribute("size", _size);
}

void Bug::setSize(const int &size) {
	_size = size;
}

int Bug::getSize() const {
	return _size;
}

int Bug::getMaxMovement() const {
	return _maxBugMovement;
}

int Bug::getMaxConsumptionRate() const {
	return _maxConsumptionRate;
}

int Bug::getSurvivalProbability() const {
	return _survivalProbability;
}

void Bug::reproduce(const std::string &childId) {
	Bug * child = new Bug(childId, this->_maxBugMovement, this->_maxConsumptionRate, this->_survivalProbability, 0);
	// creation of a new agent
	_world->addAgent(child);
	bool colocat = false;
	Engine::Point2D<int>newPosition = this->getPosition();
	for (int i = 0; i < 5 and not colocat; ++i) {
		// find an unoccupied cell within 5 trys
		int modX = Engine::GeneralState::statistics().getUniformDistValue(-3,3);
		newPosition._x += modX;
		int modY = Engine::GeneralState::statistics().getUniformDistValue(-3,3);
		newPosition._y += modY;
		if (_world->checkPosition(newPosition)) colocat = true;
	}
	// if the agent has foud a valid position it is placed there
	if (not colocat) child->remove();
	// otherwise the agent is removed
	else child->setPosition(newPosition);
}


	void Bug::updateState( ) {}
	void Bug::updateKnowledge( ) {}

} // namespace Examples

