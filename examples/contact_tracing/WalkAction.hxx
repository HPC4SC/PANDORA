#ifndef __WalkAction_hxx__
#define __WalkAction_hxx__

#include <Action.hxx>
#include <Walker.hxx>

#include <string>

namespace Engine
{
    class Agent;
}

namespace Examples
{

class WalkAction : public Engine::Action
{

public:
    WalkAction();

    ~WalkAction();

    void execute(Engine::Agent & agent);

    std::string describe() const;

    bool insideWorld(const Engine::Point2D<int>& point, const Walker& walker);

};

}

#endif