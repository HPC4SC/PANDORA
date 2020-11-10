#ifndef __EatAction_hxx__
#define __EatAction_hxx__

#include <Action.hxx>

#include <Customer.hxx>
#include <Restaurant.hxx>

#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples 
{

class EatAction : public Engine::Action {
public:
    EatAction();

    ~EatAction();

    void execute(Engine::Agent & agent);
    
    std::string describe() const;
};

}

#endif