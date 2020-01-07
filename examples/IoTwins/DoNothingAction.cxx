#include <DoNothingAction.hxx>
#include <GeneralState.hxx>
#include <Person.hxx>

namespace Examples {

    DoNothingAction::DoNothingAction() {
    }

    DoNothingAction::~DoNothingAction() {
    }

    void DoNothingAction::execute(Engine::Agent &agent) {
        std::cout << "I'm " << agent.getId() << " and I'm at my target YAY!!" << std::endl;
    }

    std::string DoNothingAction::describe() const {
        return "DoNothingAction";
    }

}
