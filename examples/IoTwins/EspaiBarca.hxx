#ifndef __EspaiBarca_hxx
#define __EspaiBarca_hxx

#include <World.hxx>
#include <RNGNormal.hxx>
#include <RNGUniformInt.hxx>

#include <EspaiConfig.hxx>

namespace Examples {
    class EspaiBarca : public Engine::World
            {

        const EspaiConfig& _espaiConfig = (const EspaiConfig &) getConfig();

        std::vector<Engine::Point2D<int>> _spawnPoints;
        std::vector<Engine::Point2D<int>> _acces9;
        std::vector<Engine::Point2D<int>> _acces15;
        std::vector<Engine::Point2D<int>> _finalTargets;
        std::vector<Engine::Point2D<int>> _targets;
        std::vector<Engine::Point2D<int>> _counters;
        std::vector<Engine::Point2D<int>> _museum;

        bool _countersSettedUp = false;
        bool _createdDistributions = false;
        int _lastId = 0;
        int _seedRun = _espaiConfig.getSeed();

        Engine::RNGNormal _distrAcces9 = Engine::RNGNormal(0,0,0);
        Engine::RNGNormal _distrAcces15 = Engine::RNGNormal(0,0,0);
        Engine::RNGUniformInt _acces9Size;
        Engine::RNGUniformInt _acces15Size;
        Engine::RNGUniformInt _countersSize;
        Engine::RNGUniformInt _museumSize;
        Engine::RNGUniformInt _finalTargetsSize;
        Engine::RNGUniformInt _uniformZero100 = Engine::RNGUniformInt(_seedRun,0,100);
        Engine::RNGUniformInt _uniformOne25 = Engine::RNGUniformInt(_seedRun,1,25);
        Engine::RNGUniformInt _uniformZero40 = Engine::RNGUniformInt(_seedRun,0,40);
        Engine::RNGUniformInt _uniformTen80 = Engine::RNGUniformInt(_seedRun,10,80);
        Engine::RNGUniformInt _uniformOne40 = Engine::RNGUniformInt(_seedRun,1,40);
        Engine::RNGUniformInt _uniformAgentVision = Engine::RNGUniformInt(_seedRun,_espaiConfig._minAgentVision,_espaiConfig._maxAgentVision);
        Engine::RNGUniformInt _uniformVelocity = Engine::RNGUniformInt(_seedRun,_espaiConfig._minAgentVelocity,_espaiConfig._maxAgentVelocity);
        Engine::RNGUniformInt _uniformAgentAge = Engine::RNGUniformInt(_seedRun,_espaiConfig._minAgentAge,_espaiConfig._maxAgentAge);
        Engine::RNGUniformInt _uniformWallDistance = Engine::RNGUniformInt(_seedRun,_espaiConfig._minWallDistance,_espaiConfig._maxWallDistance);
        Engine::RNGUniformInt _uniformAgentDistance = Engine::RNGUniformInt(_seedRun,_espaiConfig._minAgentDistance,_espaiConfig._maxAgentDistance);

        std::vector<int> _entradesAcces9 = {6,31,45,34,73,76,143,157,223,268,258,214,285,280,237,290,268,318,256,219,229,277,199,192,144,220,284,264,194,225,292,274,152,212,215,143,176,149,143,142,195,167,90,95};
        std::vector<int> _entradesAcces15 = {12,23,20,40,25,44,54,48,46,57,49,54,40,31,60,36,58,39,64,50,43,62,89,67,72,92,75,57,48,55,63,34,56,55,87,42,75,61,62,34,38,25,61,28,29};

        int _creationIndex = 0;
        bool _firstCreation = true;

        void createInitialRasters();

        void createAgents();

        void defineAgent(int& vision, int& velocity, int& age, bool& tourist,
                        Engine::Point2D<int>& finalTarget, Engine::Point2D<int>& target, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease);

        void defineTourist(int& vision, int& velocity, int& age, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease);

        void definePerson(int& vision, int& velocity, int& age, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease);

        void step();

        void setupValidRasterPoints();

        void setupCounters();

        bool candidateTooClose(const Engine::Point2D<int>& candidate, const std::vector<Engine::Point2D<int>>& setedUpPoints);

        void createDistributions();

        bool finalTargetisMuseum(const Engine::Point2D<int> finalTarget);
        
    public:

        EspaiBarca(EspaiConfig *config, Engine::Scheduler *scheduler = 0);

        virtual ~EspaiBarca();

        int getSeedRun();

        int getUniformZeroHundred();

    };

}

#endif