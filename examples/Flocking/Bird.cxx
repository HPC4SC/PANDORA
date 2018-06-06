
#include <Bird.hxx>
#include <MoveAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>

namespace Examples {

Bird::Bird (const std::string & id , const int & velocity, const int &sigth, const int &mindist, const float &max_A_turn, const float &max_C_turn, const float &max_S_turn) : Agent(id), _velocity(velocity), _sigth(sigth), _mindist(mindist), _max_A_turn(max_A_turn), _max_C_turn(max_C_turn), _max_S_turn(max_S_turn) {
	_heading = Engine::GeneralState::statistics().getUniformDistValue(0,360);
}

Bird::~Bird() {}

void Bird::selectActions() {
	_actions.push_back(new MoveAction());
}

void Bird::updateState() {
	if (_velocity < 1) _velocity = 1;
	else if (_sigth < 1) _sigth = 1;
	else if (_heading > 359.99) _heading = 0.0;
	else if (_heading < 0.0) _heading = 359.99;
}

void Bird::registerAttributes(){}

void Bird::serialize(){}

int Bird::getVelocity() const {
	return _velocity;
}

int Bird::getSigth() const {
	return _sigth;
}

int Bird::getMindist() {
	return _mindist;
}

void Bird::setHeading(float h) {
	_heading = h;	
}

float Bird::getHeading() {
	return _heading;
}

float Bird::getMaxATurn() const{
	 return _max_A_turn;
}

float Bird::getMaxCTurn() const{
	 return _max_C_turn;
}

float Bird::getMaxSTurn() {
	 return _max_S_turn;
}

} // namespace Examples

