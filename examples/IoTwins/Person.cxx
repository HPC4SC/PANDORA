#include <Person.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <MoveAction.hxx>

namespace Examples {

    Person::Person(const std::string &id, const int &vision, const int &velocity, const int &age, const bool &tourist,
                   const Engine::Point2D<int> finalTraget)
            : Agent(id), _vision(vision), _velocity(velocity), _age(age), _isTourist(tourist), _finalTarget(finalTraget) {
        std::cout << "I'm Person: " << id << std::endl;
        std::cout << "I have a vision of: " << _vision << std::endl;
        std::cout << "I have a velocity of: " << _velocity << std::endl;
        std::cout << "I'm: " << age << std::endl;
        std::cout << "I'm a tourist: " << _isTourist << std::endl;
        std::cout << "My final Target is: (" << _finalTarget._x << "," << _finalTarget._y << ")" << std::endl;
    }

    Person::~Person() {}

    void Person::selectActions() {
        _actions.push_back(new MoveAction());
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

    void Person::setGroup(const int &group) {
        _group = group;
    }

    void Person::setTarget(const Engine::Point2D<int> &target) {
        _target = target;
    }

}