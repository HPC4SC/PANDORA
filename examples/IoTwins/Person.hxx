#ifndef __Person_hxx__
#define __Person_hxx__

#include <Agent.hxx>
#include <Action.hxx>
#include <Point2D.hxx>
#include <string>

namespace Examples
{

    class Person : public Engine::Agent
            {

        int _vision;

        int _age;

        bool _isTourist;

        bool _isInGroup;

        int _group;

        Engine::Point2D<int> _finalTarget;

        Engine::Point2D<int> _target;

    public:

        Person(const std::string &id, const int &vision, const int &age, const bool &tourist,
               const Engine::Point2D<int> finalTarget);

        virtual ~Person();

        void selectActions();

        int getVision();

        int getAge();

        bool isTourist();

        bool isInGroup();

        int getGroup();

        Engine::Point2D<int> getFinalTarget();

        Engine::Point2D<int> getTarget();

        void setVision(const int &vision);

        void setAge(const int &age);

        void setGroup(const int &group);

        void setTarget(const Engine::Point2D<int> &target);

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