#include <NextPositionAction.hxx>

namespace Examples 
{

NextPositionAction::NextPositionAction() {}

NextPositionAction::~NextPositionAction() {}

void NextPositionAction::execute(Engine::Agent & agent) {
	Athlete& athlete = (Athlete&)agent;
    Engine::World* world = agent.getWorld();
     Engine::Point2D<int> nextMemoryPosition = athlete.getTargetPath().front();
    if (world->checkPosition(nextMemoryPosition)) {
		athlete.popTargetPath();
		agent.setPosition(nextMemoryPosition);
	}
    else {
        athlete.calculatePath();
    }
}

std::string NextPositionAction::describe() const {
    return "NextPositionAction";
}

}