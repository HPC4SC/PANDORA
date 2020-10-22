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

    void EspaiBarca::createInitialRasters() {
        registerStaticRaster("buildings", true, 0);
        registerStaticRaster("entrances", true, 1);
        registerStaticRaster("finalTargets", true, 2);
        registerStaticRaster("targets", true, 3);
        registerStaticRaster("counters", true, 4);
        registerStaticRaster("museum", true, 5);
        registerStaticRaster("acces9", true, 6);
        registerStaticRaster("acces15", true, 7);
        // 0 building 1 street
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("buildings"), _espaiConfig._mapRoute,
                getBoundaries());
        // 0 validSpawnPoint 1 invalid
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("entrances"), _espaiConfig._entrancesRoute,
                getBoundaries());
        // 0 valid finalTarget 1 noTarget
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("finalTargets"),
                _espaiConfig._finalTargetsRoute, getBoundaries());
        
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("targets"), _espaiConfig._targetsRoute, getBoundaries());

        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("counters"), _espaiConfig._countersRoute, getBoundaries());
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("museum"), _espaiConfig._museumRoute, getBoundaries());
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("acces9"), _espaiConfig._acces9Route, getBoundaries());
        Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("acces15"), _espaiConfig._acces15Route, getBoundaries());
        setupValidRasterPoints();
    }

    void EspaiBarca::createInitialAgents() {
        createAgents(); 
    }

    void EspaiBarca::createAgents() {
        std::stringstream logName;
        logName << "agents_" << getId();

        if (not _countersSettedUp) {
            setupCounters();
            _countersSettedUp = true;
        }

        if (not _createdDistributions) {
            createDistributions();
            _createdDistributions = true;
        }

        //int agentsToCreate_9 = _distrAcces9.draw();
        //int agentsToCreate_15 = _distrAcces15.draw();

        int agentsToCreate_9 = _entradesAcces9[_creationIndex];
        //std::cout << "creating " << agentsToCreate_9 << std::endl;
        int agentsToCreate_15 = _entradesAcces15[_creationIndex];

        int agentsToCreate = agentsToCreate_9 + agentsToCreate_15;

        for(int i = 0; i < agentsToCreate and _lastId < _espaiConfig._numAgents; i++) { 
            _lastId += 1;
            std::ostringstream oss;
            oss << "Person_" << _lastId;
            int vision, age, velocity, wallDistance, agentDistance, maxDistanceBAgents, provFollow, interest, interestDecrease;
            bool tourist;
            Engine::Point2D<int> finalTarget, target, spawn;
            defineAgent(vision, velocity, age, tourist, finalTarget, target, wallDistance, agentDistance,
                    maxDistanceBAgents, provFollow, interest, interestDecrease);
            Person* person = new Person(oss.str(),vision,velocity,age,tourist,finalTarget,target,wallDistance,agentDistance,
                    maxDistanceBAgents,provFollow,interest,interestDecrease,this);
            addAgent(person);
            if (agentsToCreate_9 > 0) {
                spawn = _acces9[_acces9Size.draw()];
                while (spawn.distance(finalTarget) < 50) {
                    finalTarget = _acces15[_acces15Size.draw()];
                }
                person->setFinalTarget(finalTarget);
                agentsToCreate_9--;
            }
            else if (agentsToCreate_15 > 0) {
                spawn = _acces15[_acces15Size.draw()];
                while (spawn.distance(finalTarget) < 50) {
                    finalTarget = _acces9[_acces9Size.draw()];
                }
                person->setFinalTarget(finalTarget);
                if (finalTargetisMuseum(finalTarget)) person->setTarget(_counters[_countersSize.draw()]);
                agentsToCreate_15--;
            }
            //std::cout << "I spawn at: " << spawn << " and my final target is: " << finalTarget << std::endl;
            person->setPosition(spawn);
            log_INFO(logName.str(), getWallTime() << " new agent: " << person);
        }
        _creationIndex++;
    }

    void EspaiBarca::step() {
        std::stringstream logName;
        logName << "simulation_" << getId();
        log_INFO(logName.str(), getWallTime() << " executing step: " << _step);

        if (_step%_config->getSerializeResolution() == 0) {
            _scheduler->serializeRasters(_step);
            _scheduler->serializeAgents(_step);
            log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " serialization done");
        }
        if (_step%900 == 0) createAgents();

        log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has executed step enviroment");
        _scheduler->updateEnvironmentState();
        _scheduler->executeAgents();
        _scheduler->removeAgents();
        log_INFO(logName.str(), getWallTime() << " finished step: " << _step);
    }

    void EspaiBarca::defineAgent(int& vision, int& velocity, int& age, bool& tourist,
                                 Engine::Point2D<int>& finalTarget, Engine::Point2D<int>& target, int& wallDistance, int& agentDistance,
                                 int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease) {
        tourist = _uniformZero100.draw() > _espaiConfig._provTourist;
        if (tourist) defineTourist(vision,velocity,age,wallDistance,agentDistance,maxDistanceBAgents,provFollow,interest,interestDecrease);
        else definePerson(vision,velocity,age,wallDistance,agentDistance,maxDistanceBAgents,provFollow,interest,interestDecrease);
        if (_uniformZero100.draw() <= _espaiConfig._provMuseum) finalTarget = _museum[_museumSize.draw()];
        else finalTarget = _finalTargets[_finalTargetsSize.draw()];
        target = Engine::Point2D<int>(-1,-1);
    }

    void EspaiBarca::defineTourist(int& vision, int& velocity, int& age, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease) {
        vision = _uniformAgentVision.draw();
        velocity = _uniformVelocity.draw();
        age = _uniformAgentAge.draw();
        wallDistance = _uniformWallDistance.draw();
        agentDistance = _uniformAgentDistance.draw();
        maxDistanceBAgents = _espaiConfig._maxDistanceBAgents;
        provFollow = _espaiConfig._provFollow;
        interest = _uniformZero100.draw();
        interestDecrease = _uniformOne25.draw();
    }

    void EspaiBarca::definePerson(int& vision, int& velocity, int& age, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease) {
        vision = _uniformAgentVision.draw();
        velocity = _uniformVelocity.draw();
        age = _uniformAgentAge.draw();
        wallDistance = _uniformWallDistance.draw();
        agentDistance = _uniformAgentDistance.draw();
        maxDistanceBAgents = _espaiConfig._maxDistanceBAgents;
        provFollow = _espaiConfig._provFollow * (_uniformZero40.draw() * 0.01);
        interest = _uniformTen80.draw();
        interestDecrease = _uniformOne40.draw();
    }

    void EspaiBarca::setupValidRasterPoints() {
        for (int i = 0; i < getBoundaries().right(); ++i) {
            for (int j = 0; j < getBoundaries().bottom(); ++j) {
                Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
                if (getStaticRaster("entrances").getValue(candidate) == 0) _spawnPoints.push_back(candidate);
                if (getStaticRaster("finalTargets").getValue(candidate) == 0) _finalTargets.push_back(candidate);
                if (getStaticRaster("targets").getValue(candidate) == 0) _targets.push_back(candidate);
                if (getStaticRaster("counters").getValue(candidate) == 0) _counters.push_back(candidate);
                if (getStaticRaster("museum").getValue(candidate) == 0) _museum.push_back(candidate);
                if (getStaticRaster("acces9").getValue(candidate) == 0) _acces9.push_back(candidate);
                if (getStaticRaster("acces15").getValue(candidate) == 0) _acces15.push_back(candidate);
            }
        }
    }

    void EspaiBarca::setupCounters() {
        std::vector<Engine::Point2D<int>> setedUpPoints;
        setedUpPoints.push_back(_counters[0]);
        for (unsigned int j = 1; j < _counters.size(); j++) {
            Engine::Point2D<int> candidate = _counters[j];
            if (not candidateTooClose(candidate,setedUpPoints) and setedUpPoints.size() < (unsigned int)_espaiConfig._numCounters) setedUpPoints.push_back(candidate);
        }
        for (unsigned int i = 0; i < setedUpPoints.size(); i++) {
            std::ostringstream oss;
            oss << "Counter_" << i;
            Counter *counter = new Counter(oss.str());
            addAgent(counter);
            counter->setPosition(setedUpPoints[i]);
        }
    }

    bool EspaiBarca::candidateTooClose(const Engine::Point2D<int>& candidate, const std::vector<Engine::Point2D<int>>& setedUpPoints) {
        for (unsigned int i = 0; i < setedUpPoints.size(); i++) {
            if (candidate.distance(setedUpPoints[i]) < 10) return true;
        }
        return false;
    }

    void EspaiBarca::createDistributions() {
        _distrAcces9 = Engine::RNGNormal(_espaiConfig.getSeed(), (double) _espaiConfig._entrance9M, (double) _espaiConfig._entrance9SD);
        _distrAcces15 = Engine::RNGNormal(_espaiConfig.getSeed(), (double) _espaiConfig._entrance15M, (double) _espaiConfig._entrance15SD);
        _acces9Size = Engine::RNGUniformInt(_seedRun,0,_acces9.size() - 1);
        _acces15Size = Engine::RNGUniformInt(_seedRun,0,_acces15.size() - 1);
        _countersSize = Engine::RNGUniformInt(_seedRun,0,_counters.size() - 1);
        _museumSize = Engine::RNGUniformInt(_seedRun,0,_museum.size() - 1);
        _finalTargetsSize = Engine::RNGUniformInt(_seedRun,0,_finalTargets.size() - 1);
    }

    bool EspaiBarca::finalTargetisMuseum(const Engine::Point2D<int> finalTarget) {
        for (unsigned int i = 0; i < _museum.size(); i++) {
            if (finalTarget == _museum[i]) return true;
        } 
        return false;
    }

    int EspaiBarca::getSeedRun() {
        return _seedRun;
    }

    int EspaiBarca::getUniformZeroHundred() {
        return _uniformZero100.draw();
    }

}