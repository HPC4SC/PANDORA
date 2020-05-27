#ifndef __WanderAction_hxx__
#define __WanderAction_hxx__

#include <Action.hxx>
#include <Person.hxx>
#include <World.hxx>
#include <string>

namespace Engine {
    class Person;
}

namespace Examples {

    class WanderAction : public Engine::Action
    {

    public:
        WanderAction();

        virtual ~WanderAction();

        void execute(Engine::Agent &agent);

        Engine::Point2D<int> selectNextPosition(Engine::Agent &agent,Engine::World *world);

        void defineLoopBounds(int &firstI,int &firstJ, int &lastI, int &lastJ, const int &posX, const int &posY,
                                            const int &capability, Engine::World *world);

        int calculateUtility(Engine::Point2D<int> point, Engine::Agent& agent, Engine::World* world);

        int nearPeople(Engine::Agent &agent, Engine::World *world);

        bool nearWall(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world);

        bool nearAgent(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world);

        bool tooFarFromAgent(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world);

        std::string describe() const;
    };

}

#endif