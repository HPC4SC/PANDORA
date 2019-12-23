#ifndef __DoNothingAction_hxx__
#define __DoNothingAction_hxx__

#include <Action.hxx>
#include <string>

namespace Engine {
    class Person;
}

namespace Examples {

    class DoNothingAction : public Engine::Action {

    public:
        DoNothingAction();

        virtual ~DoNothingAction();

        void execute(Engine::Agent &agent);

        std::string describe() const;
    };

}

#endif