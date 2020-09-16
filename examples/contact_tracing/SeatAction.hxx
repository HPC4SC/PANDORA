#ifndef __SeatAction_hxx__
#define __SeatAction_hxx__

#include <Action.hxx>
#include <Passanger.hxx>

#include <Point2D.hxx>

#include <string>

namespace Engine
{
    class Agent;
}

namespace Examples
{
    class SeatAction : public Engine::Action {
        public:
            SeatAction();

            ~SeatAction();

            void execute(Engine::Agent & agent);
    
            std::string describe() const;
    };
}

#endif