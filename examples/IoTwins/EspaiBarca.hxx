#ifndef __EspaiBarca_hxx
#define __EspaiBarca_hxx

#include <World.hxx>

namespace Examples {

    class EspaiConfig;

    class EspaiBarca : public Engine::World {

        void createRasters();

        void createAgents();

        void defineAgent(const EspaiConfig &espaiConfig, int &vision, int &age, bool &tourist);

    public:

        EspaiBarca(EspaiConfig *config, Engine::Scheduler *scheduler = 0);

        virtual ~EspaiBarca();

    };

}

#endif