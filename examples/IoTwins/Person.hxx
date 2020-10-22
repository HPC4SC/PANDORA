#ifndef __Person_hxx__
#define __Person_hxx__

#include <EspaiBarca.hxx>

#include <Agent.hxx>
#include <Action.hxx>
#include <Point2D.hxx>
#include <string>
#include <vector>

namespace Examples {

    class Person : public Engine::Agent
            {

    private:

        int _vision; //MpiBasicAttribute
        int _velocity; //MpiBasicAttribute
        int _age; //MpiBasicAttribute
        bool _isTourist; //MpiBasicAttribute
        bool _isInGroup; //MpiBasicAttribute
        int _group; //MpiBasicAttribute
        Engine::Point2D<int> _finalTarget;
        Engine::Point2D<int> _target;
        int _wallDistance; //MpiBasicAttribute
        int _agentDistance; //MpiBasicAttribute
        int _distanceBAgents; //MpiBasicAttribute
        std::vector<Engine::Point2D<int>> _visitedPositions;
        int _provFollow; //MpiBasicAttribute
        int _interest; //MpiBasicAttribute
        int _interestDecrease; //MpiBasicAttribute
        std::vector<Engine::Point2D<int>> _visitedInterestPoints; 
        EspaiBarca* _espai;
        int _timeSpent; //MpiBasicAttribute

    public:

        Person(const std::string& id, const int& vision, const int& velocity, const int& age, const bool& tourist,
               const Engine::Point2D<int>& finalTarget, const Engine::Point2D<int>& target, const int& wallDistance, const int& agentDistance,
               const int& maxDistanceBAgents, const int& provFollow, const int& interest, const int& interestDecrease, EspaiBarca* espai);

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

        void setGroup(const int& group);

        void setTarget(const Engine::Point2D<int>& target);

        void setFinalTarget(const Engine::Point2D<int>& finalTarget);

        void addVisited(Engine::Point2D<int> newPosition);

        void printVisited();

        void updateKnowledge();

        bool visitedInterestPoint(const Engine::Point2D<int>& candidate);

        void increaseTimeCount();

        void registerAttributes();

        void serialize();

        int getTimeSpent();

        bool targetInSight();

        EspaiBarca* getEspai();

        ////////////////////////////////////////////////
        // This code has been automatically generated //
        /////// Please do not modify it ////////////////
        ////////////////////////////////////////////////
        Person( void * );
	    void * fillPackage() const;
	    void freePackage(void* package) const;
	    bool hasTheSameAttributes(const Engine::Agent&) const;
	    void sendVectorAttributes(int);
	    void receiveVectorAttributes(int);
        ////////////////////////////////////////////////
        //////// End of generated code /////////////////
        ////////////////////////////////////////////////

    };
}

#endif