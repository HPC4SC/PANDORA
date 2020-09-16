#ifndef __Train_hxx__
#define __Train_hxx__


#include <TrainConfig.hxx>

#include <World.hxx>
#include <Point2D.hxx>

#include <vector>
#include <string>
#include <list>
#include <queue>

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

        std::list<Engine::Point2D<int>> getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target);

        void exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev);

        bool validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited);

        std::list<Engine::Point2D<int>> reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev);

        Engine::Point2D<int> findClosestDoor(Engine::Point2D<int> pos);

        std::vector<Engine::Point2D<int>> getAvaliableSeats();

};

}

#endif