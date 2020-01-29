#ifndef __LeaveAction_hxx__
#define __LeaveAction_hxx__

#include <Action.hxx>
#include <string>

namespace Engine {
    class Person;
}

namespace Examples {

    class LeaveAction : public Engine::Action
            {

    public:
        LeaveAction();

        virtual ~LeaveAction();

        void execute(Engine::Agent &agent);

        std::string describe() const;
    };

}

#endif