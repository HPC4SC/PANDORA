#include <Walker.hxx>
#include <WalkAction.hxx>
#include <InfectActionOutside.hxx>

namespace Examples 
{

Walker::Walker(const std::string& id, const double& infectiousness, const bool& sick, const int& encounterRadius, 
    const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int&signalRadius, const float& stopping,
	const int& stopTime, const int& entryTime, const float& drifting, const float& speed, Street* street) 
    : HumanBeeing(id,infectiousness,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _stopping(stopping), _stopTime(stopTime),
    _stopCounter(0), _entryTime(entryTime), _drifting(drifting), _speed(speed), _street(street) {}

Walker::~Walker() {}

void Walker::selectActions() {
    _actions.push_back(new InfectActionOutside);
    if (_stopCounter > 0) _stopCounter--;
    else _actions.push_back(new WalkAction);
}

void Walker::setDirectionTop() {
    _directionTop = true;
}

void Walker::setDirectionBot() {
    _directionTop = false;
}

float Walker::getStopping() {
    return _stopping;
}

void Walker::setStopCount(const int& count) {
    _stopCounter = count;
}

int Walker::getStopTime() {
    return _stopTime;
}

bool Walker::directionTop() {
    return _directionTop;
}

float Walker::getDrifting() {
    return _drifting;
}

float Walker::getSpeed() {
    return _speed;
}

Street* Walker::getStreet() {
    return _street;
}

}