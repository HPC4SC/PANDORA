#include <RandomMoveAction.hxx>

#include <GeneralState.hxx>

namespace Examples
{
    RandomMoveAction::RandomMoveAction() {}

    RandomMoveAction::~RandomMoveAction() {}

    void RandomMoveAction::execute(Engine::Agent & agent) {
        Engine::World * world = agent.getWorld();
	    Engine::Point2D<int> newPosition = agent.getPosition();
	    int modX = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	    newPosition._x += modX;
	    int modY = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	    newPosition._y += modY;
	    if (world->checkPosition(newPosition)) agent.setPosition(newPosition);
    }
    
    std::string RandomMoveAction::describe() const {
        return "RandomMoveAction";
    }
}