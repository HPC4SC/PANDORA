#include <Supermarket.hxx>
#include <Cashier.hxx>
#include <Client.hxx>

#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Scheduler.hxx>
#include <Logger.hxx>
#include <RNGNormal.hxx>

#include <algorithm>
#include <math.h>

namespace Examples
{

Supermarket::Supermarket(Engine::Config* config, Engine::Scheduler* scheduler) : World(config, scheduler, false) {}

Supermarket::~Supermarket() {}

void Supermarket::createRasters() {
    registerStaticRaster("layout", true, 0);
    Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("layout"), _supermarketConfig._mapRoute,
                getBoundaries());
    devideLayout();
}

void Supermarket::newAgents() {
    std::cout << "AgentCreation" << std::endl;
    if (_step == 0) for (int i = 0; i < _supermarketConfig._numCashiers; i++) createCashier();
    if (_step%_supermarketConfig._clientRate == 0) createClient();
}

void Supermarket::createCashier() {
    std::cout << "Create Cashier" << std::endl;
    std::ostringstream oss;
    oss << "Cashier_" << _cashierId;
    _cashierId++;
    bool sick = false;
    if (_currentSickCashiers < _supermarketConfig._sickCashiers and _supermarketConfig._sickCashiers != -1) sick = true;
    int initWorkedTime = _supermarketConfig._cashierShift * Engine::GeneralState::statistics().getUniformDistValue();
    bool hasApp = Engine::GeneralState::statistics().getUniformDistValue() < _supermarketConfig._applicationRate;
    Cashier* cashier = new Cashier(oss.str(),sick,_supermarketConfig._cashierShift,initWorkedTime,
        _supermarketConfig._phoneThreshold1,_supermarketConfig._phoneThreshold2,hasApp,_supermarketConfig._signalRadius,_supermarketConfig._encounterRadius);
    addAgent(cashier);
    int spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_cashierWorkplace.size() - 1);
    Engine::Point2D<int> spawn = _cashierWorkplace[spawnIndex];
    while (not this->checkPosition(spawn)) {
        spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_cashierWorkplace.size() - 1);
        spawn = _cashierWorkplace[spawnIndex];
    }
    cashier->setPosition(spawn);
}

void Supermarket::createClient() {
    std::cout << "Create Client" << std::endl;
    std::ostringstream oss;
    oss << "Client_" << _clientId;
    _clientId++;
    bool sick = false;
    if (Engine::GeneralState::statistics().getUniformDistValue() < _supermarketConfig._sickRate) sick = true;
    float purchaseSpeed = Engine::GeneralState::statistics().getUniformDistValue();
    float stopping = Engine::GeneralState::statistics().getUniformDistValue() * _supermarketConfig._stopping;
    int stopTime = (int)Engine::GeneralState::statistics().getUniformDistValue() * _supermarketConfig._stopTime;
    bool hasApp = Engine::GeneralState::statistics().getUniformDistValue() < _supermarketConfig._applicationRate;
    Client* client = new Client(oss.str(),sick,purchaseSpeed,stopping,stopTime,_step,
        _supermarketConfig._phoneThreshold1,_supermarketConfig._phoneThreshold2,hasApp,_supermarketConfig._signalRadius,_supermarketConfig._encounterRadius,this,
        _supermarketConfig._wander);
    addAgent(client);
    int spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_entry.size() - 1);
    Engine::Point2D<int> spawn = _entry[spawnIndex];
    while (not this->checkPosition(spawn)) {
        spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_entry.size() - 1);
        spawn = _entry[spawnIndex];
    }
    client->setPosition(spawn);
}

void Supermarket::devideLayout() {
    setupTransitionProbabilities();
    for (int i = 0; i <= getBoundaries().right(); ++i) {
        for (int j = 0; j <= getBoundaries().bottom(); ++j) {
            Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
            if (getStaticRaster("layout").getValue(candidate) == 0) _obstacles.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 1) _entry.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 2) _entry.push_back(candidate); //area
            else if (getStaticRaster("layout").getValue(candidate) == 254) _exitZone.push_back(candidate); //area
            else if (getStaticRaster("layout").getValue(candidate) == 255) _exit.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 202) {
                _cashierTill.push_back(candidate);
                _zoneTargets[202].push_back(candidate);
            }
            else if (getStaticRaster("layout").getValue(candidate) == 201) _cashierWorkplace.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) >= 10 and getStaticRaster("layout").getValue(candidate) < 100) {
                _purchaseTargets.push_back(candidate);
                _obstacles.push_back(candidate);
                _zoneTargets[getStaticRaster("layout").getValue(candidate)+100].push_back(candidate);
            }
        }
    }
    setupZoneProbabilities();
    addPurchaseProbability();
    calculateCentroids();
    calculateTransitionProbabilities();
    normalizeTransitionProbabilities();
}

void Supermarket::step( )
    {
        std::stringstream logName;
        logName << "simulation_" << getId( );
        log_INFO( logName.str( ), getWallTime( ) << " executing step: " << _step );

        if ( _step%_config->getSerializeResolution( )==0 )
        {
            _scheduler->serializeRasters( _step );
            _scheduler->serializeAgents( _step );
            log_DEBUG( logName.str( ), getWallTime( ) << " step: " << _step << " serialization done" );
        }
        newAgents();
        stepEnvironment( );
        log_DEBUG( logName.str( ), getWallTime( ) << " step: " << _step << " has executed step environment" );
        _scheduler->updateEnvironmentState();
        _scheduler->executeAgents( );
        _scheduler->removeAgents( );
        log_INFO( logName.str( ), getWallTime( ) << " finished step: " << _step );
    }

bool Supermarket::isObstacle(Engine::Point2D<int> point) {
    std::vector<Engine::Point2D<int>>::iterator it = find(_obstacles.begin(),_obstacles.end(),point);
    return it != _obstacles.end(); 
}

bool Supermarket::isExit(Engine::Point2D<int> point) {
    std::vector<Engine::Point2D<int>>::iterator it = find(_exit.begin(),_exit.end(),point);
    return it != _exit.end(); 
}

bool Supermarket::isPossibleTarget(Engine::Point2D<int> point) {
    std::vector<Engine::Point2D<int>>::iterator it = find(_purchaseTargets.begin(),_purchaseTargets.end(),point);
    return it != _purchaseTargets.end();
}

bool Supermarket::isCashier(Engine::Point2D<int> point) {
    std::vector<Engine::Point2D<int>>::iterator it = find(_cashierTill.begin(),_cashierTill.end(),point);
    return it != _cashierTill.end();
}

Engine::Point2D<int> Supermarket::getRandomExit() {
    int index = Engine::GeneralState::statistics().getUniformDistValue(0,_exit.size()-1);
    return _exit[index];
}

int Supermarket::getCurrentZone(const Engine::Point2D<int>& pos) {
    return getStaticRaster("layout").getValue(pos);
}

std::map<int,double> Supermarket::getTransitionProbabilities(const int& zone) {
    std::map<int,std::map<int,double>>::iterator it;
    if (zone == 2) return _transitionProbabilities[1];
    else {
        it = _transitionProbabilities.find(zone);
        return it->second;
    } 
}

void Supermarket::setupTransitionProbabilities() {
    std::map<int,double> probabilities;
    for (unsigned int i = 0; i < _zones.size(); i++) probabilities.insert(std::pair<int,double>(_zones[i],0.));
    for (unsigned int i = 0; i < _zones.size(); i++) _transitionProbabilities.insert(std::pair<int,std::map<int,double>>(_zones[i],probabilities));
    _transitionProbabilities.insert(std::pair<int,std::map<int,double>>(1,probabilities));
}

void Supermarket::printTransitionProbabilities() {
    std::map<int,std::map<int,double>>::iterator aux = _transitionProbabilities.begin();
    while (aux != _transitionProbabilities.end()) {
        std::cout << aux->first << " {";
        std::map<int,double>::iterator aux2 = aux->second.begin();
        while (aux2 != aux->second.end())
        for (unsigned int i = 0; i < aux->second.size(); i++) {
             std::cout << " " << aux2->first << ": " << aux2->second << ",";
             aux2++;
        }
        std::cout << "}" << std::endl;
        aux++;
    }
}

void Supermarket::setupZoneProbabilities() {
    std::vector<int> zone_tragets = {110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126};
    std::vector<double> probs = {14.7, 1.18, 2.88, 3.03, 2.93, 3.47, 9.08, 10.65, 28.3, 5.98, 15.76, 1.45, 0.59, 0, 0, 0, 0};
    for (unsigned int i = 0; i < probs.size(); i++) _zoneProbabilities.insert(std::pair<int,double>(zone_tragets[i],probs[i]));
    std::map<int,double>::iterator it = _zoneProbabilities.begin();
    while (it != _zoneProbabilities.end()) {
        if (it->second == 0 ) {
            int expandZone = Engine::GeneralState::statistics().getUniformDistValue(0,_zoneProbabilities.size()-1);
            std::map<int,double>::iterator it2 = _zoneProbabilities.find(zone_tragets[expandZone]);
            double currentProb = it2->second;
            it2->second = currentProb/2.;
            it->second = currentProb/2.;
        }
        it++;
    }
}

void Supermarket::addPurchaseProbability() {
    double avgPurchasedItems = 16.5;
    for (unsigned int i = 0; i < _zones.size(); i++) {
        _transitionProbabilities[_zones[i]][202] = 1.0/avgPurchasedItems;
        _transitionProbabilities[1][_zones[i]] = _zoneProbabilities[_zones[i]];
    }
}

void Supermarket::calculateCentroids() {
    std::map<int,std::vector<Engine::Point2D<int>>>::iterator it = _zoneTargets.begin();
    while (it != _zoneTargets.end()) {
        std::vector<Engine::Point2D<int>> targets = it->second;
        int randIndex = Engine::GeneralState::statistics().getUniformDistValue(0,targets.size()-1);
        _centroids.insert(std::pair<int,Engine::Point2D<int>>(it->first,it->second[randIndex]));
        it++;
    }
}

void Supermarket::calculateTransitionProbabilities() {
    for (unsigned int i = 0; i < _zones.size(); i++) {
        for (unsigned int j = 0; j < _zones.size(); j++) {
            if (_zones[i] != _zones[j] and _zones[j] != 202) {
                double pop = _zoneProbabilities[_zones[i]] * _zoneProbabilities[_zones[j]];
                double dist = (_centroids[_zones[i]].distance(_centroids[_zones[j]]))/7.0;
                double prob = pop/pow(dist,1.75);
                _transitionProbabilities[_zones[i]][_zones[j]] = prob;
            }
        }
    }
}

void Supermarket::normalizeTransitionProbabilities() {
    for (unsigned int i = 0; i < _zones.size(); i++) {
        if (_zones[i] != 202) {
            double total = 1-0 - _transitionProbabilities[_zones[i]][202];
            double accum = 0;
            for (unsigned int j = 0; j < _zones.size(); j++) {
                if (_zones[j] != 202)
                accum += _transitionProbabilities[_zones[i]][_zones[j]];
            }
            for (unsigned int j = 0; j < _zones.size(); j++) {
                if (_zones[j] != 202) {
                     _transitionProbabilities[_zones[i]][_zones[j]] = _transitionProbabilities[_zones[i]][_zones[j]] * total / accum;
                }
            }
        }
        else _transitionProbabilities[_zones[i]][202] = 0.;
    }
}

Engine::Point2D<int> Supermarket::pickTargetFromZone(const int& zone) {
    int randIdx = Engine::GeneralState::statistics().getUniformDistValue(0,_zoneTargets[zone].size()-1);
    return _zoneTargets[zone][randIdx];
}

std::list<Engine::Point2D<int>> Supermarket::getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target) {
    std::queue<int> rowQueue;
    std::queue<int> columnQueue;
    std::vector<std::vector<bool>> visited(_supermarketConfig.getSize().getHeight(), std::vector<bool>(_supermarketConfig.getSize().getWidth(),false));
    std::vector<std::vector<Engine::Point2D<int>>> prev(_supermarketConfig.getSize().getHeight(), std::vector<Engine::Point2D<int>>(_supermarketConfig.getSize().getWidth(),Engine::Point2D<int>(-1,-1)));

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

void Supermarket::exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev) {
    std::vector<int> rowDirection = {1,-1,0,0,-1,1,1,-1};
    std::vector<int> columnDirection = {0,0,1,-1,-1,1,-1,1};
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

bool Supermarket::validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited) {
    bool res = rr >= 0 and cc >= 0  and rr < _supermarketConfig.getSize().getHeight() and cc < _supermarketConfig.getSize().getWidth() and (not visited[rr][cc]); 
    if (res) res = res and getStaticRaster("layout").getValue(Engine::Point2D<int>(rr,cc)) != 0;
    return res;
}

std::list<Engine::Point2D<int>> Supermarket::reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev) {
    std::list<Engine::Point2D<int>> path;
    for (Engine::Point2D<int> at = target; at != Engine::Point2D<int>(-1,-1); at = prev[at._x][at._y]) path.push_back(at);
    path.reverse();
    if (path.front() == pos) return path;
    return {};
}

}