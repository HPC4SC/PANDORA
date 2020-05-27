#include <LeaveAction.hxx>
#include <GeneralState.hxx>
#include <Person.hxx>

namespace Examples {

    LeaveAction::LeaveAction() {}

    LeaveAction::~LeaveAction() {}

    void LeaveAction::execute(Engine::Agent &agent) {
        Person & person = (Person&)agent;
        std::cout << "I'm " << agent.getId() << " and I no longer exist. My time spent is: " << person.getTimeSpent() << std::endl;
        agent.remove();
    }

    std::string LeaveAction::describe() const {
        return "LeaveAction";
    }

}
