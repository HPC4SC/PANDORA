#ifndef __NextPositionAction_hxx__
#define __NextPositionAction_hxx__

#include <Action.hxx>
#include <Athlete.hxx>

#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples 
{

class NextPositionAction : public Engine::Action {
public:
    NextPositionAction();

    ~NextPositionAction();

    void execute(Engine::Agent & agent);
    
    std::string describe() const;
};

}

#endif