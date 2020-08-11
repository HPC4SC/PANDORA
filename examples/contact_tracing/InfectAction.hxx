#ifndef __InfectAction_hxx__
#define __InfectAction_hxx__

#include <Action.hxx>

#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples 
{

class InfectAction : public Engine::Action {
    public:

        InfectAction();

        ~InfectAction();

        void execute(Engine::Agent & agent);

        std::string describe() const;
};

}

#endif