#ifndef __InfectActionOutside_hxx__
#define __InfectActionOutside_hxx__

#include <HumanBeeing.hxx>

#include <Action.hxx>
#include <Point2D.hxx>

#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples 
{

class InfectActionOutside : public Engine::Action {
    public:

        InfectActionOutside();

        ~InfectActionOutside();

        void execute(Engine::Agent & agent);

        std::string describe() const;
};

}

#endif