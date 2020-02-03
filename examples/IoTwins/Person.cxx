#include <Person.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <MoveAction.hxx>
#include <DoNothingAction.hxx>
#include <LeaveAction.hxx>
#include <WanderAction.hxx>

namespace Examples {

    Person::Person(const std::string &id, const int &vision, const int &velocity, const int &age, const bool &tourist,
                   const Engine::Point2D<int> finalTarget, const int &wallDistance, const int &agentDistance,
                   const int &maxDistanceBAgents, const int &provFollow)
            : Agent(id), _vision(vision), _velocity(velocity), _age(age), _isTourist(tourist),
            _finalTarget(finalTarget), _wallDistance(wallDistance),_agentDistance(agentDistance),
            _distanceBAgents(maxDistanceBAgents), _provFollow(provFollow) {}

    Person::~Person() {}

    void Person::selectActions() {
        if(_finalTarget == this->getPosition()) {
            _actions.push_back(new LeaveAction());
        }
        else {
            if(Engine::GeneralState::statistics().getUniformDistValue(0,100) < _provFollow) {
                _actions.push_back(new WanderAction());
            } else {
                _actions.push_back(new MoveAction());
            }
        }
    }

    int Person::getVision() {
        return _vision;
    }

    int Person::getVelocity() {
        return _velocity;
    }

    int Person::getAge() {
        return _age;
    }

    bool Person::isTourist() {
        return _isTourist;
    }

    bool Person::isInGroup() {
        return _isInGroup;
    }

    int Person::getGroup() {
        return _group;
    }

    Engine::Point2D<int> Person::getFinalTarget() {
        return _finalTarget;
    }

    Engine::Point2D<int> Person::getTarget() {
        return _target;
    }

    int Person::getWallDistance() {
        return _wallDistance;
    }

    int Person::getAgentDistance() {
        return _agentDistance;
    }

    int Person::getMaxDistanceBetweenAgents() {
        return _distanceBAgents;
    }

    bool Person::haveVisited(Engine::Point2D<int> newPosition) {
        for (int i = 0; i < _visitedPositions.size(); i++) {
            if(_visitedPositions[i] == newPosition) return true;
        }
        return false;
    }

    void Person::setGroup(const int &group) {
        _group = group;
    }

    void Person::setTarget(const Engine::Point2D<int> &target) {
        _target = target;
    }

    void Person::addVisited(Engine::Point2D<int> newPosition) {
        if(_visitedPositions.size() < 20) _visitedPositions.push_back(newPosition);
        else {
            for (int i = 0; i < _visitedPositions.size()-1; i++) _visitedPositions[i] = _visitedPositions[i+1];
            _visitedPositions[_visitedPositions.size()-1] = newPosition;
        }
    }

}