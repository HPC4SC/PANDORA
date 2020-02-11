#ifndef __Person_hxx__
#define __Person_hxx__

#include <Agent.hxx>
#include <Action.hxx>
#include <Point2D.hxx>
#include <string>
#include <vector>

namespace Examples {

    class Person : public Engine::Agent
            {

    private:

        int _vision;

        int _velocity;

        int _age;

        bool _isTourist;

        bool _isInGroup;

        int _group;

        Engine::Point2D<int> _finalTarget;

        Engine::Point2D<int> _target;

        int _wallDistance;

        int _agentDistance;

        int _distanceBAgents;

        std::vector<Engine::Point2D<int>> _visitedPositions;

        int _provFollow;

    public:

        Person(const std::string &id, const int &vision, const int &velocity, const int &age, const bool &tourist,
               const Engine::Point2D<int> finalTarget, const int &wallDistance, const int &agentDistance,
               const int &maxDistanceBAgents, const int &provFollow);

        virtual ~Person();

        void selectActions();

        int getVision();

        int getVelocity();

        int getAge();

        bool isTourist();

        bool isInGroup();

        int getGroup();

        Engine::Point2D<int> getFinalTarget();

        Engine::Point2D<int> getTarget();

        int getWallDistance();

        int getAgentDistance();

        int getMaxDistanceBetweenAgents();

        bool haveVisited(Engine::Point2D<int> newPosition);

        void setGroup(const int &group);

        void setTarget(const Engine::Point2D<int> &target);

        void addVisited(Engine::Point2D<int> newPosition);

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