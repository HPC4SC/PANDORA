#include <LeaveAction.hxx>

#include <Exception.hxx>

namespace Examples 
{

LeaveAction::LeaveAction() {}

LeaveAction::~LeaveAction() {}

void LeaveAction::execute(Engine::Agent & agent) {
    HumanBeeing& person = (HumanBeeing&)agent;
    person.printEncounters();
    agent.remove();
}

std::string LeaveAction::describe() const {
    return "LeaveAction";
}

}