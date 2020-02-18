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
        registerStaticRaster("entrances", true, 1);
        registerStaticRaster("finalTargets", true, 2);
        // 0 building 1 street
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("buildings"), espaiConfig._mapRoute,
                getBoundaries());
        // 0 validSpawnPoint 1 invalid
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("entrances"), espaiConfig._entrancesRoute,
                getBoundaries());
        // 0 valid finalTarget 1 noTarget
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("finalTargets"),
                espaiConfig._finalTargetsRoute, getBoundaries());
        setupValidRasterPoints();
    }

    void EspaiBarca::createAgents() {
        std::stringstream logName;
        logName << "agents_" << getId();

        const EspaiConfig &espaiConfig = (const EspaiConfig &) getConfig();
        int maxAgents = espaiConfig._numAgents - static_cast<int>(this->getNumberOfAgents());
        int agentsToCreate = Engine::GeneralState::statistics().getUniformDistValue(0,6); //TODO canviar distribucio

        for (int i = 0; i < agentsToCreate and _lastId < espaiConfig._numAgents; i++) {
            if ((i % getNumTasks()) == getId()) {
                _lastId += 1;
                std::ostringstream oss;
                oss << "Person_" << _lastId;
                int vision, age, velocity, wallDistance, agentDistance, maxDistanceBAgents, provFollow;
                bool tourist;
                Engine::Point2D<int> finalTarget;
                defineAgent(espaiConfig, vision, velocity, age, tourist, finalTarget, wallDistance, agentDistance,
                        maxDistanceBAgents, provFollow);
                Person *agent = new Person(oss.str(),vision,velocity,age,tourist,finalTarget,wallDistance,agentDistance,
                        maxDistanceBAgents,provFollow);
                addAgent(agent);
                int spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_spawnPoints.size() - 1);
                Engine::Point2D<int> spawn = _spawnPoints[spawnIndex];
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
        std::cout <<  "Inici step: " << _step << std::endl;
        _scheduler->executeAgents();
        _scheduler->removeAgents();
        log_INFO(logName.str(), getWallTime() << " finished step: " << _step);
    }

    void EspaiBarca::defineAgent(const EspaiConfig &espaiConfig, int &vision, int &velocity, int &age, bool &tourist,
                                 Engine::Point2D<int> &finalTarget, int &wallDistance, int &agentDistance,
                                 int &maxDistanceBAgents, int &provFollow) {
        vision = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentVision,
                                                                        espaiConfig._maxAgentVision);
        velocity = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentVelocity,
                                                                          espaiConfig._maxAgentVelocity);
        age = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentAge,
                                                                     espaiConfig._maxAgentAge);
        tourist = Engine::GeneralState::statistics().getUniformDistValue(0, 100) > espaiConfig._provTourist;
        finalTarget = _finalTargets[Engine::GeneralState::statistics().getUniformDistValue(0, _finalTargets.size() - 1)];
        wallDistance = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minWallDistance,
                                                                           espaiConfig._maxWallDistance);
        agentDistance = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentDistance,
                                                                            espaiConfig._maxAgentDistance);
        maxDistanceBAgents = espaiConfig._maxDistanceBAgents;
        provFollow = espaiConfig._provFollow;
    }

    void EspaiBarca::setupValidRasterPoints() {
        for (int i = 0; i < getBoundaries().right(); ++i) {
            for (int j = 0; j < getBoundaries().bottom(); ++j) {
                Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
                if (getStaticRaster("entrances").getValue(candidate) == 0) _spawnPoints.push_back(candidate);
                if (getStaticRaster("finalTargets").getValue(candidate) == 0) _finalTargets.push_back(candidate);
            }
        }
    }

}