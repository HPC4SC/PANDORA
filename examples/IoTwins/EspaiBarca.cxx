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
        registerStaticRaster("buildings", true, 0);
        // 0 building 1 street
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("buildings"), espaiConfig._mapRoute,
                                                            getBoundaries());
    }

    void EspaiBarca::createAgents() {
        std::stringstream logName;
        logName << "agents_" << getId();

        const EspaiConfig &espaiConfig = (const EspaiConfig &) getConfig();
        int maxAgents = espaiConfig._numAgents - static_cast<int>(this->getNumberOfAgents());
        int agentsToCreate = Engine::GeneralState::statistics().getUniformDistValue(0,maxAgents);

        for (int i = 0; i < agentsToCreate; i++) {
            if ((i % getNumTasks()) == getId()) {
                std::ostringstream oss;
                oss << "Person_" << i;
                int vision, age, velocity, wallDistance, agentDistance;
                bool tourist;
                Engine::Point2D<int> finalTarget;
                defineAgent(espaiConfig, vision, velocity, age, tourist, finalTarget, wallDistance, agentDistance);
                Person *agent = new Person(oss.str(), vision, velocity, age, tourist, finalTarget,
                                           wallDistance, agentDistance);
                addAgent(agent);
                Engine::Point2D<int> spawn = this->getRandomPosition();
                while (getStaticRaster("buildings").getValue(spawn) == 0) spawn = this->getRandomPosition();
                agent->setPosition(spawn);
                log_INFO(logName.str(), getWallTime() << " new agent: " << agent);
            }
        }
    }

    void EspaiBarca::step() {
        std::stringstream logName;
        logName << "simulation_" << getId();
        log_INFO(logName.str(), getWallTime() << " executing step: " << _step);

        if (_step%_config->getSerializeResolution() == 0) {
            _scheduler->serializeRasters(_step);
            _scheduler->serializeAgents(_step);
            log_DEBUG(logName.str(), getWallTime() << " step: " << step_ << " serialization done");
        }
        createAgents();

        log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has executed step enviroment");
        // then the agents perform their actions
        _scheduler->executeAgents();
        _scheduler->removeAgents();
        log_INFO(logName.str(), getWallTime() << " finished step: " << _step);
    }

    void EspaiBarca::defineAgent(const EspaiConfig &espaiConfig, int &vision, int &velocity, int &age, bool &tourist,
                                 Engine::Point2D<int> &finalTarget, int &wallDistance, int &agentDistance) {
        vision = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentVision,
                                                                        espaiConfig._maxAgentVision);
        velocity = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentVelocity,
                                                                          espaiConfig._maxAgentVelocity);
        age = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentAge,
                                                                     espaiConfig._maxAgentAge);
        tourist = Engine::GeneralState::statistics().getUniformDistValue(0, 100) > espaiConfig._provTourist;
        finalTarget = this->getRandomPosition();
        wallDistance = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minWallDistance,
                                                                           espaiConfig._maxWallDistance);
        agentDistance = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentDistance,
                                                                            espaiConfig._maxAgentDistance);
    }

}