#ifndef __Restaurant_hxx__
#define __Restaurant_hxx__

#include <RestaurantConfig.hxx>

#include <World.hxx>
#include <Point2D.hxx>

#include <map>
#include <queue>

namespace Examples
{

class Restaurant : public Engine::World
{
private:
    const RestaurantConfig& _restaurantConfig = (const RestaurantConfig &) getConfig();

    int _seedRun = _restaurantConfig.getSeed();
    int _customerId = 0;

public:
    Restaurant(Engine::Config* config, Engine::Scheduler* scheduler = 0);

    ~Restaurant();

    void createInitialRasters();

    void setupZones();

    void createAgents();

    void createCostumer();

    void step();

    std::list<Engine::Point2D<int>> getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target);

    void exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev);

    bool validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited);

    std::list<Engine::Point2D<int>> reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev);

};

}

#endif