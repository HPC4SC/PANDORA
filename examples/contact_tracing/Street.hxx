#ifndef __Street_hxx__
#define __Street_hxx__

#include <StreetConfig.hxx>

#include <World.hxx>
#include <Point2D.hxx>
#include <RNGUniformDouble.hxx>

#include <vector>

namespace Examples 
{

class Street : public Engine::World
{

private:

    const StreetConfig& _streetConfig = (const StreetConfig &) getConfig();

    int _walkerId = 0;

    std::vector<Engine::Point2D<int>> _topLimit;

    std::vector<Engine::Point2D<int>> _botLimit;

    bool _initialAgentsCreated = false;

    int _seedRun = _streetConfig.getSeed();

    Engine::RNGUniformDouble _uniformZeroOne = Engine::RNGUniformDouble(_seedRun,0.,1.);

public:
        
    Street(Engine::Config* config, Engine::Scheduler* scheduler = 0);

    ~Street();
	
	void createAgents();

    void createWalker();

    void setupLimits();

    void step();

    double getUniZeroOne();
};

}

#endif