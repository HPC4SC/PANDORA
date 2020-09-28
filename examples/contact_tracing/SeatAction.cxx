#include <SeatAction.hxx>

#include <GeneralState.hxx>

namespace Examples
{

SeatAction::SeatAction() {}

SeatAction::~SeatAction() {}

void SeatAction::execute(Engine::Agent & agent) {
    Passanger& passanger = (Passanger&)agent;
    Engine::World * world = agent.getWorld();
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
            int modX = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
            newPosition._x += modX;
            int modY = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
            newPosition._y += modY;
        }
    }
    
    if (world->checkPosition(newPosition)) passanger.setPosition(newPosition);
}

std::string SeatAction::describe() const {
    return "SeatAction";
}

}