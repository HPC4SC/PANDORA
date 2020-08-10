#include <Supermarket.hxx>
#include <Cashier.hxx>
#include <Client.hxx>

#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Scheduler.hxx>
#include <Logger.hxx>
#include <RNGNormal.hxx>

namespace Examples
{

Supermarket::Supermarket(Engine::Config * config, Engine::Scheduler * scheduler) : World(config, scheduler, false) {}

Supermarket::~Supermarket() {}

void Supermarket::createRasters() {
    registerStaticRaster("layout", true, 0);
    Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("layout"), _supermarketConfig._mapRoute,
                getBoundaries());
    devideLayout();
}

void Supermarket::createAgents() {
    if (_step == 0) {
        for (i = 0; i < _supermarketConfig; i++) {
            std::ostringstream oss;
            oss << "Cashier_" << _cashierId;
            _cashierId++;
            Cashier *cashier = new Cashier(oss.str());
            addAgent(cashier);
            int spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_cashierWorkplacec reo .size() - 1);
            Engine::Point2D<int> spawn = _cashierWorkplace[spawnIndex];
            cashier->setPosition(spawn);
        }
    }
    if (_step%_supermarketConfig._clientRate == 0) {
        std::ostringstream oss;
        oss << "Client_" << _clientId;
        _clientId++;
        Client *client = new Client(oss.str());
        addAgent(client);
        int spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_entry.size() - 1);
        Engine::Point2D<int> spawn = _entry[spawnIndex];
        agent->setPosition(spawn);
    }
        
}

void Supermarket::step() {
    if (_step%_config->getSerializeResolution() == 0) {
        _scheduler->serializeRasters(_step);
        _scheduler->serializeAgents(_step);
    }
    createAgents();
    _scheduler->executeAgents();
    _scheduler->removeAgents();
}

void Supermarket::devideLayout() {
    for (int i = 0; i <= getBoundaries().right(); ++i) {
        for (int j = 0; j <= getBoundaries().bottom(); ++j) {
            Engine::Point2D<int> candidate = Engine::Point2D<int>(j,i);
            if (getStaticRaster("layout").getValue(candidate) == 0) _obstacles.push_back(candidate);
            if (getStaticRaster("layout").getValue(candidate) == 118) _cashierWorkplace.push_back(candidate);
            if (getStaticRaster("layout").getValue(candidate) == 86) _cashierTill.push_back(candidate);
            if (getStaticRaster("layout").getValue(candidate) == 79) _exit.push_back(candidate);
            if (getStaticRaster("layout").getValue(candidate) == 135) _exit.push_back(candidate);
            if (getStaticRaster("layout").getValue(candidate) == 31) _exitZone.push_back(candidate);
            std::cout << getStaticRaster("layout").getValue(candidate) << " ";
        }
        std::cout << std::endl;
    }
}

}