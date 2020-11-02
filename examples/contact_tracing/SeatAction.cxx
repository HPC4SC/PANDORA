#include <SeatAction.hxx>
#include <Train.hxx>

#include <GeneralState.hxx>

namespace Examples
{

SeatAction::SeatAction() {}

SeatAction::~SeatAction() {}

void SeatAction::execute(Engine::Agent & agent) {
    Passanger& passanger = (Passanger&)agent;
    Engine::World * world = agent.getWorld();
    Train* train = passanger.getTrain(); 
    Engine::Point2D<int> newPosition;
    if (passanger.getExiting()) {
        newPosition = passanger.nextPosition();
        passanger.popCurrentPosition();
    }
    else if (passanger.targetPathSet()) {
        newPosition = passanger.nextPosition();
        passanger.popCurrentPosition();
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
        }
        passanger.setPath();
    }
    if (world->checkPosition(newPosition) and newPosition.distance(agent.getPosition()) < 2) {
        passanger.setPosition(newPosition);
    }
    else passanger.setPath();
}

std::string SeatAction::describe() const {
    return "SeatAction";
}

}