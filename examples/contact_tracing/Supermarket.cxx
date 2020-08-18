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

void Supermarket::createAgents() {
    if (not _first) {
        if (_step == 0) for (int i = 0; i < _supermarketConfig._numCashiers; i++) createCashier();
        if (_step%_supermarketConfig._clientRate == 0) createClient();
    }
    else _first = false;
}

void Supermarket::createCashier() {
    std::ostringstream oss;
    oss << "Cashier_" << _cashierId;
    _cashierId++;
    bool sick = false;
    if (_currentSickCashiers < _supermarketConfig._sickCashiers and _supermarketConfig._sickCashiers != -1) sick = true;
    int initWorkedTime = _supermarketConfig._cashierShift * Engine::GeneralState::statistics().getUniformDistValue(0.,1.);
    bool hasApp = Engine::GeneralState::statistics().getUniformDistValue(0.,1.) * _supermarketConfig._applicationRate;
    Cashier *cashier = new Cashier(oss.str(),sick,_supermarketConfig._cashierShift,initWorkedTime,
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
    std::ostringstream oss;
    oss << "Client_" << _clientId;
    _clientId++;
    bool sick = false;
    if (Engine::GeneralState::statistics().getUniformDistValue(0.,1.) < _supermarketConfig._sickRate) {
        std::cout << oss.str() << " is sick" << std::endl;
        sick = true;
    }
    float purchaseSpeed = Engine::GeneralState::statistics().getUniformDistValue(0.,1.);
    float stopping = Engine::GeneralState::statistics().getUniformDistValue(0.,1.) * _supermarketConfig._stopping;
    int stopTime = (int)Engine::GeneralState::statistics().getUniformDistValue(0.,1.) * _supermarketConfig._stopTime;
    bool hasApp = Engine::GeneralState::statistics().getUniformDistValue(0.,1.) * _supermarketConfig._applicationRate;
    Client *client = new Client(oss.str(),sick,purchaseSpeed,stopping,stopTime,_step,
        _supermarketConfig._phoneThreshold1,_supermarketConfig._phoneThreshold2,hasApp,_supermarketConfig._signalRadius,_supermarketConfig._encounterRadius);
    addAgent(client);
    int spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_entry.size() - 1);
    Engine::Point2D<int> spawn = _entry[spawnIndex];
    while (not this->checkPosition(spawn)) {
        spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_entry.size() - 1);
        spawn = _entry[spawnIndex];
    }
    client->setPosition(spawn);
}

void Supermarket::step() {
    std::cout << "Executing step: " << _step << std::endl;
    if (_step%_config->getSerializeResolution() == 0) {
        _scheduler->serializeRasters(_step);
        _scheduler->serializeAgents(_step);
    }
    createAgents();
    _scheduler->updateEnvironmentState();
    _scheduler->executeAgents();
    _scheduler->removeAgents();
    std::cout << "End step" << std::endl;
}

void Supermarket::devideLayout() {
    setupTransitionProbabilities();
    for (int i = 0; i <= getBoundaries().right(); ++i) {
        for (int j = 0; j <= getBoundaries().bottom(); ++j) {
            Engine::Point2D<int> candidate = Engine::Point2D<int>(j,i);
            if (getStaticRaster("layout").getValue(candidate) == 0) _obstacles.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 1) _entry.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 2) _entry.push_back(candidate); //area
            else if (getStaticRaster("layout").getValue(candidate) == 254) _exitZone.push_back(candidate); //area
            else if (getStaticRaster("layout").getValue(candidate) == 255) _exit.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 202) _cashierWorkplace.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 201) {
                _zoneTargets[202].push_back(candidate);
                _cashierTill.push_back(candidate);
            }
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
    printTransitionProbabilities();

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

Engine::Point2D<int> Supermarket::getRandomExit() {
    int index = Engine::GeneralState::statistics().getUniformDistValue(0,_exit.size());
    return _exit[index];
}

int Supermarket::getCurrentZone(const Engine::Point2D<int>& pos) {
    return getStaticRaster("layout").getValue(pos);
}

std::vector<std::pair<int,double>> Supermarket::getTransitionProbabilities(const int& zone) {
    std::map<int,std::vector<std::pair<int,double>>>::iterator it = _transitionProbabilities.find(zone);
    return it->second;
}

void Supermarket::setupTransitionProbabilities() {
    std::vector<std::pair<int,double>> probabilities;
    for (int i = 0; i < _zones.size(); i++) probabilities.push_back(std::pair<int,double>(_zones[i],0.));
    for (int i = 0; i < _zones.size(); i++) _transitionProbabilities.insert(std::pair<int,std::vector<std::pair<int,double>>>(_zones[i],probabilities));
    _transitionProbabilities.insert(std::pair<int,std::vector<std::pair<int,double>>>(1,probabilities));
    std::map<int,std::vector<std::pair<int,double>>>::iterator it = _transitionProbabilities.begin();
}

void Supermarket::printTransitionProbabilities() {
    std::map<int,std::vector<std::pair<int,double>>>::iterator aux = _transitionProbabilities.begin();
    std::cout << "_transisitionProbabilities: "<< std::endl;
    while (aux != _transitionProbabilities.end()) {
        std::cout << aux->first << " {";
        for (int i = 0; i < aux->second.size(); i++) {
            std::cout << " " << aux->second[i].first << ": " << aux->second[i].second << ",";
        }
        std::cout << "}" << std::endl;
        aux++;
    }
}

void Supermarket::setupZoneProbabilities() {
    std::vector<int> zone_tragets = {110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126};
    std::vector<double> probs = {14.7, 1.18, 2.88, 3.03, 2.93, 3.47, 9.08, 10.65, 28.3, 5.98, 15.76, 1.45, 0.59, 0, 0, 0, 0};
    for (int i = 0; i < probs.size(); i++) _zoneProbabilities.insert(std::pair<int,double>(zone_tragets[i],probs[i]));

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
    std::map<int,std::vector<std::pair<int,double>>>::iterator it = _transitionProbabilities.begin();
    while (it != _transitionProbabilities.end()) {
        if (it->first == 1) {
            for (int i = 0; i < it->second.size(); i++) {
                std::map<int,double>::iterator prob_zone = _zoneProbabilities.find(it->second[i].first);
                if (prob_zone != _zoneProbabilities.end()) it->second[i].second = prob_zone->second;
                else it->second[i].second = 0;
            }
        }
        else {
            for (int i = 0; i < it->second.size(); i++) {
                if (it->second[i].first == 202) it->second[i].second = 1.0/avgPurchasedItems;
            }
        }
        it++;
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
    for (int i = 0; i < _zones.size(); i++) {
        for (int j = 0; j < _zones.size(); j++) {
            if (_zones[i] != _zones[j]) {
                double pop = _zoneProbabilities[_zones[i]] * _zoneProbabilities[_zones[j]];
                double dist = (_centroids[_zones[i]].distance(_centroids[_zones[j]]))/7.0;
                double prov = pop/pow(dist,1.75);
                for (int k = 0; k < _transitionProbabilities[_zones[i]].size(); k++) {
                    if (_transitionProbabilities[_zones[i]][k].first == _zones[j]) _transitionProbabilities[_zones[i]][k].second = prov;
                }
            }
        }
    }
}
        
void Supermarket::normalizeTransitionProbabilities() {
    for (int i = 0; i < _zones.size(); i++) {
        for (int j = 0; j < _transitionProbabilities[_zones[i]].size(); j++) {
            if (_transitionProbabilities[_zones[i]][j].first == 202) {
                double total = 1-0 - _transitionProbabilities[_zones[i]][j].second;
                double accum = 0;
                for (int k = 0; k < _zones.size(); k++) {
                    accum += _transitionProbabilities[_zones[i]][k].second;
                }
                for (int k = 0; k < _zones.size(); k++) {
                    _transitionProbabilities[_zones[i]][k].second = _transitionProbabilities[_zones[i]][k].second * total / accum;
                }
            }
        }
    }
}

}