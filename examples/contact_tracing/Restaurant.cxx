#include <Restaurant.hxx>
#include <Customer.hxx>

#include <Scheduler.hxx>
#include <GeneralState.hxx>

#include <algorithm>

namespace Examples
{

Restaurant::Restaurant(Engine::Config* config, Engine::Scheduler* scheduler) : World(config, scheduler, false) {}

Restaurant::~Restaurant() {}

void Restaurant::createInitialRasters() {
    registerStaticRaster("layout", true, 0);
    Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("layout"), _restaurantConfig._mapRoute,
                getBoundaries());
    setupZones();
    createDistributions();
}

void Restaurant::setupZones() {
    for (int i = 0; i <= getBoundaries().right(); ++i) {
        for (int j = 0; j <= getBoundaries().bottom(); ++j) {
            Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
            if (std::find(_tables.begin(),_tables.end(),getStaticRaster("layout").getValue(candidate)) == _tables.end() and getStaticRaster("layout").getValue(candidate) != 133 and getStaticRaster("layout").getValue(candidate) != 0) {
                _tables.push_back(getStaticRaster("layout").getValue(candidate));
                _tablePositions.insert(std::pair<int,std::vector<Engine::Point2D<int>>>(getStaticRaster("layout").getValue(candidate),{candidate}));
            }
            else if (std::find(_tables.begin(),_tables.end(),getStaticRaster("layout").getValue(candidate)) != _tables.end()) 
                _tablePositions[getStaticRaster("layout").getValue(candidate)].push_back(candidate);
        }
    }
    for (unsigned int i = 0; i < _tables.size(); i++) _tableOccupied.insert(std::pair<int,bool>(_tables[i],false));
}

void Restaurant::createAgents() {
    if (_step%_restaurantConfig._agentRate == 0 and _agentsByID.size() < 7) createCostumer();
}

void Restaurant::createCostumer() {
    std::ostringstream oss;
    oss << "Customer_" << _customerId;
    _customerId++;
    bool sick = false;
    if (_uniZeroOne.draw() < _restaurantConfig._sickRate) sick = true;
    bool hasApp = _uniZeroOne.draw() < _restaurantConfig._applicationRate;
    int table = getFreeTable();
    if (table != -1) {
        _tableOccupied[table] = true;
        Customer* customer = new Customer(oss.str(),_restaurantConfig._infectiousness,sick,_step,_restaurantConfig._phoneThreshold1,_restaurantConfig._phoneThreshold2, 
            hasApp,_restaurantConfig._signalRadius,_restaurantConfig._encounterRadius,this,_uniEatTime.draw(),table);
        addAgent(customer);
        int draw = _uni113.draw();
        while (not this->checkPosition(_tablePositions[113][draw])){
            draw = _uni113.draw();
        }
        Engine::Point2D<int> spawn = _tablePositions[113][draw];
        customer->setPosition(spawn);
    }
}

void Restaurant::step() {
    std::stringstream logName;    
    if ( _step%_config->getSerializeResolution()==0 ) {
        _scheduler->serializeRasters(_step); 
        _scheduler->serializeAgents(_step);
    }
    std::cout << "Create Agents" << std::endl;
    createAgents();
    std::cout << "Update Env" << std::endl;
    _scheduler->updateEnvironmentState();
    std::cout << "Execute Agents" << std::endl;
    _scheduler->executeAgents();
    std::cout << "Remove Agents" << std::endl;
    _scheduler->removeAgents();
}

std::list<Engine::Point2D<int>> Restaurant::getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target) {
    std::queue<int> rowQueue;
    std::queue<int> columnQueue;
    std::vector<std::vector<bool>> visited(_restaurantConfig.getSize().getWidth(), std::vector<bool>(_restaurantConfig.getSize().getHeight(),false));
    std::vector<std::vector<Engine::Point2D<int>>> prev(_restaurantConfig.getSize().getWidth(), std::vector<Engine::Point2D<int>>(_restaurantConfig.getSize().getHeight(),Engine::Point2D<int>(-1,-1)));

    rowQueue.push(pos._x);
    columnQueue.push(pos._y);
    visited[pos._x][pos._y] = true;
    while (not rowQueue.empty()) {
        int r = rowQueue.front();
        int c = columnQueue.front();
        rowQueue.pop();
        columnQueue.pop();
        if (r == target._x and c == target._y) break;
        exploreNeighbours(r,c,visited,rowQueue,columnQueue,prev,target);
    }
    return reconstructPath(pos,target,prev);
}

void Restaurant::exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev, const Engine::Point2D<int>& target) {
    std::vector<int> rowDirection = {1,-1,0,0,-1,1,1,-1};
    std::vector<int> columnDirection = {0,0,1,-1,-1,1,-1,1};
    for (int i = 0; i < 8; i++) {
        int rr = r + rowDirection[i];
        int cc = c + columnDirection[i];
        if (validPosition(rr,cc,visited,target)) {
            rowQueue.push(rr);
            columnQueue.push(cc);
            visited[rr][cc] = true;
            prev[rr][cc] = Engine::Point2D<int>(r,c);
        }
    }
}

bool Restaurant::validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited, const Engine::Point2D<int>& target) {
    bool res = rr >= 0 and cc >= 0  and rr < _restaurantConfig.getSize().getWidth() and cc < _restaurantConfig.getSize().getHeight() and (not visited[rr][cc]) and checkPosition(Engine::Point2D<int>(rr,cc));
    if (res) res = res and getStaticRaster("layout").getValue(Engine::Point2D<int>(rr,cc)) != 0;
    if (res and (rr != target._x and cc != target._y)) res = res and getStaticRaster("layout").getValue(Engine::Point2D<int>(rr,cc)) == 255;
    if (res and getStaticRaster("layout").getValue(Engine::Point2D<int>(rr,cc)) == 113) res = true;
    return res;
}

std::list<Engine::Point2D<int>> Restaurant::reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev) {
    std::list<Engine::Point2D<int>> path;
    for (Engine::Point2D<int> at = target; at != Engine::Point2D<int>(-1,-1); at = prev[at._x][at._y]) path.push_back(at);
    path.reverse();
    if (path.front() == pos) return path;
    return {};
}

Engine::Point2D<int> Restaurant::getTargetFromTable(const int& table) {
    std::cout << "getTargetFromTable" << std::endl;
    Engine::Point2D<int> target = Engine::Point2D<int>(-1,-1);
    switch (table) {
        case 1:
            target = _tablePositions[table][_uni1.draw()];
            break;

        case 7:
            target = _tablePositions[table][_uni7.draw()];
            break;

        case 8:
            target = _tablePositions[table][_uni8.draw()];
            break;
        
        case 9:
            target = _tablePositions[table][_uni9.draw()];
            break;
        
        case 39:
            target = _tablePositions[table][_uni39.draw()];
            break;
        
        case 62:
            target = _tablePositions[table][_uni62.draw()];
            break;

        case 79:
            target = _tablePositions[table][_uni79.draw()];
            break;

        case 102:
            target = _tablePositions[table][_uni102.draw()];
            break;

        case 103:
            target = _tablePositions[table][_uni103.draw()];
            break;

        case 196:
            target = _tablePositions[table][_uni196.draw()];
            break;

        case 210:
            target = _tablePositions[table][_uni210.draw()];
            break;

        case 213:
            target = _tablePositions[table][_uni213.draw()];
            break;

        case 232:
            target = _tablePositions[table][_uni232.draw()];
            break;

        case 236:
            target = _tablePositions[table][_uni236.draw()];
            break;

        case 239:
            target = _tablePositions[table][_uni239.draw()];
            break;

        case 251:
            target = _tablePositions[table][_uni251.draw()];
            break;

        default:
            break;
    }
    if (not checkPosition(target)) getTargetFromTable(table);
    return target;
}

void Restaurant::createDistributions() {
    _uni1 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[1].size() - 1);
    _uni7 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[7].size() - 1);
    _uni8 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[8].size() - 1);
    _uni9 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[9].size() - 1);
    _uni39 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[39].size() - 1);
    _uni62 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[62].size() - 1);
    _uni79 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[79].size() - 1);
    _uni102 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[102].size() - 1);
    _uni103 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[103].size() - 1);
    _uni113 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[113].size() - 1);
    _uni196 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[196].size() - 1);
    _uni210 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[210].size() - 1);
    _uni213 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[213].size() - 1);
    _uni232 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[232].size() - 1);
    _uni236 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[236].size() - 1);
    _uni239 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[239].size() - 1);
    _uni251 = Engine::RNGUniformInt(_seedRun,0,_tablePositions[251].size() - 1);
    _uniTable = Engine::RNGUniformInt(_seedRun,0,_tables.size() -1);
}

Engine::Point2D<int> Restaurant::getDoor() {
    return _tablePositions[113][_uni113.draw()];
}

int Restaurant::getFreeTable() {
    int table = _tables[_uniTable.draw()];
    bool occupied = _tableOccupied[table];
    unsigned int count = 0;
    while (occupied and count < _tables.size()) {
        table = _tables[_uniTable.draw()];
        occupied = _tableOccupied[table];
        count++;
    }
    if (count == _tables.size()) table = -1;
    return table;
}

bool Restaurant::targetIsDoor(const Engine::Point2D<int>& target) {
    return getStaticRaster("layout").getValue(target) ==  113; 
} 

}