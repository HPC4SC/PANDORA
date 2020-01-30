#ifndef __WanderAction_hxx__
#define __WanderAction_hxx__

#include <Action.hxx>
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

        std::string describe() const;
    };

}

#endif