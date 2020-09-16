#ifndef __RandomMoveAction_hxx__
#define __RandomMoveAction_hxx__

#include <Action.hxx>
#include <Agent.hxx>

#include <string>

namespace Engine 
{
	class Agent;
}

namespace Examples
{
    class RandomMoveAction : public Engine::Action {
        public:
            RandomMoveAction();

            ~RandomMoveAction();

            void execute(Engine::Agent & agent);
    
            std::string describe() const;      
    };
}

#endif