#include <InfectAction.hxx>
#include <HumanBeeing.hxx>

#include <Exception.hxx>

namespace Examples 
{

InfectAction::InfectAction() {}

InfectAction::~InfectAction() {}

void InfectAction::execute(Engine::Agent & agent) {
    HumanBeeing & person = (HumanBeeing&)agent;
    Engine::Agent * p_agent = agent.getWorld()->getAgent(agent.getId());
    Engine::AgentsVector neighbours = person.getWorld()->getNeighbours(p_agent, person.getPhonePointer()->getSignalRadius());
}

std::string InfectAction::describe() const {
    return "InfectAction";
}

}