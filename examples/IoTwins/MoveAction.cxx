#include <MoveAction.hxx>
#include <GeneralState.hxx>
#include <vector>

namespace Examples {

    MoveAction::MoveAction() {
    }

    MoveAction::~MoveAction() {
    }

    void MoveAction::execute(Engine::Agent&agent) {
        Engine::World *world = agent.getWorld();
        Engine::Point2D<int> newPosition = selectNextPositon(agent,world);
        if(world->checkPosition(newPosition)) agent.setPosition(newPosition);
    }

    Engine::Point2D<int> MoveAction::selectNextPositon(Engine::Agent &agent, Engine::World *world) {
        Engine::Point2D<int> newPosition = agent.getPosition();
        std::cout << "I'm in: (" << newPosition._x << "," << newPosition._y << ") before moving" << std::endl;
        int modX = Engine::GeneralState::statistics().getUniformDistValue(-1, 1);
        int modY = Engine::GeneralState::statistics().getUniformDistValue(-1, 1);
        newPosition._x += modX;
        newPosition._y += modY;
        while (world->getValue("map", agent.getPosition()) == 0) {
            newPosition._x -= modX;
            newPosition._y -= modY;
            modX = Engine::GeneralState::statistics().getUniformDistValue(-1, 1);
            modY = Engine::GeneralState::statistics().getUniformDistValue(-1, 1);
            newPosition._x += modX;
            newPosition._y += modY;
        }
        std::cout << "I'm in: (" << newPosition._x << "," << newPosition._y << ") after moving" << std::endl;
        return newPosition;
    }

    std::string MoveAction::describe() const {
        return "MoveAction";
    }

}