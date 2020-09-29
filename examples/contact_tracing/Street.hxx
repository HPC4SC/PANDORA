#ifndef __Street_hxx__
#define __Street_hxx__

#include <StreetConfig.hxx>

#include <World.hxx>
#include <Point2D.hxx>

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

public:
        
    Street(Engine::Config* config, Engine::Scheduler* scheduler = 0);

    ~Street();
	
	void createAgents();

    void createWalker();

    void setupLimits();

    void step();
};

}

#endif