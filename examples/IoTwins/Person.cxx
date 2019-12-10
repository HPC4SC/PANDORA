#include <Person.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <MoveAction.hxx>

namespace Examples {

    Person::Person(const std::string &id) : Agent(id) {}

    Person::~Person() {}

    void Person::selectActions() {
        _actions.push_back(new MoveAction());
    }

}