
#include <Bug.hxx>
#include <MoveAction.hxx>
#include <EatAction.hxx>
#include <GrowAction.hxx>
#include <KillAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

Bug::Bug( const std::string & id, const int &maxConsumptionRate, const int &size) : Agent(id),  _maxConsumptionRate(maxConsumptionRate), _size(size) 
{
	this->_exists = true;
	setColor(this->_size);
}

Bug::~Bug() {}

void Bug::selectActions() {
	_actions.push_back(new MoveAction());
	_actions.push_back(new EatAction());
	_actions.push_back(new GrowAction());
	_actions.push_back(new KillAction());
}

void Bug::updateState() {}

void Bug::registerAttributes() {
	registerStringAttribute("color");
	registerIntAttribute("size");
}

void Bug::serialize() {
	serializeAttribute("color", _color);
	serializeAttribute("size", _size);
}

void Bug::setSize(const int &size) {
	_size = size;
}

int Bug::getSize() const {
	return _size;
}

void Bug::setColor(const int &code) {
	if (code == 0) this->_color = "yellow";
	else {
		switch (code/10){
			
			case 0:
				this->_color = "pink";
				break;
			
			case 1:
				this->_color = "pink";
				break;
			
			case 2: 
				this->_color = "pink1";
				break;
			
			case 3: 
				this->_color = "pink2";
				break;
			
			case 4: 
				this->_color = "palevioletred1";
				break;
			
			case 5: 
				this->_color = "palevioletred2";
				break;
			
			case 6: 
				this->_color = "indianred2"; 
				break;
			
			case 7:
				this->_color = "indianred3";
				break;
			
			case 8: 
				this->_color = "red3";
				break;
			
			case 9: 
				this->_color = "indianred1";
				break;
			
			default: 
				this->_color = "red";
				break;
		}
	}
}

std::string Bug::getColor() const {
	return _color;
}

int Bug::getMaxConsumptionRate() const {
	return _maxConsumptionRate;
}

int Bug::getSurvivalProbability() const {
	return _survivalProbability;
}

void Bug::kill() {
	this->_exists = false;
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
		int modX = Engine::GeneralState::statistics().getUniformDistValue(-3,3); //velocidad
		newPosition._x += modX;
		int modY = Engine::GeneralState::statistics().getUniformDistValue(-3,3);
		newPosition._y += modY;
		if (_world->checkPosition(newPosition)) colocat = true;
	}
	if (not colocat) child->kill();
	else child->setPosition(newPosition);
}

} // namespace Examples

