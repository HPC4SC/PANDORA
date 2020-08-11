#include <Client.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Client::Client(const std::string& id, const int& sick, const float& purchaseSpeed, const float& stopping, const int& stopTime, 
    const int& entryTime, const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius) 
    : Agent(id), _sick(sick), _infected(false), _purchaseFinished(false), _itemsPurchased(0), _itemsPayed(0),
    _purchaseSpeedFactor(purchaseSpeed), _stopping(stopping), _stopTime(stopTime), _stopCounter(0), _entryTime(entryTime) {
        createPhone(phoneT1,phoneT2,phoneApp,signalRadius);
    }

Client::~Client() {}

void Client::selectActions() {}

bool Client::isSick() {
    return _sick;
}

void Client::createPhone(const int& threshold1, const int& threshold2, const bool& hasApplication, const int& signalRadius) {
    _phone = new Phone(threshold1,threshold2,hasApplication,signalRadius);
}

}