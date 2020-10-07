#include <RandomMoveAction.hxx>
#include <Passanger.hxx>
#include <Train.hxx>

#include <GeneralState.hxx>

namespace Examples
{
    RandomMoveAction::RandomMoveAction() {}

    RandomMoveAction::~RandomMoveAction() {}

    void RandomMoveAction::execute(Engine::Agent & agent) {
        Passanger& passanger = (Passanger&)agent;
        Train* train = passanger.getTrain();
        Engine::World * world = agent.getWorld();
	    Engine::Point2D<int> newPosition = agent.getPosition();
	    int modX = train->getUniMinusOneOne();
	    newPosition._x += modX;
	    int modY = train->getUniMinusOneOne();
	    newPosition._y += modY;
        int count = 0;
        while ((not world->checkPosition(newPosition) or world->getStaticRaster("layout").getValue(newPosition) == 255) and count < 8) {
            newPosition = agent.getPosition();
            modX = train->getUniMinusOneOne();
	        newPosition._x += modX;
	        modY = train->getUniMinusOneOne();
	        newPosition._y += modY;
            count++;
        }
	    if (world->checkPosition(newPosition)) agent.setPosition(newPosition);
    }
    
    std::string RandomMoveAction::describe() const {
        return "RandomMoveAction";
    }
}