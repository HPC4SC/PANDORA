#include <Train.hxx>
#include <Passanger.hxx>

#include <Scheduler.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Train::Train(Engine::Config* config, Engine::Scheduler* scheduler) : World(config, scheduler, false) {}

Train::~Train() {}
	
void Train::createRasters() {
    registerStaticRaster("layout", true, 0);
    Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("layout"), _trainConfig._mapRoute,
                getBoundaries());
    setupRasters();
    setupTimes();
}

void Train::createAgents() {
    if (_step == _nextStop - 14) { // TODO refactor per crear agents durant 14 s 
        for (int i = 0; i < _passangerEntry.front(); i++) createPassanger();
        _passangerEntry.pop_front();
        for (int i = 0; i < _passangerExit.front(); i++) {
            std::ostringstream oss;
            oss << "Passanger_" << Engine::GeneralState::statistics().getUniformDistValue(0, _agents.size()-1);
            Engine::Agent* agent = getAgent(oss.str());
            Passanger* leavingPassanger = (Passanger*)agent;
            leavingPassanger->goingToLeave();
        }
        _passangerExit.pop_front();
    }
}

void Train::createPassanger() {
    std::ostringstream oss;
    oss << "Passanger_" << _passangerId;
    _passangerId++;
    bool sick = false;
    if (Engine::GeneralState::statistics().getUniformDistValue() < _trainConfig._sickRate) sick = true;
    bool hasApp = Engine::GeneralState::statistics().getUniformDistValue() < _trainConfig._applicationRate;
    bool willSeat = Engine::GeneralState::statistics().getUniformDistValue() < _trainConfig._sittingPreference;
    Passanger* passanger = new Passanger(oss.str(),sick,_trainConfig._encounterRadius,_trainConfig._phoneThreshold1,_trainConfig._phoneThreshold2,hasApp,
        _trainConfig._signalRadius,_trainConfig._move,willSeat,this);
    int spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_doors.size() - 1);
    while (not this->checkPosition(_doors[spawnIndex])) spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_doors.size() - 1);
    passanger->setPosition(_doors[spawnIndex]);
}

void Train::setupRasters() {
    for (int i = 0;  i < getStaticRaster("layout").getSize().getWidth(); i++) {
        for (int j = 0; j < getStaticRaster("layout").getSize().getHeight(); j++) {
            Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
            if (getStaticRaster("layout").getValue(candidate) == 255) _doors.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 150) _seats.push_back(candidate);
        }
    }
}

void Train::step() {
    std::cout << "Executing step: " << _step << std::endl;
    
    if (_step%_config->getSerializeResolution() == 0) {
        _scheduler->serializeRasters(_step);
        _scheduler->serializeAgents(_step);
    }
    if (_step == _nextStop) {
        _nextStop = _travelTimes.front();
        _travelTimes.pop_front();
    }
    if (_step < _nextStop - 14) _atStop = false;
    else _atStop = true;
    createAgents();
    _scheduler->updateEnvironmentState();
    _scheduler->executeAgents();
    _scheduler->removeAgents();
}

bool Train::atStop() {
    return _step < _nextStop - 14;
}

void Train::setupTimes() {
    std::string number = "";
    for (unsigned int i = 0; i < _trainConfig._travelTimes.size(); i++) {
        if (_trainConfig._travelTimes[i] != ' ') number.push_back(_trainConfig._travelTimes[i]);
        else {
            _travelTimes.push_back(std::atoi(number.c_str()));
            number = "";
        }
    }
    number = "";
    for (unsigned int i = 0; i < _trainConfig._passangerEntry.size(); i++) {
        if (_trainConfig._passangerEntry[i] != ' ') number.push_back(_trainConfig._passangerEntry[i]);
        else {
            _passangerEntry.push_back(std::atoi(number.c_str()));
            number = "";
        }
    }
    number = "";
    for (unsigned int i = 0; i < _trainConfig._passangerExit.size(); i++) {
        if (_trainConfig._passangerExit[i] != ' ') number.push_back(_trainConfig._passangerExit[i]);
        else {
            _passangerExit.push_back(std::atoi(number.c_str()));
            number = "";
        }
    }
}

}