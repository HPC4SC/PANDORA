#include <GymConfig.hxx>

#include <World.hxx>
#include <Point2D.hxx>

#include <map>
#include <vector>
#include <queue>
#include <list>

namespace Examples
{

class Gym : public Engine::World
{

private:
    const GymConfig& _gymConfig = (const GymConfig &) getConfig();

    int _seedRun = _gymConfig.getSeed();

    int _athleteId = 0;

    std::map<int,std::vector<Engine::Point2D<int>>> _mapZones;

public:
    Gym(Engine::Config* config, Engine::Scheduler* scheduler = 0);

    ~Gym();

    void createInitialRasters();

    void setupZones();

    void createAgents();

    void createAthlete();

    void step();

    int getSeedRun();

    std::list<Engine::Point2D<int>> getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target);

    void exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev);

    bool validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited);

    std::list<Engine::Point2D<int>> reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev);

    bool isNotDoor(const Engine::Point2D<int>& point);

    Engine::Point2D<int> getClosestDoor();
};

}