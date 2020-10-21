#include <SeatAction.hxx>
#include <Train.hxx>

#include <GeneralState.hxx>

namespace Examples
{

SeatAction::SeatAction() {}

SeatAction::~SeatAction() {}

void SeatAction::execute(Engine::Agent & agent) {
    std::cout << agent.getId() << " executing SeatAction" << std::endl;
    Passanger& passanger = (Passanger&)agent;
    Engine::World * world = agent.getWorld();
    Train* train = passanger.getTrain(); 
    Engine::Point2D<int> newPosition;
    if (passanger.getExiting()) {
        newPosition = passanger.nextPosition();
        passanger.popCurrentPosition();
        std::cout << agent.getId() << " exiting position is: " << agent.getPosition() << " and nextPosition is: " << newPosition << std::endl;
    }
    else if (passanger.targetPathSet()) {
        newPosition = passanger.nextPosition();
        passanger.popCurrentPosition();
        std::cout << agent.getId() << " moving to random pos position is: " << agent.getPosition() << " and nextPosition is: " << newPosition << std::endl;
    }
    else {
        passanger.popCurrentPosition();
        newPosition = agent.getPosition();
        int modX = train->getUniMinusOneOne();
        newPosition._x += modX;
        int modY = train->getUniMinusOneOne();
        newPosition._y += modY;
        int count = 0;
        while ((not world->checkPosition(newPosition) or world->getStaticRaster("layout").getValue(newPosition) == 255) and count < 8) {
            newPosition = agent.getPosition();
            int modX = train->getUniMinusOneOne();
            newPosition._x += modX;
            int modY = train->getUniMinusOneOne();
            newPosition._y += modY;
            count++;
            std::cout << agent.getId() << " Position is: " << agent.getPosition() << " and nextPosition is: " << newPosition << std::endl;
        }
        passanger.setPath();
    }
    if (world->checkPosition(newPosition) and newPosition.distance(agent.getPosition()) < 2) {
        std::cout << agent.getId() << " Position is: " << agent.getPosition() << " and nextPosition is: " << newPosition << std::endl;
        passanger.setPosition(newPosition);
    }
    else passanger.setPath();
}

std::string SeatAction::describe() const {
    return "SeatAction";
}

}