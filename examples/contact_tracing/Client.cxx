#include <Client.hxx>
#include <MoveAction.hxx>
#include <InfectAction.hxx>
#include <LeaveAction.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Client::Client(const std::string& id, const int& sick, const float& purchaseSpeed, const float& stopping, const int& stopTime, 
    const int& entryTime, const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius,
    Supermarket* super, const float& wander) 
    : HumanBeeing(id,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _purchaseFinished(false), _itemsPurchased(0), _itemsPayed(0), _purchaseSpeedFactor(purchaseSpeed),
     _stopping(stopping), _stopTime(stopTime), _stopCounter(0), _entryTime(entryTime), _super(super), _purchaseDecided(false), _wander(wander) {}

Client::~Client() {}

void Client::selectActions() {
    std::cout << "I " << getId() << " select actions" << std::endl;
    _actions.push_back(new InfectAction());
    if (_purchaseFinished) _targetPosition = _super->getRandomExit();
    else if (not _purchaseDecided) {
        int currentZone = getWorld()->getStaticRaster("layout").getValue(getPosition());
        if (currentZone >= 10 and currentZone < 100) currentZone += 100;
        if (currentZone == 201) currentZone++;
        std::vector<std::pair<int,double>> probabilities = _super->getTransitionProbabilities(currentZone);
        double sumOfWeights = 0;
        int choice;
        for (int i = 0; i < probabilities.size(); i++) sumOfWeights += probabilities[i].second;
        double rnd = Engine::GeneralState::statistics().getUniformDistValue(0,sumOfWeights*100);
        rnd /= 100;
        for (int i = 0; i < probabilities.size(); i++) {
            if (rnd < probabilities[i].second) {
                choice = i;
                break;
            }
            rnd -= probabilities[i].second;
        }
        _targetPosition = _super->pickTargetFromZone(probabilities[choice].first);
        _itemsPurchased += 1;
        _purchaseDecided = true;
    }
    if (getPosition() != _targetPosition) _actions.push_back(new MoveAction());
    else {
        if (_super->isCashier(getPosition())) {
            if (_itemsPurchased > 0) _itemsPurchased -= 1;
            else _purchaseFinished = true;
        }
        else if (_super->isExit(getPosition()) or getWorld()->getCurrentStep()%500 == 0) _actions.push_back(new LeaveAction()); //treure or
        else {
            if (Engine::GeneralState::statistics().getUniformDistValue() > 0.5) _purchaseDecided = false;
        }
    }
}

int Client::getStopCounter() {
    return _stopCounter;
}

void Client::decreaseStopCounter() {
    _stopCounter--;
}

std::list<Engine::Point2D<int>> Client::getMemory() {
    return _memory;
}

Engine::Point2D<int> Client::getTargetPosition() {
    return _targetPosition;
}

float Client::getStopping() {
    return _stopping;
}

void Client::setStopCounter(const int& stopCounter) {
    _stopCounter = stopCounter;
}

int Client::getStopTime() {
    return _stopTime;
}

void Client::setMemory(const std::list<Engine::Point2D<int>>& path) {
    _memory = path;
}

void Client::popFrontMemory() {
    _memory.pop_front();
}

float Client::getWander() {
    return _wander;
}

Supermarket* Client::getSuper() {
    return _super;
}

}