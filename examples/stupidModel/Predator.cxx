#include <Predator.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <HuntAction.hxx>

namespace Examples
{

Predator::Predator( const std::string & id ) : ConcreteAgent(id){}

Predator::~Predator() {}

void Predator::selectActions() {
	_actions.push_back(new HuntAction());
}

}

