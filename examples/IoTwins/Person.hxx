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

        int _heading;

        int _interest;

        int _interestDecrease;

        std::vector<Engine::Point2D<int>> _visitedInterestPoints; 

        int _timeSpent;

        int _inAglomeration;

    public:

        Person(const std::string& id, const int& vision, const int& velocity, const int& age, const bool& tourist,
               const Engine::Point2D<int>& finalTarget, const Engine::Point2D<int>& target, const int& wallDistance, const int& agentDistance,
               const int& maxDistanceBAgents, const int& provFollow, const int& interest, const int& interestDecrease);

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

        int getHeading();

        void setGroup(const int& group);

        void setTarget(const Engine::Point2D<int>& target);

        void addVisited(Engine::Point2D<int> newPosition);

        void setHeading(const int& heading);

        void initialHeading();

        void correctHeading();

        void turnHeadingLeft();

        void turnHeadingRight();

        bool mustTurnLeft(const int& newHeading);

        bool mustTurnRight(const int& newHeading);

        void printVisited();

        void updateKnowledge();

        bool visitedInterestPoint(const Engine::Point2D<int>& candidate);

        void increaseTimeCount();

        void registerAttributes();

        void serialize();

        int getTimeSpent();

        void checkIfInAglomeration();

        bool targetInSight();

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