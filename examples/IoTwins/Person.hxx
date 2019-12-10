#ifndef __Person_hxx__
#define __Person_hxx__

#include <Agent.hxx>
#include <Action.hxx>
#include <string>

namespace Examples
{

    class Person : public Engine::Agent {

    public:

        Person(const std::string &id);

        virtual ~Person();

        void selectActions();

        ////////////////////////////////////////////////
        // This code has been automatically generated //
        /////// Please do not modify it ////////////////
        ////////////////////////////////////////////////
        Person(void *);

        void *fillPackage();

        void sendVectorAttributes(int);

        void receiveVectorAttributes(int);
        ////////////////////////////////////////////////
        //////// End of generated code /////////////////
        ////////////////////////////////////////////////

    };
}

#endif