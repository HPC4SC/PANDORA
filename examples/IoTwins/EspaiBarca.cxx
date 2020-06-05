#include <EspaiBarca.hxx>
#include <EspaiConfig.hxx>
#include <Person.hxx>
#include <DynamicRaster.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <Size.hxx>
#include <Scheduler.hxx>
#include <Logger.hxx>
#include <Counter.hxx>

namespace Examples {

    EspaiBarca::EspaiBarca(EspaiConfig *config, Engine::Scheduler *scheduler) : World(config, scheduler, true) {}

    EspaiBarca::~EspaiBarca() {}

    void EspaiBarca::createRasters() {
        const EspaiConfig &espaiConfig = (const EspaiConfig &) getConfig();
        registerStaticRaster("buildings", true, 0);
        registerStaticRaster("entrances", true, 1);
        registerStaticRaster("finalTargets", true, 2);
        registerStaticRaster("targets", true, 3);
        // 0 building 1 street
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("buildings"), espaiConfig._mapRoute,
                getBoundaries());
        // 0 validSpawnPoint 1 invalid
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("entrances"), espaiConfig._entrancesRoute,
                getBoundaries());
        // 0 valid finalTarget 1 noTarget
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("finalTargets"),
                espaiConfig._finalTargetsRoute, getBoundaries());
        
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("targets"), espaiConfig._targetsRoute, getBoundaries());
        setupValidRasterPoints();
    }

    void EspaiBarca::createAgents() {
        std::stringstream logName;
        logName << "agents_" << getId();

        const EspaiConfig &espaiConfig = (const EspaiConfig &) getConfig();
        int maxAgents = espaiConfig._numAgents - static_cast<int>(this->getNumberOfAgents());
        int agentsToCreate = Engine::GeneralState::statistics().getUniformDistValue(0,6); //TODO canviar distribucio

        if (this->getCurrentStep() == 0) setupCounters(espaiConfig);

        for (int i = 0; i < agentsToCreate and _lastId < espaiConfig._numAgents; i++) {
            if ((i % getNumTasks()) == getId()) {
                _lastId += 1;
                std::ostringstream oss;
                oss << "Person_" << _lastId;
                int vision, age, velocity, wallDistance, agentDistance, maxDistanceBAgents, provFollow, interest, interestDecrease;
                bool tourist;
                Engine::Point2D<int> finalTarget, target;
                defineAgent(espaiConfig, vision, velocity, age, tourist, finalTarget, target, wallDistance, agentDistance,
                        maxDistanceBAgents, provFollow, interest, interestDecrease);
                Person *person = new Person(oss.str(),vision,velocity,age,tourist,finalTarget,target,wallDistance,agentDistance,
                        maxDistanceBAgents,provFollow,interest,interestDecrease);
                addAgent(person);
                int spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_spawnPoints.size() - 1);
                Engine::Point2D<int> spawn = _spawnPoints[spawnIndex];
                while (spawn.distance(finalTarget) < 80) {
                    spawnIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_spawnPoints.size() - 1);
                    spawn = _spawnPoints[spawnIndex];
                }
                std::cout << "I spawn at: " << spawn << " and my final target is: " << finalTarget << std::endl;
                person->setPosition(spawn);
                log_INFO(logName.str(), getWallTime() << " new agent: " << person);
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

    void EspaiBarca::defineAgent(const EspaiConfig& espaiConfig, int& vision, int& velocity, int& age, bool& tourist,
                                 Engine::Point2D<int>& finalTarget, Engine::Point2D<int>& target, int& wallDistance, int& agentDistance,
                                 int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease) {
        tourist = Engine::GeneralState::statistics().getUniformDistValue(0, 100) > espaiConfig._provTourist;
        if (tourist) defineTourist(espaiConfig,vision,velocity,age,wallDistance,agentDistance,maxDistanceBAgents,provFollow,interest,interestDecrease);
        else definePerson(espaiConfig,vision,velocity,age,wallDistance,agentDistance,maxDistanceBAgents,provFollow,interest,interestDecrease);
        finalTarget = _finalTargets[Engine::GeneralState::statistics().getUniformDistValue(0, _finalTargets.size() - 1)];
        target = Engine::Point2D<int>(-1,-1);
    }

    void EspaiBarca::defineTourist(const EspaiConfig& espaiConfig, int& vision, int& velocity, int& age, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease) {
        vision = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentVision,
                                                                        espaiConfig._maxAgentVision);
        velocity = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentVelocity,
                                                                          espaiConfig._maxAgentVelocity);
        age = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentAge,
                                                                     espaiConfig._maxAgentAge);
        wallDistance = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minWallDistance,
                                                                           espaiConfig._maxWallDistance);
        agentDistance = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentDistance,
                                                                            espaiConfig._maxAgentDistance);
        maxDistanceBAgents = espaiConfig._maxDistanceBAgents;
        provFollow = espaiConfig._provFollow;
        interest = Engine::GeneralState::statistics().getUniformDistValue(0,100);
        interestDecrease = Engine::GeneralState::statistics().getUniformDistValue(1,25);
    }

    void EspaiBarca::definePerson(const EspaiConfig& espaiConfig, int& vision, int& velocity, int& age, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease) {
        vision = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentVision,
                                                                        espaiConfig._maxAgentVision);
        velocity = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentVelocity,
                                                                          espaiConfig._maxAgentVelocity);
        age = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentAge,
                                                                     espaiConfig._maxAgentAge);
        wallDistance = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minWallDistance,
                                                                           espaiConfig._maxWallDistance);
        agentDistance = Engine::GeneralState::statistics().getUniformDistValue(espaiConfig._minAgentDistance,
                                                                            espaiConfig._maxAgentDistance);
        maxDistanceBAgents = espaiConfig._maxDistanceBAgents;
        provFollow = espaiConfig._provFollow * (Engine::GeneralState::statistics().getUniformDistValue(0,40) * 0.01);
        interest = Engine::GeneralState::statistics().getUniformDistValue(10,80);
        interestDecrease = Engine::GeneralState::statistics().getUniformDistValue(1,40);
    }

    void EspaiBarca::setupValidRasterPoints() {
        for (int i = 0; i < getBoundaries().right(); ++i) {
            for (int j = 0; j < getBoundaries().bottom(); ++j) {
                Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
                if (getStaticRaster("entrances").getValue(candidate) == 0) _spawnPoints.push_back(candidate);
                if (getStaticRaster("finalTargets").getValue(candidate) == 0) _finalTargets.push_back(candidate);
                if (getStaticRaster("targets").getValue(candidate) == 0) _targets.push_back(candidate);
            }
        }
    }

    void EspaiBarca::setupCounters(const EspaiConfig& espaiConfig) {
        std::vector<Engine::Point2D<int>> setedUpPoints;
        setedUpPoints.push_back(_targets[0]);
        for (int j = 1; j < _targets.size(); j++) {
            Engine::Point2D<int> candidate = _targets[j];
            if (not candidateTooClose(candidate,setedUpPoints) and setedUpPoints.size() < espaiConfig._numCounters) setedUpPoints.push_back(candidate);
        }
        for (int i = 0; i < setedUpPoints.size(); i++) {
            std::cout << "setedUpPoints: " << setedUpPoints[i] << std::endl;
            std::ostringstream oss;
            oss << "Counter_" << i;
            Counter *counter = new Counter(oss.str());
            addAgent(counter);
            counter->setPosition(setedUpPoints[i]);
        }
    }

    bool EspaiBarca::candidateTooClose(const Engine::Point2D<int>& candidate, const std::vector<Engine::Point2D<int>>& setedUpPoints) {
        for (int i = 0; i < setedUpPoints.size(); i++) {
            if (candidate.distance(setedUpPoints[i]) < 10) return true;
        }
        return false;
    }

}