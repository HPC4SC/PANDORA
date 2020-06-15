#ifndef __EspaiBarca_hxx
#define __EspaiBarca_hxx

#include <World.hxx>
#include <RNGNormal.hxx>

namespace Examples {

    class EspaiConfig;

    class EspaiBarca : public Engine::World
            {

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

        Engine::RNGNormal _distrAcces9 = Engine::RNGNormal(0,0,0);

        Engine::RNGNormal _distrAcces15 = Engine::RNGNormal(0,0,0);

        std::vector<int> _entradesAcces9 = {6,31,45,34,73,76,143,157,223,268,258,214,285,280,237,290,268,318,256,219,229,277,199,192,144,220,284,264,194,225,292,274,152,212,215,143,176,149,143,142,195,167,90,95};

        std::vector<int> _entradesAcces15 = {12,23,20,40,25,44,54,48,46,57,49,54,40,31,60,36,58,39,64,50,43,62,89,67,72,92,75,57,48,55,63,34,56,55,87,42,75,61,62,34,38,25,61,28,29};

        int _creationIndex = 0;

        bool _firstCreation = true;

        void createRasters();

        void createAgents();

        void defineAgent(const EspaiConfig& espaiConfig, int& vision, int& velocity, int& age, bool& tourist,
                        Engine::Point2D<int>& finalTarget, Engine::Point2D<int>& target, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease);

        void defineTourist(const EspaiConfig& espaiConfig, int& vision, int& velocity, int& age, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease);

        void definePerson(const EspaiConfig& espaiConfig, int& vision, int& velocity, int& age, int& wallDistance, int& agentDistance,
                        int& maxDistanceBAgents, int& provFollow, int& interest, int& interestDecrease);

        void step();

        void setupValidRasterPoints();

        void setupCounters(const EspaiConfig& espaiConfig);

        bool candidateTooClose(const Engine::Point2D<int>& candidate, const std::vector<Engine::Point2D<int>>& setedUpPoints);

        void createDistributions(const EspaiConfig& espaiConfig);

        bool finalTargetisMuseum(const Engine::Point2D<int> finalTarget);

    public:

        EspaiBarca(EspaiConfig *config, Engine::Scheduler *scheduler = 0);

        virtual ~EspaiBarca();

    };

}

#endif