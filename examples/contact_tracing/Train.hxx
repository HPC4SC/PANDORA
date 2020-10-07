#ifndef __Train_hxx__
#define __Train_hxx__


#include <TrainConfig.hxx>

#include <World.hxx>
#include <Point2D.hxx>
#include<RNGUniformDouble.hxx>
#include <RNGUniformInt.hxx>

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

        int _nextStop = 0;

        std::list<int> _passangerEntry;

        std::list<int> _passangerExit;

        bool _atStop = false;

        int _agentsToLeave = 0;

        std::vector<Engine::Point2D<int>> _avaliableSeats;

        int _seedRun = _trainConfig.getSeed();

        Engine::RNGUniformDouble _uniformZeroOne = Engine::RNGUniformDouble(_seedRun,0.,1.);

        Engine::RNGUniformInt _uniMinusOneOne = Engine::RNGUniformInt(_seedRun,-1,1);

        Engine::RNGUniformInt _uniAvaliableSeats = Engine::RNGUniformInt(_seedRun,0,(int)_avaliableSeats.size() - 1);

        Engine::RNGUniformInt _uniDoors = Engine::RNGUniformInt(_seedRun,0,(int)_doors.size() - 1);

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

        std::list<Engine::Point2D<int>> getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const bool& exiting);

        void exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev, const bool& exiting);

        bool validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited, const bool& exiting);

        std::list<Engine::Point2D<int>> reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev);

        Engine::Point2D<int> findClosestDoor(Engine::Point2D<int> pos);

        std::vector<Engine::Point2D<int>> getAvaliableSeats();

        int getAgentsToLeave();

        void agentLeaves();

        void setupAvaliableSeats();

        double getUniZeroOne();

        int getRandomIndexAvaliableSeats();

        int getUniMinusOneOne();

};

}

#endif