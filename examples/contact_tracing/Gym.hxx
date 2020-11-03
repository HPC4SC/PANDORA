#ifndef __Gym_hxx__
#define __Gym_hxx__

#include <GymConfig.hxx>

#include <World.hxx>
#include <Point2D.hxx>
#include <RNGUniformInt.hxx>
#include <RNGUniformDouble.hxx>

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
    int _directed1 = 0;
    int _directed2 = 0;
    int _directed3 = 0;
    int _athleteId = 0;

    std::map<int,std::vector<Engine::Point2D<int>>> _mapZones;

    std::vector<int> _zones;
    std::vector<std::pair<int,int>> _exerciceTime;

    Engine::RNGUniformInt _uni1;
    Engine::RNGUniformInt _uni79;
    Engine::RNGUniformInt _uni103;
    Engine::RNGUniformInt _uni113;
    Engine::RNGUniformInt _uni120;
    Engine::RNGUniformInt _uni213;
    Engine::RNGUniformInt _uni232;
    Engine::RNGUniformInt _uni251;
    Engine::RNGUniformInt _uni255;
    Engine::RNGUniformInt _uniWarmUp = Engine::RNGUniformInt(_seedRun,_gymConfig._minWarmup,_gymConfig._minWarmup);
    Engine::RNGUniformInt _uniLose = Engine::RNGUniformInt(_seedRun,_gymConfig._minLose,_gymConfig._maxLose);
    Engine::RNGUniformInt _uniCooldown = Engine::RNGUniformInt(_seedRun,_gymConfig._minCooldown,_gymConfig._maxCooldown);
    Engine::RNGUniformInt _uniDirected = Engine::RNGUniformInt(_seedRun,_gymConfig._uniDirected,_gymConfig._uniDirected);
    Engine::RNGUniformDouble _uniZeroOne = Engine::RNGUniformDouble(_seedRun,0.,1.);
    Engine::RNGUniformDouble _uniMinusOneOne = Engine::RNGUniformDouble(_seedRun,-1.,1.);

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

    Engine::Point2D<int> getClosestDoor(const Engine::Point2D<int>& point);

    int getExerciceTimeFromZone(const int& zone);

    bool atExerciceZone(Engine::Point2D<int> point, const int& zone);

    Engine::Point2D<int> getTargetFromZone(const int& zone);

    void createDistributions();

    std::list<int> createRoutine();

    int currentClassTime();
};

}

#endif