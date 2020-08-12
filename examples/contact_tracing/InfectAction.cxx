#include <InfectAction.hxx>
#include <Client.hxx>

#include <Exception.hxx>

namespace Examples 
{

InfectAction::InfectAction() {}

InfectAction::~InfectAction() {}

void InfectAction::execute(Engine::Agent & agent) {
    Client & client = (Client&)agent;
    Engine::Agent * p_agent = agent.getWorld()->getAgent(agent.getId());
    Engine::AgentsVector neighbours = client.getWorld()->getNeighbours(p_agent, client.getPhone()->getSignalRadius());
}

std::string InfectAction::describe() const {
    return "InfectAction";
}

}