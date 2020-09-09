#ifndef __Street_hxx__
#define __Street_hxx__

#include <StreetConfig.hxx>

#include <World.hxx>

namespace Examples 
{

class Street : public Engine::World
{

private:

    const StreetConfig &_supermarketConfig = (const StreetConfig &) getConfig();

public:
        
    Street(Engine::Config* config, Engine::Scheduler* scheduler = 0);

    ~Street();
};

}

#endif