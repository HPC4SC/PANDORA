#include <Train.hxx>
#include <Passanger.hxx>

#include <Scheduler.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Train::Train(Engine::Config* config, Engine::Scheduler* scheduler) : World(config, scheduler, false) {}

Train::~Train() {}
	
void Train::createInitialRasters() {
    registerStaticRaster("layout", true, 0);
    Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("layout"), _trainConfig._mapRoute,
                getBoundaries());
    setupRasters();
    setupTimes();
}

void Train::createAgents() {
    _avaliableSeats.clear();
    if (_step%_trainConfig._agentRate == 0 and not _entry.empty()) createPassanger();
    if (_step == _nextStop - 14) {
        if (_entry.empty()) {
            for (int i = 0; i < _passangerEntry.front(); i++) createPassanger();
            _passangerEntry.pop_front();
            if (not _passangerExit.empty()) {
                _agentsToLeave = _passangerExit.front();
                _passangerExit.pop_front();
            }
            else _agentsToLeave = _agentsByID.size();
        }
        else {
            _remainingAgents = _passangerExit.front();
            _passangerExit.pop_front();
            if (_remainingAgents > _doors.size()/2) {
                for (int i = 0; i < _doors.size()/2; i++) createPassangerFromTrain();
                _remainingAgents -= _doors.size()/2;
            }
            else for (int i = 0; i < _remainingAgents; i++) createPassangerFromTrain();
        }
        setupAvaliableSeats();
        _pickedSeats.clear();
    }
}

void Train::createPassanger() {
    std::ostringstream oss;
    oss << "Passanger_" << _passangerId;
    _passangerId++;
    bool sick = false;
    if (_uniformZeroOne.draw() < _trainConfig._sickRate) sick = true;
    bool hasApp = _uniformZeroOne.draw() < _trainConfig._applicationRate;
    bool willSeat = _uniformZeroOne.draw() < _trainConfig._sittingPreference;
    bool onPlatform = not _entry.empty();
    Passanger* passanger = new Passanger(oss.str(),_trainConfig._infectiosness,sick,_trainConfig._encounterRadius,_trainConfig._phoneThreshold1,_trainConfig._phoneThreshold2,hasApp,
        _trainConfig._signalRadius,_trainConfig._move,willSeat,this,onPlatform);
    addAgent(passanger);
    if (_entry.empty()) {
        int spawnIndex = _uniDoors.draw();
        while (not this->checkPosition(_doors[spawnIndex])) spawnIndex = _uniDoors.draw();
        passanger->setPosition(_doors[spawnIndex]);
    }
    else {
        int spawnIndex = _uniEntry.draw();
        while (not this->checkPosition(_entry[spawnIndex])) spawnIndex = _uniEntry.draw();
        passanger->setPosition(_entry[spawnIndex]);
    }
}

void Train::createPassangerFromTrain() {
    std::ostringstream oss;
    oss << "Passanger_" << _passangerId;
    _passangerId++;
    bool sick = false;
    if (_uniformZeroOne.draw() < _trainConfig._sickRate) sick = true;
    bool hasApp = _uniformZeroOne.draw() < _trainConfig._applicationRate;
    bool willSeat = _uniformZeroOne.draw() < _trainConfig._sittingPreference;
    Engine::Point2D<int> target = _entry[_uniEntry.draw()];
    Passanger* passanger = new Passanger(oss.str(),_trainConfig._infectiosness,sick,_trainConfig._encounterRadius,_trainConfig._phoneThreshold1,_trainConfig._phoneThreshold2,hasApp,
        _trainConfig._signalRadius,_trainConfig._move,willSeat,this,target);
    addAgent(passanger);
    int spawnIndex = _uniDoors.draw();
    while (not this->checkPosition(_doors[spawnIndex])) spawnIndex = _uniDoors.draw();
    passanger->setPosition(_doors[spawnIndex]);
}

void Train::setupRasters() {
    for (int i = 0;  i < getStaticRaster("layout").getSize().getWidth(); i++) {
        for (int j = 0; j < getStaticRaster("layout").getSize().getHeight(); j++) {
            Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
            if (getStaticRaster("layout").getValue(candidate) == 255) _doors.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 150) _seats.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 34) _entry.push_back(candidate);
        }
    }
    _uniDoors = Engine::RNGUniformInt(_seedRun,0,(int)_doors.size() - 1);
    if (not _entry.empty()) _uniEntry = Engine::RNGUniformInt(_seedRun,0,(int)_entry.size() - 1);
}

void Train::step() {
    if (_step%_config->getSerializeResolution() == 0) {
        _scheduler->serializeRasters(_step);
        _scheduler->serializeAgents(_step);
    }
    if (_step == _nextStop and not _travelTimes.empty()) {
        _nextStop = _travelTimes.front();
        _travelTimes.pop_front();
    }
    if (_step < _nextStop - 14) _atStop = false;
    else if (_passangerExit.empty()) {
        _atStop = true;
    }
    else _atStop = true;
    createAgents();
    _scheduler->updateEnvironmentState();
    _scheduler->executeAgents();
    _scheduler->removeAgents();
}

bool Train::atStop() {
    return _atStop;
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

std::list<Engine::Point2D<int>> Train::getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const bool& exiting) {
    std::queue<int> rowQueue;
    std::queue<int> columnQueue;
    std::vector<std::vector<bool>> visited(_trainConfig.getSize().getWidth(), std::vector<bool>(_trainConfig.getSize().getHeight(),false));
    std::vector<std::vector<Engine::Point2D<int>>> prev(_trainConfig.getSize().getWidth(), std::vector<Engine::Point2D<int>>(_trainConfig.getSize().getHeight(),Engine::Point2D<int>(-1,-1)));

    rowQueue.push(pos._x);
    columnQueue.push(pos._y);
    visited[pos._x][pos._y] = true;
    while (not rowQueue.empty()) {
        int r = rowQueue.front();
        int c = columnQueue.front();
        rowQueue.pop();
        columnQueue.pop();
        if (r == target._x and c == target._y) break;
        exploreNeighbours(r,c,visited,rowQueue,columnQueue,prev,exiting);
    }
    return reconstructPath(pos,target,prev);
}

void Train::exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev, const bool& exiting) {
    std::vector<int> rowDirection = {1,-1,0,0,-1,1,1,-1};
    std::vector<int> columnDirection = {0,0,1,-1,-1,1,-1,1};
    for (int i = 0; i < 8; i++) {
        int rr = r + rowDirection[i];
        int cc = c + columnDirection[i];
        if (validPosition(rr,cc,visited,exiting)) {
            rowQueue.push(rr);
            columnQueue.push(cc);
            visited[rr][cc] = true;
            prev[rr][cc] = Engine::Point2D<int>(r,c);
        }
    }
}

bool Train::validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited, const bool& exiting) {
    bool res = rr >= 0 and cc >= 0  and rr < _trainConfig.getSize().getWidth() and cc < _trainConfig.getSize().getHeight() and (not visited[rr][cc]) and checkPosition(Engine::Point2D<int>(rr,cc));
    return res;
}

std::list<Engine::Point2D<int>> Train::reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev) {
    std::list<Engine::Point2D<int>> path;
    for (Engine::Point2D<int> at = target; at != Engine::Point2D<int>(-1,-1); at = prev[at._x][at._y]) path.push_back(at);
    path.reverse();
    if (path.front() == pos) return path;
    return {};
}

Engine::Point2D<int> Train::findClosestDoor(Engine::Point2D<int> pos) {
    double minDistance = pos.distance(_doors[0]);
    int minIdx = 0;
    for (unsigned int i = 1; i < _doors.size(); i++) {
        if (minDistance > pos.distance(_doors[i])) {
            minDistance = pos.distance(_doors[i]);
            minIdx = i;
        }
    }
    return _doors[minIdx];
}

Engine::Point2D<int> Train::findClosestSeat(Engine::Point2D<int> pos) {
    double minDistance = pos.distance(_seats[0]);
    int minIdx = 0;
    for (unsigned int i = 1; i < _seats.size(); i++) {
        if ((minDistance > pos.distance(_seats[i])) and (_pickedSeats.end() == std::find(_pickedSeats.begin(), _pickedSeats.end(), _seats[i]))) {
            minDistance = pos.distance(_seats[i]);
            minIdx = i;
        }
    }
    _pickedSeats.push_back(_seats[minIdx]);
    return _seats[minIdx];
}

void Train::setupAvaliableSeats() {
    for (unsigned int i = 0; i < _seats.size(); i++) {
        if (getAgent(_seats[i]).empty()) _avaliableSeats.push_back(_seats[i]);
    }
    int avaliableSeats = (int)_avaliableSeats.size() - 1;
    if (avaliableSeats < 0) avaliableSeats = 0;
    _uniAvaliableSeats = Engine::RNGUniformInt(_seedRun,0,avaliableSeats);
}

int Train::getAgentsToLeave() {
    return _agentsToLeave;
}

void Train::agentLeaves() {
    _agentsToLeave--;
}

std::vector<Engine::Point2D<int>> Train::getAvaliableSeats() {
    return _avaliableSeats;
}

double Train::getUniZeroOne() {
    return _uniformZeroOne.draw();
}

int Train::getRandomIndexAvaliableSeats() {
    return _uniAvaliableSeats.draw();
}

int Train::getUniMinusOneOne() {
    return _uniMinusOneOne.draw();
}

bool Train::isDoor(const Engine::Point2D<int> point) {
    std::vector<Engine::Point2D<int>>::iterator it = std::find(_doors.begin(),_doors.end(),point);
    return it != _doors.end();
}

Engine::Point2D<int> Train::randomClosePosition(const Engine::Point2D<int> origin) {
    Engine::Point2D<int> position = getRandomPosition();
    while (origin.distance(position) > 10 or origin.distance(position) < 4 or getStaticRaster("layout").getValue(position) == 255 or getStaticRaster("layout").getValue(position) == 155) position = getRandomPosition();
    return position;
}


}