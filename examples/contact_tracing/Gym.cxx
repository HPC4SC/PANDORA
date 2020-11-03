#include <Gym.hxx>
#include <Athlete.hxx>

#include <Scheduler.hxx>
#include <GeneralState.hxx>

#include <algorithm>

namespace Examples
{

Gym::Gym(Engine::Config* config, Engine::Scheduler* scheduler) : World(config, scheduler, false) {}

Gym::~Gym() {}

void Gym::createInitialRasters() {
    registerStaticRaster("layout", true, 0);
    Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("layout"), _gymConfig._mapRoute,
                getBoundaries());
    setupZones();
    createDistributions();
}

void Gym::setupZones() {
    for (int i = 0; i <= getBoundaries().right(); ++i) {
        for (int j = 0; j <= getBoundaries().bottom(); ++j) {
            Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
            if (std::find(_zones.begin(),_zones.end(),getStaticRaster("layout").getValue(candidate)) == _zones.end()) {
                _zones.push_back(getStaticRaster("layout").getValue(candidate));
                _mapZones.insert(std::pair<int,std::vector<Engine::Point2D<int>>>(getStaticRaster("layout").getValue(candidate),{candidate}));
            }
            else if (std::find(_zones.begin(),_zones.end(),getStaticRaster("layout").getValue(candidate)) != _zones.end()) 
                _mapZones[getStaticRaster("layout").getValue(candidate)].push_back(candidate);
        }
    }
}

void Gym::createAgents() {
    if (_step%_gymConfig._agentRate == 0) createAthlete();
}

void Gym::createAthlete() {
    std::ostringstream oss;
    oss << "Athlete_" << _athleteId;
    _athleteId++;
    bool sick = false;
    if (_uniZeroOne.draw() < _streetConfig._sickRate) sick = true;
    bool hasApp = _uniZeroOne.draw() < _streetConfig._applicationRate;
    int routineType = -1;
    bool directedClass = false;
    int totalExerciceTime;
    switch (routineType) {
    case 0:
        totalExerciceTime = _uniLose.draw();
        break;

    case 1:
        totalExerciceTime = _gymConfig._definition +  _gymConfig._definition * _uniMinusOneOne;
        break;

    case 2:
        totalExerciceTime = _gymConfig._volume +  _gymConfig._volume * _uniMinusOneOne;
        break;

    case 3:
        directedClass = true;
        totalExerciceTime = currentClassTime();
        break;
    
    default:
        break;
    }

    std::list<int> exerciceList = createRoutine(routineType);
    Athlete::Athlete(oss.str(),_gymConfig._infectiousness,sick,_step,_gymConfig._phoneThreshold1,_gymConfig._phoneThreshold2,hasApp,
        _gymConfig._signalRadius, _gymConfig._encounterRadius,this,exerciceList,totalExerciceTime,directedClass) 
}

void Gym::step() {
    std::stringstream logName;    
    if ( _step%_config->getSerializeResolution()==0 ) {
        _scheduler->serializeRasters(_step); 
        _scheduler->serializeAgents(_step);
    }
    createAgents();
    stepEnvironment();
    _scheduler->updateEnvironmentState();
    _scheduler->executeAgents();
    _scheduler->removeAgents();
}

int Gym::getSeedRun() {
    return _seedRun;
}

std::list<Engine::Point2D<int>> Gym::getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target) {
    std::queue<int> rowQueue;
    std::queue<int> columnQueue;
    std::vector<std::vector<bool>> visited(_gymConfig.getSize().getHeight(), std::vector<bool>(_gymConfig.getSize().getWidth(),false));
    std::vector<std::vector<Engine::Point2D<int>>> prev(_gymConfig.getSize().getHeight(), std::vector<Engine::Point2D<int>>(_gymConfig.getSize().getWidth(),Engine::Point2D<int>(-1,-1)));
    rowQueue.push(pos._x);
    columnQueue.push(pos._y);
    visited[pos._x][pos._y] = true;
    while (not rowQueue.empty()) {
        int r = rowQueue.front();
        int c = columnQueue.front();
        rowQueue.pop();
        columnQueue.pop();
        if (r == target._x and c == target._y) break;
        exploreNeighbours(r,c,visited, rowQueue, columnQueue, prev);
    }
    return reconstructPath(pos,target,prev);
}

void Gym::exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev) {
    std::vector<int> rowDirection = {1,-1,0, 0,1,-1,1,-1};
    std::vector<int> columnDirection = {0, 0,1,-1,1,1,-1,-1};
    for (int i = 0; i < 8; i++) {
        int rr = r + rowDirection[i];
        int cc = c + columnDirection[i];
        if (validPosition(rr,cc,visited)) {
            rowQueue.push(rr);
            columnQueue.push(cc);
            visited[rr][cc] = true;
            prev[rr][cc] = Engine::Point2D<int>(r,c);
        }
    }
}

bool Gym::validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited) {
    bool res = rr >= 0 and cc >= 0  and rr < _gymConfig.getSize().getHeight() and cc < _gymConfig.getSize().getWidth() and (not visited[rr][cc]); 
    if (res) res = res and getStaticRaster("layout").getValue(Engine::Point2D<int>(rr,cc)) != 0;
    return res;
}

std::list<Engine::Point2D<int>> Gym::reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev) {
    std::list<Engine::Point2D<int>> path;
    for (Engine::Point2D<int> at = target; at != Engine::Point2D<int>(-1,-1); at = prev[at._x][at._y]) path.push_back(at);
    path.reverse();
    if (path.front() == pos) return path;
    return {};
}

bool Gym::isNotDoor(const Engine::Point2D<int>& point) {
    return std::find(_mapZones[113].begin(),_mapZones[113].end(),point) != _mapZones[113].end();
}

Engine::Point2D<int> Gym::getClosestDoor(const Engine::Point2D<int>& point) {
    double minDistance = point.distance(_mapZones[113][0]);
    int minIdx = 0;
    for (unsigned int i = 1; i < _mapZones[113].size(); i++) {
        if (minDistance > point.distance(_mapZones[113][i])) {
            minDistance = point.distance(_mapZones[113][i]);
            minIdx = i;
        }
    }
    return _mapZones[113][minIdx];
}

int Gym::getExerciceTimeFromZone(const int& zone) {
    
}

bool Gym::atExerciceZone(Engine::Point2D<int> point, const int& zone) {
    return std::find(_mapZones[zone].begin(),_mapZones[zone].end(),point) != _mapZones[zone].end();
}

Engine::Point2D<int> Gym::getTargetFromZone(const int& zone) {
    switch (zone) {
        case 1:
            return _mapZones[zone][_uni1.draw()];
            break;

        case 79:
            return _mapZones[zone][_uni79.draw()];
            break;

        case 103:
            return _mapZones[zone][_uni103.draw()];
            break;

        case 113:
            return _mapZones[zone][_uni113.draw()];
            break;

        case 120:
            return _mapZones[zone][_uni120.draw()];
            break;

        case 213:
            return _mapZones[zone][_uni213.draw()];
            break;

        case 232:
            return _mapZones[zone][_uni232.draw()];
            break;

        case 251:
            return _mapZones[zone][_uni251.draw()];
            break;

        case 255:
            return _mapZones[zone][_uni255.draw()];
            break;

        default:
            break;
    }
    return Engine::Point2D<int>(-1,-1);
}

void Gym::createDistributions() {
    _uni1 = Engine::RNGUniformInt(_seedRun,0,_mapZones[1].size() - 1);
    _uni79 = Engine::RNGUniformInt(_seedRun,0,_mapZones[79].size() - 1);
    _uni103 = Engine::RNGUniformInt(_seedRun,0,_mapZones[103].size() - 1);
    _uni113 = Engine::RNGUniformInt(_seedRun,0,_mapZones[113].size() - 1);
    _uni120 = Engine::RNGUniformInt(_seedRun,0,_mapZones[120].size() - 1);
    _uni213 = Engine::RNGUniformInt(_seedRun,0,_mapZones[213].size() - 1);
    _uni232 = Engine::RNGUniformInt(_seedRun,0,_mapZones[232].size() - 1);
    _uni251 = Engine::RNGUniformInt(_seedRun,0,_mapZones[251].size() - 1);
    _uni255 = Engine::RNGUniformInt(_seedRun,0,_mapZones[255].size() - 1);
}

std::list<int> Gym::createRoutine(int& routineType) {
    std::list<int> routine;
    if (_uniZeroOne.draw() < 0.4) {
        routineType = 3; //directed

    }
    else {
        double warmUpDraw = _uniZeroOne.draw();
        if (warmUpDraw <= 0.3) routine.insert(103);
        else if (warmUpDraw > 0.3 and warmUpDraw <= 0.6) routine.insert(232);
        else if (warmUpDraw > 0.6) routine.insert(251);

        double routineTypeDraw = _uniZeroOne.draw();
        if (routineTypeDraw <= 0.3) {
            routineType = 0; //Lose
            double exerciceDraw = _uniZeroOne.draw();
            if (exerciceDraw <= 0.25) routine.insert(103);
            else if (exerciceDraw > 0.25 and exerciceDraw <= 0.5) routine.insert(213);
            else if (exerciceDraw > 0.5 and exerciceDraw <= 0.75) routine.insert(232);
            else if (exerciceDraw > 0.75) routine.insert(251);
        }

        else if (routineTypeDraw > 0.3 and routineTypeDraw <= 0.6) {
            routineType = 1; //Definition
            double exerciceDraw = _uniZeroOne.draw();
            if (exerciceDraw <= 0.5) routine.insert(213);
            else routine.insert(251);
        }

        else if (routineTypeDraw > 0.6) {
            routineType = 2; //Volume
            double exerciceDraw = _uniZeroOne.draw();
            if (exerciceDraw <= 0.5) routine.insert(213);
            else routine.insert(251);
        }

        double coolDownDraw = _uniZeroOne.draw();
        if (coolDownDraw <= 0.3) routine.insert(103);
        else if (coolDownDraw > 0.3 and coolDownDraw <= 0.6) routine.insert(232);
        else if (coolDownDraw > 0.6) routine.insert(251);
    }
}

int currentClassTime() {}

}