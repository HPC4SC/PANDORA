#include <Cashier.hxx>
#include <Client.hxx>
#include <InfectAction.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

#include <iostream>

namespace Examples 
{

Cashier::Cashier(const std::string& id, const int& sick, const int& shift, const int& workedTime, 
        const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius) 
    : Agent(id), _sick(sick), _infected(false), _shift(shift), _workedTime(workedTime), _encounterRadius(encounterRadius), _timeSpentWithOthers(0) {
        createPhone(phoneT1,phoneT2,phoneApp,signalRadius);
    }

Cashier::~Cashier() {}

void Cashier::selectActions() {}

void Cashier::updateKnowledge() {
    countEncountersReal();
    countEncountersRecorded();
}

bool Cashier::isSick() {
    return _sick;
}

void Cashier::createPhone(const int& threshold1, const int& threshold2, const bool& hasApplication, const int& signalRadius) {
    _phone = new Phone(threshold1,threshold2,hasApplication,signalRadius);
}

void Cashier::countEncountersReal() {
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

void Cashier::countEncountersRecorded() {
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

bool Cashier::phoneBroadcast() {
    return _phone->hasApp() and (getWorld()->getCurrentStep() + _phone->getStartOffset())%300 == 0;
}

}