#include <ConcreteAgent.hxx>
#include <Statistics.hxx>
#include <World.hxx>
#include <MoveAction.hxx>
#include <EatAction.hxx>
#include <DieAction.hxx>
#include <HuntAction.hxx>

namespace Examples
{

ConcreteAgent::ConcreteAgent( const std::string & id ) : Agent(id){}

ConcreteAgent::~ConcreteAgent() {}

void ConcreteAgent::selectActions() {
	_actions.push_back(new MoveAction());
	_actions.push_back(new EatAction());
	_actions.push_back(new DieAction());
	_actions.push_back(new HuntAction());
}

}
