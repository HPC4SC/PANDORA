#include <MoveAction.hxx>
#include <World.hxx>
#include <Person.hxx>
#include <GeneralState.hxx>

namespace Examples {

    MoveAction::MoveAction() {
    }

    MoveAction::~MoveAction() {
    }

    void MoveAction::execute(Engine::Agent&agent) {
        Engine::World *world = agent.getWorld();
        Engine::Point2D<int> newPosition = agent.getPosition();
        int modX = Engine::GeneralState::statistics().getUniformDistValue(-1, 1);
        int modY = Engine::GeneralState::statistics().getUniformDistValue(-1, 1);
        newPosition._x += modX;
        newPosition._y += modY;
        std::cout << "Value a on em moc: " << world->getValue("map", agent.getPosition()) << " posicio: ("<< newPosition._x << "," << newPosition._y << ")" <<  std::endl;
        while (world->getValue("map", agent.getPosition()) == 0) {
            std::cout << "Entro al if" << std::endl;
            newPosition._x -= modX;
            newPosition._y -= modY;
            modX = Engine::GeneralState::statistics().getUniformDistValue(-1, 1);
            modY = Engine::GeneralState::statistics().getUniformDistValue(-1, 1);
            newPosition._x += modX;
            newPosition._y += modY;
            std::cout << "Value a on em moc despres de canviar: " << world->getValue("map", agent.getPosition()) << std::endl;
        }
        if(world->checkPosition(newPosition)) agent.setPosition(newPosition);
    }

    std::string MoveAction::describe() const {
        return "MoveAction";
    }

}