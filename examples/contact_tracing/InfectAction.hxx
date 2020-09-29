#ifndef __InfectAction_hxx__
#define __InfectAction_hxx__

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

class InfectAction : public Engine::Action {
    public:

        InfectAction();

        ~InfectAction();

        void execute(Engine::Agent & agent);

        bool barrier(const HumanBeeing& person, const HumanBeeing* other, const Engine::World* world);

        std::vector<Engine::Point2D<int>> getShortestPath(const Engine::Point2D<int> p1, const Engine::Point2D<int> p2);

        std::string describe() const;
};

}

#endif