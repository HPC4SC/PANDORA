#include <Client.hxx>
#include <MoveAction.hxx>
#include <InfectAction.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Client::Client(const std::string& id, const int& sick, const float& purchaseSpeed, const float& stopping, const int& stopTime, 
    const int& entryTime, const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius) 
    : HumanBeeing(id,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _purchaseFinished(false), _itemsPurchased(0), _itemsPayed(0), _purchaseSpeedFactor(purchaseSpeed),
     _stopping(stopping), _stopTime(stopTime), _stopCounter(0), _entryTime(entryTime){
    }

Client::~Client() {}

void Client::selectActions() {
    _actions.push_back(new MoveAction());
    _actions.push_back(new InfectAction());
}

}