#include <Client.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Client::Client(const std::string& id, const int& sick, const float& purchaseSpeed, const float& stopping, 
                const int& stopTime, const int& entryTime) 
    : Agent(id), _sick(sick), _infected(false), _purchaseFinished(false), _itemsPurchased(0), _itemsPayed(0),
     _purchaseSpeedFactor(purchaseSpeed), _stopping(stopping), _stopTime(stopTime), _stopCounter(0), _entryTime(entryTime) {}

Client::~Client() {}

void Client::selectActions() {}

bool Client::isSick() {
    return _sick;
}

}