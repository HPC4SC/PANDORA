#include <LeaveAction.hxx>
#include <GeneralState.hxx>
#include <Person.hxx>

namespace Examples {

    LeaveAction::LeaveAction() {}

    LeaveAction::~LeaveAction() {}

    void LeaveAction::execute(Engine::Agent &agent) {
        Person &person = dynamic_cast<Person&>(agent);
        std::cout << "I'm " << agent.getId() << " I'm a toruist: " << person.isTourist() << " and I no longer exist" << std::endl;
        agent.remove();
    }

    std::string LeaveAction::describe() const {
        return "LeaveAction";
    }

}
