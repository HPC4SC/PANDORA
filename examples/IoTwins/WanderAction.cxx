#include <WanderAction.hxx>
#include <Person.hxx>
#include <GeneralState.hxx>

namespace Examples {
    WanderAction::WanderAction() {}

    WanderAction::~WanderAction() {}

    void WanderAction::execute(Engine::Agent &agent) {
        std::cout << "now I wander" << std::endl;
    }

    std::string WanderAction::describe() const {
        return "WanderAction";
    }
}
