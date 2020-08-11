#include <Client.hxx>
#include <MoveAction.hxx>
#include <InfectAction.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Client::Client(const std::string& id, const int& sick, const float& purchaseSpeed, const float& stopping, const int& stopTime, 
    const int& entryTime, const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius) 
    : Agent(id), _sick(sick), _infected(false), _purchaseFinished(false), _itemsPurchased(0), _itemsPayed(0),
    _purchaseSpeedFactor(purchaseSpeed), _stopping(stopping), _stopTime(stopTime), _stopCounter(0), _entryTime(entryTime), _encounterRadius(encounterRadius),
    _timeSpentWithOthers(0) {
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

void Client::countEncountersReal() {
    Engine::AgentsVector neighbours = _world->getNeighbours(this, _encounterRadius);
    for (int i = 0; i < neighbours.size(); i++) {
        if (neighbours[i]->getId() != getId()) {
            for (int j = 0; j < _encountersReal.size(); j++) {
                if (_encountersReal[j].first == neighbours[i]->getId()) _encountersReal[j].second += 1;
                else _encountersReal.push_back(std::pair<std::string,int>(neighbours[i]->getId(),1));
                _timeSpentWithOthers += 1;
            }
        }
    }
}

void Client::countEncountersRecorded() {
    if (phoneBroadcast()) {
        Engine::AgentsVector neighbours = getWorld()->getNeighbours(this, _phone->getSignalRadius());
        int counted = 0;
        Engine::AgentsVector::iterator neighbour = neighbours.begin();
        for (int i = 0; i < neighbours.size(); i++) {
            if (neighbours[i]->getId() != getId()) {
                for (int j = 0; j < _encountersRecorded.size(); j++) {
                    if (_encountersRecorded[j].first == neighbours[i]->getId()) _encountersRecorded[j].second += 1;
                    else _encountersRecorded.push_back(std::pair<std::string,int>(neighbours[i]->getId(),1));
                    Client* client = dynamic_cast<Client*>(neighbour->get());
                    int detection = client->phoneListen(_sick,getPosition().distance(client->getPosition()));
                    counted += detection;
                }
            }
            neighbour++;
        }       
    }
}

bool Client::phoneBroadcast() {
    return _phone->hasApp() and (getWorld()->getCurrentStep() + _phone->getStartOffset())%300 == 0;
}

int Client::phoneListen(const bool& sick, const double& distance) { 
    return 1.0*(_phone->hasApp() and _phone->checkDetection(sick,distance,300));
}

}