#include <WalkAction.hxx>
#include <Street.hxx>

#include <GeneralState.hxx>

namespace Examples
{

WalkAction::WalkAction() {}

WalkAction::~WalkAction() {}

void WalkAction::execute(Engine::Agent & agent) {
    Walker& walker = (Walker&)agent;
    Street* street = walker.getStreet();
    if (street->getUniZeroOne() < walker.getStopping()) {
        walker.setStopCount((int) walker.getStopTime() * street->getUniZeroOne());
    }
    else {
        Engine::Point2D<int> newPosition = walker.getPosition();
        int modX = 0;
        int modY = 0;
        if (walker.directionTop()) modY = 1;
        else modY = -1;
        if (street->getUniZeroOne() < walker.getDrifting()) {
            if (street->getUniZeroOne() < 0.5) modX = (int)walker.getSpeed() * 1;
            else modX = (int)walker.getSpeed() * -1;
        }
        newPosition._x += modX;
        newPosition._y += modY;
        if (insideWorld(newPosition,walker)) walker.setPosition(newPosition);
        else {
            walker.printEncounters();
            walker.remove();
        }
    }
}

std::string WalkAction::describe() const {
    return "WalkAction";
}

bool WalkAction::insideWorld(const Engine::Point2D<int>& point, const Walker& walker) {
    return walker.getWorld()->getBoundaries().contains(point);
}

}