#ifndef __MoveAction_hxx__
#define __MoveAction_hxx__

#include <Action.hxx>
#include <World.hxx>
#include <Person.hxx>
#include <string>

namespace Engine {
    class Person;
}

namespace Examples {

    class MoveAction : public Engine::Action
            {

        Engine::Point2D<int> selectNextPosition(Engine::Agent &agent, Engine::World *world, Engine::Point2D<int>& currentTarget);

        std::vector<std::pair<Engine::Point2D<int>, int>> lookAround(Engine::Agent &agent, Engine::World *world, Engine::Point2D<int>& currentTarget);

        int assignPriority(Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world, Engine::Point2D<int>& currentTarget);

        bool nearWall(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world);

        bool nearAgent(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world);

        bool tooFarFromAgent(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world);

        bool targetNearWall(Engine::Agent& agent, Engine::World *world);

        void defineLoopBounds(int &firstI,int &firstJ, int &lastI, int &lastJ, const int &posX, const int &posY,
                          const int &velocity, Engine::World *world);

    public:
        MoveAction();

        virtual ~MoveAction();

        void execute(Engine::Agent &agent);

        std::string describe() const;
    };

}

#endif

