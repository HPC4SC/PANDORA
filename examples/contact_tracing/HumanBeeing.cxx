#include <HumanBeeing.hxx>

#include <GeneralState.hxx>

namespace Examples

{

HumanBeeing::HumanBeeing(const std::string& id, const bool& sick, const int& encounterRadius, 
    const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius) 
: Agent(id), _countInfected(0), _phoneActiveCount(0), _sick(sick), _infected(false),
    _timeSpentWithOthers(0), _encounterRadius(encounterRadius), _infectionTime(-1), _timeInSimulation(0)  {
    createPhone(phoneT1,phoneT2,phoneApp,signalRadius);
}

HumanBeeing::~HumanBeeing() {}

bool HumanBeeing::isSick() {
    return _sick;
}

bool HumanBeeing::isInfected() {
    return _infected;
}

Phone* HumanBeeing::getPhonePointer() {
    return _phone;
}

int HumanBeeing::getEncounterRadius() {
    return _encounterRadius;
}

void HumanBeeing::updateKnowledge() {
    _timeInSimulation++;
    countEncountersReal();
    countEncountersRecorded();
}

void HumanBeeing::createPhone(const int& threshold1, const int& threshold2, const bool& hasApplication, const int& signalRaius) {
    _phone = new Phone(threshold1,threshold2,hasApplication,signalRaius);
}

void HumanBeeing::countEncountersReal() {
    Engine::AgentsVector neighbours = _world->getNeighbours(this, _encounterRadius);
    for (unsigned int i = 0; i < neighbours.size(); i++) {
        if (neighbours[i]->getId() != getId()) {
            if (_encountersReal.size() == 0) _encountersReal.push_back(std::pair<std::string,int>(neighbours[i]->getId(),1));
            else {
                bool in = false;
                for (unsigned int j = 0; j < _encountersReal.size(); j++) {
                    if (_encountersReal[j].first == neighbours[i]->getId()) {
                        in = true;
                        _encountersReal[j].second += 1;
                    }
                }
                if (not in) _encountersReal.push_back(std::pair<std::string,int>(neighbours[i]->getId(),1));
            }
            _timeSpentWithOthers += 1;
        }
    }
}

void HumanBeeing::countEncountersRecorded() {
    if (phoneBroadcast()) {
        Engine::AgentsVector neighbours = getWorld()->getNeighbours(this, _phone->getSignalRadius());
        int counted = 0;
        Engine::AgentsVector::iterator neighbour = neighbours.begin();
        for (unsigned int i = 0; i < neighbours.size(); i++) {
            if (neighbours[i]->getId() != getId()) {
                if (_encountersRecorded.size() == 0) _encountersRecorded.push_back(std::pair<std::string,int>(neighbours[i]->getId(),1));
                else {
                    bool in = false;
                    for (unsigned int j = 0; j < _encountersRecorded.size(); j++) {
                        if (_encountersRecorded[j].first == neighbours[i]->getId()) {
                            in = true;
                            _encountersRecorded[j].second += 1;
                        }
                    }
                    if (not in) _encountersRecorded.push_back(std::pair<std::string,int>(neighbours[i]->getId(),1));
                    HumanBeeing* person = dynamic_cast<HumanBeeing*>(neighbour->get());
                    counted += person->phoneListen(_sick,getPosition().distance(person->getPosition()));
                }
            }
            neighbour++;
        }
    }
}

bool HumanBeeing::phoneBroadcast() {
    if (_phoneActiveCount > 0) {
        _phoneActiveCount--;
        return true;
    }
    bool result = _phone->hasApp() and (getWorld()->getCurrentStep() + _phone->getStartOffset())%300 == 0;
    if (result) _phoneActiveCount = 4;
    return result;
}

int HumanBeeing::phoneListen(const bool& sick, const double& distance) { 
    return (int)1.0*(_phone->hasApp() and _phone->checkDetection(sick,distance,300));
}

void HumanBeeing::getInfected() {
    _infected = true;
}

void HumanBeeing::incCountInfected() {
    _countInfected++;
}

void HumanBeeing::setInfectionTime(const int& infectionTime) {
    _infectionTime = infectionTime;
}

void HumanBeeing::printEncounters() {
    std::cout << "Encounters real by: " <<  getId() << " #"<< _encountersReal.size() << std::endl;
    for (unsigned int i = 0; i < _encountersReal.size(); i++) std::cout << _encountersReal[i].first << "," << _encountersReal[i].second << " ";
    std::cout << std::endl;
    std::cout << "Encounters recorded by: " <<  getId() << " #"<< _encountersRecorded.size() << std::endl;
    for (unsigned int i = 0; i < _encountersRecorded.size(); i++) std::cout << _encountersRecorded[i].first << "," << _encountersRecorded[i].second << " ";
    std::cout << std::endl;
}

void HumanBeeing::registerAttributes() {
    registerIntAttribute("countInfected");
    registerIntAttribute("infectionTime");
    registerIntAttribute("sick");
    registerIntAttribute("infected");
    registerIntAttribute("timeInSimulation");
    registerIntAttribute("firstBucketNotifications");
    registerIntAttribute("combinedBucketsNotifications");
    registerIntAttribute("SICKfirstBucketNotifications");
    registerIntAttribute("SICKcombinedBucketsNotifications");
}

void HumanBeeing::serialize() {
    serializeAttribute("countInfected",_countInfected);
    serializeAttribute("infectionTime",_infectionTime);
    serializeAttribute("sick",_sick);
    serializeAttribute("infected",_infected);
    serializeAttribute("timeInSimulation",_timeInSimulation);
    serializeAttribute("firstBucketNotifications",_phone->getFirstBucketNotifications());
    serializeAttribute("combinedBucketsNotifications",_phone->getCombinedNotifications());
    serializeAttribute("SICKfirstBucketNotifications",_phone->getFirstBucketSickNotifications());
    serializeAttribute("SICKcombinedBucketsNotifications",_phone->getCombinedSickNotifications());
}

}