#include <Gym.hxx>
#include <Athlete.hxx>

#include <Scheduler.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Gym::Gym(Engine::Config* config, Engine::Scheduler* scheduler) : World(config, scheduler, false) {}

Gym::~Gym() {}

void Gym::createInitialRasters() {
    registerStaticRaster("layout", true, 0);
    Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("layout"), _gymConfig._mapRoute,
                getBoundaries());
    setupZones();
}

void Gym::setupZones() {
    for (int i = 0; i <= getBoundaries().right(); ++i) {
        for (int j = 0; j <= getBoundaries().bottom(); ++j) {
            Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
            std::cout << getStaticRaster("layout").getValue(candidate) << ' ';
        }
        std::cout << std::endl; 
    }
}

void createAgents() {}

void createAthlete() {}

void Gym::step() {
    std::stringstream logName;    

    if ( _step%_config->getSerializeResolution( )==0 ) {
        _scheduler->serializeRasters( _step ); 
        _scheduler->serializeAgents( _step );
    }
    createAgents();
    stepEnvironment( );
    _scheduler->updateEnvironmentState();
    _scheduler->executeAgents( );
    _scheduler->removeAgents( );
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
    std::vector<int> rowDirection =    {1,-1,0, 0,1,-1,1,-1};
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

bool isNotDoor(const Engine::Point2D<int>& point) {

}

Engine::Point2D<int> getClosestDoor() {
    
}

}