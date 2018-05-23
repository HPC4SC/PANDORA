
#include <Bird.hxx>
#include <MoveAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>

namespace Examples
{

Bird::Bird (const std::string & id , const int & velocity, const int &sigth, const int &mindist, const float &max_A_turn, const float &max_C_turn, const float &max_S_turn) : Agent(id), _velocity(velocity), _sigth(sigth), _mindist(mindist), _max_A_turn(max_A_turn), _max_C_turn(max_C_turn), _max_S_turn(max_S_turn)
{
}

Bird::~Bird()
{
}

void Bird::selectActions()
{
	_actions.push_back(new MoveAction());
}

void Bird::updateState()
{
	if (_velocity < 1) _velocity = 1;
	else if (_sigth < 1) _sigth = 1;
	else if (_heading > 359.9) _heading = 0.0;
	else if (_heading < 0.0) _heading = 359.9;
}

void Bird::registerAttributes(){}

void Bird::serialize(){}

void Bird::setVelocity(int v) {
	_velocity = v;
}

int Bird::getVelocity() const {
	return _velocity;
}

void Bird::setSigth(int s) {
	_sigth = s;
}

int Bird::getSigth() const {
	return _sigth;
}

void Bird::setMindist(int d) {
	_mindist = d;
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

void Bird::setmaxATurn(float maxTurn) {
	_max_A_turn = maxTurn;	
}

float Bird::getmaxATurn() const{
	 return _max_A_turn;
}


void Bird::setmaxCTurn(float maxTurn) {
	_max_C_turn = maxTurn;	
}

float Bird::getmaxCTurn() const{
	 return _max_C_turn;
}


void Bird::setmaxSTurn(float maxTurn) {
	_max_S_turn = maxTurn;	
}

float Bird::getmaxSTurn() {
	 return _max_S_turn;
}

int distNearestNeigthbour(){
	
}

} // namespace Examples

