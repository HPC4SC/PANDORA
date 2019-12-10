#include <EspaiBarca.hxx>
#include <EspaiConfig.hxx>
#include <Person.hxx>
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
        // 0 building 255 street
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("map"), espaiConfig._mapRoute,
                                                            getBoundaries());
    }

    void EspaiBarca::createAgents() {
        std::stringstream logName;
        logName << "agents_" << getId();

        const EspaiConfig &espaiConfig = (const EspaiConfig &) getConfig();
        for(int i=0; i<espaiConfig._numAgents; i++) {
            if((i%getNumTasks())==getId()) {
                std::ostringstream oss;
                oss << "Person_" << i;
                Person * agent = new Person(oss.str());
                addAgent(agent);
                agent->setRandomPosition();
                log_INFO(logName.str(), getWallTime() << " new agent: " << agent);
            }
        }
    }

}