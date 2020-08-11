#include <Supermarket.hxx>
#include <Cashier.hxx>
#include <Client.hxx>

#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Scheduler.hxx>
#include <Logger.hxx>
#include <RNGNormal.hxx>

#include <algorithm>

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
    if (_step == 0) {
        for (int i = 0; i < _supermarketConfig._numCashiers; i++) createCashier();
    }
    if (_step%_supermarketConfig._clientRate == 0) createClient();
}

void Supermarket::createCashier() {
    std::ostringstream oss;
    oss << "Cashier_" << _cashierId;
    _cashierId++;
    bool sick = false;
    if (_currentSickCashiers < _supermarketConfig._sickCashiers and _supermarketConfig._sickCashiers != -1) sick = true;
    int initWorkedTime = _supermarketConfig._cashierShift * Engine::RNGNormal(_supermarketConfig.getSeed(), 0, 1.0).draw();
    Cashier *cashier = new Cashier(oss.str(),sick,_supermarketConfig._cashierShift,initWorkedTime);
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
    if (Engine::RNGNormal(_supermarketConfig.getSeed(), 0, 1.0).draw() < _supermarketConfig._sickRate) sick = true;
    float purchaseSpeed = Engine::RNGNormal(_supermarketConfig.getSeed(), 0, 1.0).draw();
    float stopping = Engine::RNGNormal(_supermarketConfig.getSeed(), 0, 1.0).draw() * _supermarketConfig._stopping;
    int stopTime = (int)Engine::RNGNormal(_supermarketConfig.getSeed(), 0, 1.0).draw() * _supermarketConfig._stopTime;
    Client *client = new Client(oss.str(),sick,purchaseSpeed,stopping,stopTime,_step);
    addAgent(client);
    int spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_entry.size() - 1);
    Engine::Point2D<int> spawn = _entry[spawnIndex];
    while (not this->checkPosition(spawn)) {
        spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_cashierWorkplace.size() - 1);
        spawn = _cashierWorkplace[spawnIndex];
    }
    client->setPosition(spawn);
}

void Supermarket::step() {
    if (_step%_config->getSerializeResolution() == 0) {
        _scheduler->serializeRasters(_step);
        _scheduler->serializeAgents(_step);
    }
    createAgents();
    _scheduler->updateEnvironmentState();
    _scheduler->executeAgents();
    _scheduler->removeAgents();
}

void Supermarket::devideLayout() {
    for (int i = 0; i <= getBoundaries().right(); ++i) {
        for (int j = 0; j <= getBoundaries().bottom(); ++j) {
            Engine::Point2D<int> candidate = Engine::Point2D<int>(j,i);
            if (getStaticRaster("layout").getValue(candidate) == 0) _obstacles.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 118) _cashierWorkplace.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 86) _cashierTill.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 79) _exit.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 135) _exit.push_back(candidate);
            else if (getStaticRaster("layout").getValue(candidate) == 31) _exitZone.push_back(candidate);
            else _purchaseTargets.push_back(candidate);
        }
    }
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

}