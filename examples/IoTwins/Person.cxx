#include <Person.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <MoveAction.hxx>
#include <DoNothingAction.hxx>

namespace Examples {

    Person::Person(const std::string &id, const int &vision, const int &velocity, const int &age, const bool &tourist,
                   const Engine::Point2D<int> finalTraget, const int &wallDistance, const int &agentDistance)
            : Agent(id), _vision(vision), _velocity(velocity), _age(age), _isTourist(tourist), _finalTarget(finalTraget),
            _wallDistance(wallDistance), _agentDistance(agentDistance) {
    }

    Person::~Person() {}

    void Person::selectActions() {
        if(_finalTarget == this->getPosition()) {
            _actions.push_back(new DoNothingAction());
        } else {
            _actions.push_back(new MoveAction());
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

    void Person::setGroup(const int &group) {
        _group = group;
    }

    void Person::setTarget(const Engine::Point2D<int> &target) {
        _target = target;
    }

}