#ifndef __EspaiBarca_hxx
#define __EspaiBarca_hxx

#include <World.hxx>

namespace Examples {

    class EspaiConfig;

    class EspaiBarca : public Engine::World {

        std::vector<Engine::Point2D<int>> _spawnPoints;

        void createRasters();

        void createAgents();

        void defineAgent(const EspaiConfig &espaiConfig, int &vision, int &velocity, int &age, bool &tourist,
                         Engine::Point2D<int> &finalTarget, int &wallDistance, int &agentDistance,
                         int &maxDistanceBAgents);

        void step();

        void setupValidSpawnPoints();

    public:

        EspaiBarca(EspaiConfig *config, Engine::Scheduler *scheduler = 0);

        virtual ~EspaiBarca();

    };

}

#endif