#ifndef __EspaiBarca_hxx
#define __EspaiBarca_hxx

#include <World.hxx>

namespace Examples {

    class EspaiConfig;

    class EspaiBarca : public Engine::World
            {

        std::vector<Engine::Point2D<int>> _spawnPoints;

        std::vector<Engine::Point2D<int>> _finalTargets;

        std::vector<Engine::Point2D<int>> _targets;

        bool _countersSettedUp = false;

        int _lastId = 0;

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

    public:

        EspaiBarca(EspaiConfig *config, Engine::Scheduler *scheduler = 0);

        virtual ~EspaiBarca();

    };

}

#endif