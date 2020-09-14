#include <Train.hxx>

#include <Scheduler.hxx>
#include <GeneralState.hxx>
#include <Point2D>

namespace Examples
{

Train::Train(Engine::Config* config, Engine::Scheduler* scheduler) : World(config, scheduler, false) {}

Train::~Train() {}
	
void Train::createAgents() {
    registerStaticRaster("layout", true, 0);
    Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("layout"), _supermarketConfig._mapRoute,
                getBoundaries());
    for (int i = 0; i < getBoundaries().getSize().getHeight(); i++) {
        std::cout << std::endl;
        for (int j = 0; j < getBoundaries().getSize().getWidth(); j++) {
            std::cout << getStaticRaster("layout").getValue(Engine::Point2D<int>(i,j)) << " ";
        }
    }
    setupRasters();
}

void Train::createPassanger() {}

void Train::setupRasters() {

}

void Train::step() {
    std::cout << "Executing step: " << _step << std::endl;
    if (_step%_config->getSerializeResolution() == 0) {
        _scheduler->serializeRasters(_step);
        _scheduler->serializeAgents(_step);
    }
    createAgents();
    _scheduler->updateEnvironmentState();
    _scheduler->executeAgents();
    _scheduler->removeAgents();
}

}