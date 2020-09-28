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
        int count = 0;
        while ((not world->checkPosition(newPosition) or world->getStaticRaster("layout").getValue(newPosition) == 255) and count < 8) {
            newPosition = agent.getPosition();
            modX = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	        newPosition._x += modX;
	        modY = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
	        newPosition._y += modY;
            count++;
        }
	    if (world->checkPosition(newPosition)) agent.setPosition(newPosition);
    }
    
    std::string RandomMoveAction::describe() const {
        return "RandomMoveAction";
    }
}