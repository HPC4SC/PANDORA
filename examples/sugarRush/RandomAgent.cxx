
#include <RandomAgent.hxx>
#include <MoveAction.hxx>
#include <ReplacementAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>

namespace Examples
{

RandomAgent::RandomAgent(const std::string & id, const int &wealth, const int &vision, const int &metabolicRate, const int &currentAge, const int &maxAge) : Agent(id), _wealth(wealth), _vision(vision), _metabolicRate(metabolicRate), _currentAge(currentAge), _maxAge(maxAge) 
{
}

RandomAgent::~RandomAgent()
{
}

void RandomAgent::selectActions() {
	_actions.push_back(new MoveAction());
	_actions.push_back(new ReplacementAction());
}

void RandomAgent::registerAttributes() {
	registerIntAttribute("wealth");
	registerIntAttribute("vision");
	registerIntAttribute("metabolicRate");
	registerIntAttribute("currentAge");
	registerIntAttribute("maxAge");
}

void RandomAgent::serialize() {
	serializeAttribute("wealth", _wealth);
	serializeAttribute("vision", _vision);
	serializeAttribute("metabolicRate", _metabolicRate);
	serializeAttribute("currentAge", _currentAge);
	serializeAttribute("maxAge", _maxAge);
}

int RandomAgent::getWealth() const {
	return _wealth;
}

void RandomAgent::setWealth(const int &wealth) {
	_wealth = wealth;
}

int RandomAgent::getVision() const {
	return _vision;
}

int RandomAgent::getMetabolicRate() const {
	return _metabolicRate;
}

int RandomAgent::getCurrentAge() const {
	return _currentAge;
}

void RandomAgent::incCurrentAge() {
	_currentAge++;
}

int RandomAgent::getMaxAge() const {
	return _maxAge;
}

} // namespace Examples

