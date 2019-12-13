#include <Person.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <MoveAction.hxx>

namespace Examples {

    Person::Person(const std::string &id) : Agent(id) {}

    Person::~Person() {}

    void Person::selectActions() {
        _actions.push_back(new MoveAction());
    }

    int Person::getVision() {
        return _vision;
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

    void Person::setVision(const int &vision) {
        _vision = vision;
    }

    void Person::setAge(const int &age) {
        _age = age;
    }

    void Person::setGroup(const int &group) {
        _group = group;
    }

    void Person::setTarget(const Engine::Point2D<int> &target) {
        _target = target;
    }

}