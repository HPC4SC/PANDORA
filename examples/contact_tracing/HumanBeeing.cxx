#include <HumanBeeing.hxx>

namespace Examples

{

HumanBeeing::HumanBeeing(const std::string& id, const bool& sick, const int& encounterRadius, 
    const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius) 
: Agent(id), _sick(sick), _infected(false), _timeSpentWithOthers(0), _encounterRadius(encounterRadius) {
    createPhone(phoneT1,phoneT2,phoneApp,signalRadius);
}

HumanBeeing::~HumanBeeing() {}

void HumanBeeing::countEncountersReal() {
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

void HumanBeeing::countEncountersRecorded() {
    if (phoneBroadcast()) {
        Engine::AgentsVector neighbours = getWorld()->getNeighbours(this, _phone->getSignalRadius());
        int counted = 0;
        Engine::AgentsVector::iterator neighbour = neighbours.begin();
        for (int i = 0; i < neighbours.size(); i++) {
            if (neighbours[i]->getId() != getId()) {
                for (int j = 0; j < _encountersRecorded.size(); j++) {
                    if (_encountersRecorded[j].first == neighbours[i]->getId()) _encountersRecorded[j].second += 1;
                    else _encountersRecorded.push_back(std::pair<std::string,int>(neighbours[i]->getId(),1));
                    HumanBeeing* person = dynamic_cast<HumanBeeing*>(neighbour->get());
                    int detection = person->phoneListen(_sick,getPosition().distance(person->getPosition()));
                    counted += detection;
                }
            }
            neighbour++;
        }       
    }
}

bool HumanBeeing::phoneBroadcast() {
    return _phone->hasApp() and (getWorld()->getCurrentStep() + _phone->getStartOffset())%300 == 0;
}

int HumanBeeing::phoneListen(const bool& sick, const double& distance) { 
    return 1.0*(_phone->hasApp() and _phone->checkDetection(sick,distance,300));
}

}