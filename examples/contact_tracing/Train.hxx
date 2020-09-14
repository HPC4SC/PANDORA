#ifndef __Train_hxx__
#define __Train_hxx__


#include <TrainConfig.hxx>

#include <World.hxx>

namespace Examples
{

class Train : public Engine::World
{

    private:
        const TrainConfig& _trainConfig = (const TrainConfig &) getConfig();

        int _passangerId = 0;

    public:
        Train(Engine::Config* config, Engine::Scheduler* scheduler = 0);

        ~Train();
	
	    void createAgents();

        void createPassanger();

        void setupRasters();

        void step();

};

}

#endif