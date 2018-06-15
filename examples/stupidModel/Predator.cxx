#include <Predator.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <HuntAction.hxx>

namespace Examples
{

Predator::Predator( const std::string & id ) : Agent(id){}

Predator::~Predator() {}

void Predator::selectActions() {
	_actions.push_back(new HuntAction());
}

void Predator::updateState() {}

void Predator::registerAttributes() {}

void Predator::serialize() {}

}

