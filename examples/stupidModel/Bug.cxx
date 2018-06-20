
#include <Bug.hxx>
#include <MoveAction.hxx>
#include <EatAction.hxx>
#include <DieAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

Bug::Bug( const std::string & id, const int &maxConsumptionRate, const int &size) : Agent(id),  _maxConsumptionRate(maxConsumptionRate), _size(size) 
{
	this->_exists = true;
}

Bug::~Bug() {}

void Bug::selectActions() {
	Engine::World * world = this->getWorld();
	int step = world->getCurrentStep();
	if (step%2 == 0) {
		//std::cout << "I'm " << this->getId() << "and now I execute my actions" << std::endl;
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

int Bug::getMaxConsumptionRate() const {
	return _maxConsumptionRate;
}

int Bug::getSurvivalProbability() const {
	return _survivalProbability;
}

void Bug::reproduce(const std::string &childId) {
	std::ostringstream oss;
	oss <<  childId;
	Bug * child = new Bug(oss.str(),this->_maxConsumptionRate,0);
	_world->addAgent(child);
	bool colocat = false;
	Engine::Point2D<int>newPosition = this->getPosition();
	for (int i = 0; i < 5 and not colocat; ++i) {
		//trobar una pos no ocupada
		int modX = Engine::GeneralState::statistics().getUniformDistValue(-3,3);
		newPosition._x += modX;
		int modY = Engine::GeneralState::statistics().getUniformDistValue(-3,3);
		newPosition._y += modY;
		if (_world->checkPosition(newPosition)) colocat = true;
	}
	if (not colocat) child->remove();
	else child->setPosition(newPosition);
}

} // namespace Examples

