#include <LeaveAction.hxx>
#include <GeneralState.hxx>
#include <Person.hxx>

namespace Examples {

    LeaveAction::LeaveAction() {}

    LeaveAction::~LeaveAction() {}

    void LeaveAction::execute(Engine::Agent &agent) {
        std::cout << "I'm " << agent.getId() << " and I no longer exist" << std::endl;
        Person & person = (Person&)agent;
        agent.remove();
    }

    std::string LeaveAction::describe() const {
        return "LeaveAction";
    }

}
