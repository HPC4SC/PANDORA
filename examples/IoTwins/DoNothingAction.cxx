#include <DoNothingAction.hxx>
#include <GeneralState.hxx>

namespace Examples {

    DoNothingAction::DoNothingAction() {
    }

    DoNothingAction::~DoNothingAction() {
    }

    void DoNothingAction::execute(Engine::Agent &agent) {
        std::cout << "I'm at my taget YAY!!" << std::endl;
    }

    std::string DoNothingAction::describe() const {
        return "DoNothingAction";
    }

}
