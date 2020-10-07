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
    else {
        newPosition = passanger.nextPosition();
        passanger.popCurrentPosition();
        while (not world->checkPosition(newPosition) or world->getStaticRaster("layout").getValue(newPosition) == 255) {
            newPosition = agent.getPosition();
            int modX = train->getUniMinusOneOne();
            newPosition._x += modX;
            int modY = train->getUniMinusOneOne();
            newPosition._y += modY;
        }
    }
    
    if (world->checkPosition(newPosition)) passanger.setPosition(newPosition);
}

std::string SeatAction::describe() const {
    return "SeatAction";
}

}