#ifndef __MoveAction_hxx__
#define __MoveAction_hxx__

#include <Action.hxx>
#include <World.hxx>
#include <Person.hxx>
#include <string>

namespace Engine {
    class Human;
}

namespace Examples {

    class MoveAction : public Engine::Action {

        Engine::Point2D<int> selectNextPositon(Engine::Agent &agent, Engine::World *world);

        std::vector<std::pair<Engine::Point2D<int>, int>> calculateDistances(Engine::Agent &agent, Engine::World *world);

        void defineLoopBounds(int &firstI,int &firstJ, int &lastI, int &lastJ, const int &posX, const int &posY,
                          const int &velocity, Engine::World *world);

    public:
        MoveAction();

        virtual ~MoveAction();

        void execute(Engine::Agent &agent);

        std::string describe() const;
    };

} // namespace Examples

#endif // __MoveHomeAction_hxx__
