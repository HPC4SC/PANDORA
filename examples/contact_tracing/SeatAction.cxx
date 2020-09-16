#include <SeatAction.hxx>

namespace Examples
{

SeatAction::SeatAction() {}

SeatAction::~SeatAction() {}

void SeatAction::execute(Engine::Agent & agent) {
    Passanger& passanger = (Passanger&)agent;
    Engine::World * world = agent.getWorld();
    Engine::Point2D<int> newPosition = passanger.nextPosition();
    passanger.popCurrentPosition();
    if (world->checkPosition(newPosition)) passanger.setPosition(newPosition);
}
    
std::string SeatAction::describe() const {
    return "SeatAction";
}

}