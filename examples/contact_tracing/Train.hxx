#ifndef __Train_hxx__
#define __Train_hxx__


#include <TrainConfig.hxx>

#include <World.hxx>
#include <Point2D.hxx>

#include <vector>
#include <string>
#include <list>

namespace Examples
{

class Train : public Engine::World
{

    private:
        const TrainConfig& _trainConfig = (const TrainConfig &) getConfig();

        int _passangerId = 0;

        std::vector<Engine::Point2D<int>> _seats;

        std::vector<Engine::Point2D<int>> _doors;

        std::list<int> _travelTimes;

        int _nextStop;

        std::list<int> _passangerEntry;

        std::list<int> _passangerExit;

        bool _atStop = false;

    public:
        Train(Engine::Config* config, Engine::Scheduler* scheduler = 0);

        ~Train();
	
	    void createAgents();

        void createRasters();

        void createPassanger();

        void setupRasters();

        void step();

        bool atStop();

        void setupTimes();

};

}

#endif