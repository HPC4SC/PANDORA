#ifndef __LeaveAction_hxx__
#define __LeaveAction_hxx__

#include <HumanBeeing.hxx>

#include <Action.hxx>

#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples 
{

class LeaveAction : public Engine::Action {
    public:

        LeaveAction();

        ~LeaveAction();

        void execute(Engine::Agent & agent);

        std::string describe() const;
};

}

#endif