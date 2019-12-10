#include <EspaiBarca.hxx>
#include <EspaiConfig.hxx>
#include <DynamicRaster.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <Size.hxx>

namespace Examples {

    EspaiBarca::EspaiBarca(EspaiConfig *config, Engine::Scheduler *scheduler) : World(config, scheduler, true) {}

    EspaiBarca::~EspaiBarca() {}

    void EspaiBarca::createRasters() {
        const EspaiConfig &espaiConfig = (const EspaiConfig &) getConfig();
        registerStaticRaster("map", true, 0);
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("map"), espaiConfig._mapRoute,
                                                            getBoundaries()); // 0 is street 255 is building
    }

    void EspaiBarca::createAgents() {}

    void EspaiBarca::stepEnvironment() {}

}