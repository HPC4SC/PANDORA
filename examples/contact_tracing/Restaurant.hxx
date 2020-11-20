#ifndef __Restaurant_hxx__
#define __Restaurant_hxx__

#include <RestaurantConfig.hxx>

#include <World.hxx>
#include <Point2D.hxx>
#include <RNGUniformInt.hxx>
#include <RNGUniformDouble.hxx>

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
    int _remainingAgentsToCreate = 0;
    int _currentTable = -1;
    int _groupsIn = 0;
    int _currentEatTime;
    
    std::map<int,std::vector<Engine::Point2D<int>>> _tablePositions;
    std::map<int,bool> _tableOccupied;
    std::vector<int> _tables;
    std::vector<Engine::Point2D<int>> _doors;

    Engine::RNGUniformInt _uni1;
    Engine::RNGUniformInt _uni7;
    Engine::RNGUniformInt _uni8;
    Engine::RNGUniformInt _uni9;
    Engine::RNGUniformInt _uni39;
    Engine::RNGUniformInt _uni62;
    Engine::RNGUniformInt _uni79;
    Engine::RNGUniformInt _uni102;
    Engine::RNGUniformInt _uni103;
    Engine::RNGUniformInt _uni113;
    Engine::RNGUniformInt _uni196;
    Engine::RNGUniformInt _uni210;
    Engine::RNGUniformInt _uni213;
    Engine::RNGUniformInt _uni232;
    Engine::RNGUniformInt _uni236;
    Engine::RNGUniformInt _uni239;
    Engine::RNGUniformInt _uni251;
    Engine::RNGUniformInt _uniTable;
    Engine::RNGUniformDouble _uniZeroOne = Engine::RNGUniformDouble(_seedRun,0.,1.);
    Engine::RNGUniformInt _uniEatTime = Engine::RNGUniformInt(_seedRun,_restaurantConfig._minEatTime,_restaurantConfig._maxEatTime);
    Engine::RNGUniformInt _uniGroupSize = Engine::RNGUniformInt(_seedRun,_restaurantConfig._minGroupSize,_restaurantConfig._maxGroupSize);

public:
    Restaurant(Engine::Config* config, Engine::Scheduler* scheduler = 0);

    ~Restaurant();

    void createInitialRasters();

    void setupZones();

    void createAgents();

    void createCustomer(const int& table, const int& eatTime);

    void step();

    std::list<Engine::Point2D<int>> getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target);

    void exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev, const Engine::Point2D<int>& target);

    bool validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited, const Engine::Point2D<int>& target);

    std::list<Engine::Point2D<int>> reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev);

    Engine::Point2D<int> getTargetFromTable(const int& table);

    void createDistributions();

    Engine::Point2D<int> getDoor();

    int getFreeTable();

    bool targetIsDoor(const Engine::Point2D<int>& target);

    int getPositionValue(const Engine::Point2D<int>& position);

    void setTableFree(const int& table);

    int getFreeDoors();

};

}

#endif