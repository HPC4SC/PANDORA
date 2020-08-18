#include <Client.hxx>
#include <MoveAction.hxx>
#include <InfectAction.hxx>
#include <LeaveAction.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Client::Client(const std::string& id, const int& sick, const float& purchaseSpeed, const float& stopping, const int& stopTime, 
    const int& entryTime, const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius) 
    : HumanBeeing(id,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _purchaseFinished(false), _itemsPurchased(0), _itemsPayed(0), _purchaseSpeedFactor(purchaseSpeed),
     _stopping(stopping), _stopTime(stopTime), _stopCounter(0), _entryTime(entryTime) {}

Client::~Client() {}

void Client::selectActions() {
    const Engine::World* world = getWorld();
    Supermarket& super = (Supermarket&)world;
    if (_purchaseFinished) _targetPosition = super.getRandomExit();
    else {
        int currentZone = super.getCurrentZone(getPosition());
        std::vector<std::pair<int,double>> probabilities = super.getTransitionProbabilities(currentZone);
    }
    _actions.push_back(new MoveAction());
    _actions.push_back(new InfectAction());
    if (getWorld()->getCurrentStep()%500 == 0)  _actions.push_back(new LeaveAction());
}

}