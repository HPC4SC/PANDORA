#include <EspaiBarca.hxx>
#include <EspaiConfig.hxx>
#include <Person.hxx>
#include <DynamicRaster.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <Size.hxx>
#include <Scheduler.hxx>
#include <Logger.hxx>

namespace Examples {

    EspaiBarca::EspaiBarca(EspaiConfig *config, Engine::Scheduler *scheduler) : World(config, scheduler, true) {}

    EspaiBarca::~EspaiBarca() {}

    void EspaiBarca::createRasters() {
        const EspaiConfig &espaiConfig = (const EspaiConfig &) getConfig();
        registerStaticRaster("map", true, 0);
        // 0 building 1 street
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
                int vision, age;
                bool tourist;
                Engine::Point2D<int> finalTarget;
                defineAgent(espaiConfig, vision, age, tourist, finalTarget);
                Person * agent = new Person(oss.str(), vision, age, tourist, finalTarget);
                addAgent(agent);
                Engine::Point2D<int> spawn = this->getRandomPosition();
                while  (getStaticRaster("map").getValue(spawn) == 0) spawn = this->getRandomPosition();
                agent->setPosition(spawn);
                log_INFO(logName.str(), getWallTime() << " new agent: " << agent);
            }
        }
    }

    void EspaiBarca::defineAgent(const EspaiConfig &espaiConfig, int &vision, int &age, bool &tourist, Engine::Point2D<int> &finalTarget) {
        vision = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentVision, espaiConfig._maxAgentVision);
        age = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentAge, espaiConfig._maxAgentAge);
        tourist = Engine::GeneralState::statistics().getUniformDistValue(0,100) > espaiConfig._provTourist;
        finalTarget = this->getRandomPosition();
    }

}