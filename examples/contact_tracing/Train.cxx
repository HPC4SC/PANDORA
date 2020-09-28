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
    _avaliableSeats.clear();
    if (_step == _nextStop - 14) { // TODO refactor per crear agents durant 14 s
        for (int i = 0; i < _passangerEntry.front(); i++) createPassanger();
        _passangerEntry.pop_front();
        if (not _passangerExit.empty()) {
            _agentsToLeave = _passangerExit.front();
            _passangerExit.pop_front();
        }
        else _agentsToLeave = _agents.size();
    }
    setupAvaliableSeats();
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
    addAgent(passanger);
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
    if (_step == _nextStop and not _travelTimes.empty()) {
        _nextStop = _travelTimes.front();
        _travelTimes.pop_front();
    }
    if (_step < _nextStop - 14) _atStop = false;
    else if (_passangerExit.empty()) _atStop = true;
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
    bool res = rr >= 0 and cc >= 0  and rr < _trainConfig.getSize().getWidth() and cc < _trainConfig.getSize().getHeight() and (not visited[rr][cc]);
    //if (not exiting) res = res and getStaticRaster("layout").getValue(Engine::Point2D<int>(rr,cc)) == 255;
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

void Train::setupAvaliableSeats() {
    for (unsigned int i = 0; i < _seats.size(); i++) {
        if (getAgent(_seats[i]).empty()) _avaliableSeats.push_back(_seats[i]);
    }
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

}