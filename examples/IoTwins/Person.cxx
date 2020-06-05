#include <Person.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <MoveAction.hxx>
#include <DoNothingAction.hxx>
#include <LeaveAction.hxx>
#include <WanderAction.hxx>

namespace Examples {

    Person::Person(const std::string& id, const int& vision, const int& velocity, const int& age, const bool& tourist,
                   const Engine::Point2D<int>& finalTarget, const Engine::Point2D<int>& target, const int& wallDistance, const int& agentDistance,
                   const int& maxDistanceBAgents, const int& provFollow, const int& interest, const int& interestDecrease)
            : Agent(id), _vision(vision), _velocity(velocity), _age(age), _isTourist(tourist),
            _finalTarget(finalTarget), _target(target), _wallDistance(wallDistance),_agentDistance(agentDistance),
            _distanceBAgents(maxDistanceBAgents), _provFollow(provFollow), _heading(-1), _interest(interest), _interestDecrease(interestDecrease),
            _timeSpent(0) {}

    Person::~Person() {}

    void Person::selectActions() {
        if (_finalTarget == this->getPosition()) {
            _actions.push_back(new LeaveAction());
        }
        else if (targetInSight() and _interest > 0) {
            if (Engine::GeneralState::statistics().getUniformDistValue(0,100) < 70) _actions.push_back(new WanderAction);
            else _actions.push_back(new DoNothingAction);
            _interest -= _interestDecrease;
            _visitedInterestPoints.push_back(_target);
        }
        else {
            if (Engine::GeneralState::statistics().getUniformDistValue(0,100) < _provFollow) {
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
        //std::cout << "I'm " << _id << " and I my visiteds are: " << std::endl;
        for (int i = 0; i < _visitedPositions.size(); i++) {
            //std::cout << _visitedPositions[i] << " and newPosition is: " << newPosition << std::endl;
            if(_visitedPositions[i].isEqual(newPosition)){
                //std::cout << "I return true" << std::endl;
                return true;
            }
        }
        //std::cout << "I return flase" << std::endl;
        return false;
    }

    int Person::getHeading() {
        return _heading;
    }

    void Person::setGroup(const int &group) {
        _group = group;
    }

    void Person::setTarget(const Engine::Point2D<int> &target) {
        _target = target;
    }

    void Person::addVisited(Engine::Point2D<int> newPosition) {
        if(_visitedPositions.size() < 540) _visitedPositions.push_back(newPosition);
        else {
            for (int i = 0; i < _visitedPositions.size()-1; i++) {
                //std::cout << "before the change visitedPositions[i]: " << _visitedPositions[i] << std::endl;
                _visitedPositions[i] = _visitedPositions[i+1];
                //std::cout << "after the change visitedPositions[i]: " << _visitedPositions[i] << std::endl;
            } 
            _visitedPositions[_visitedPositions.size()-1] = newPosition;
        }
    }

    void Person::setHeading(const int& heading) {
        _heading = heading;
    }

    void Person::initialHeading() {
        /*this->getWorld();
        this->getPosition();
        std::vector<int> distances;
        for (int i = _position._x - 1; i < _position._x + 1; i++) {
            for (int j = _position._y - 1; j < _position._y + 1; j++) {
                Engine::Point2D<int> checkedPosition = Engine::Point2D<int>(i,j);
                if (i != j) distances.push_back(checkedPosition.distance(_finalTarget));
            }
        }
        int initHeading = 0;
        for (int i = 1; i < distances.size(); i++) {
            if (distances[i] <= distances[initHeading]) initHeading = i;
        }
        _heading = initHeading;
        //std::cout << "I'm " << _id << " and I set my initialHeading to: " << _heading << std::endl;*/
    }

    void Person::correctHeading() {
        /*this->getWorld();
        this->getPosition();
        std::vector<int> distances;
        for (int i = _position._x - 1; i < _position._x + 1; ++i) {
            for (int j = _position._y - 1; j < _position._y + 1; ++j) {
                Engine::Point2D<int> checkedPosition = Engine::Point2D<int>(i,j);
                if (i != j) distances.push_back(checkedPosition.distance(_finalTarget));
            }
        }
        int newHeading = 0;
        for (int i = 1; i < distances.size(); i++) {
            if (distances[i] < distances[newHeading]) newHeading = i;
        }
        if (mustTurnLeft(newHeading)) turnHeadingLeft();
        else if(not mustTurnLeft(newHeading) and newHeading != _heading) turnHeadingRight();*/
    }

    void Person::turnHeadingLeft() {
       /* switch(_heading) {
            case 0: 
                _heading = 3;
                break;
            case 1:
                _heading = 0;
                break;
            case 2: 
                _heading = 1;
                break;
            case 3:
                _heading = 5;
                break;
            case 4:
                _heading = 2;
                break;
            case 5:
                _heading = 6;
                break;
            case 6:
                _heading = 7;
                break;  
            case 7:
                _heading = 4;
                break;          
        }*/
    }

    void Person::turnHeadingRight() {
        /*switch(_heading) {
            case 0: 
                _heading = 1;
                break;
            case 1:
                _heading = 2;
                break;
            case 2: 
                _heading = 4;
                break;
            case 3:
                _heading = 0;
                break;
            case 4:
                _heading = 7;
                break;
            case 5:
                _heading = 3;
                break;
            case 6:
                _heading = 5;
                break;
            case 7:
                _heading = 6;
                break;            
        }*/
    }

    bool Person::mustTurnLeft(const int& newHeading) {
        
    }

    bool Person::mustTurnRight(const int& newHeading) {
        
    }

    void Person::printVisited() {
        std::cout << "I have visited: ";
        for (int i = 0; i < _visitedPositions.size(); i++) std::cout << _visitedPositions[i] << " ";
        std::cout << std::endl;
    }

    void Person::updateKnowledge() {
        std::cout << "I'm " << _id << " going to updare my knowledge, my _position is: " << _position << " and my time spent is: " << _timeSpent << std::endl;
        increaseTimeCount();
        checkIfInAglomeration();
        if (_target.isEqual(Engine::Point2D<int>(-1,-1))) { 
            for (int i = _position._x - _vision; i < _position._x + _vision; i++) {
                for (int j = _position._y - _vision; j < _position._y + _vision; j++) {
                    Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
                    if (this->getWorld()->getStaticRaster("targets").getValue(candidate) == 0 and not visitedInterestPoint(candidate)) {
                        _target = candidate;
                    }
                }
            }
        }
        if (_position.isEqual(_target)) _target = Engine::Point2D<int>(-1,-1);
    }

    void Person::registerAttributes() {
        registerIntAttribute("timeSpent");
    }

    void Person::serialize() {
        serializeAttribute("timeSpent", _timeSpent);
    }

    bool Person::visitedInterestPoint(const Engine::Point2D<int>& candidate) {
        for (int i = 0; i < _visitedInterestPoints.size();  i++) {
            if (candidate.isEqual(_visitedInterestPoints[i])) return true;
            if (candidate.distance(_visitedInterestPoints[i]) < 70) return true;
        }
        return false;
    }

    void Person::increaseTimeCount() {
        _timeSpent += 1;
    }

    int Person::getTimeSpent() {
        return _timeSpent;
    }

    void Person::checkIfInAglomeration() {
        
    }

    bool Person::targetInSight() {
        if (_target.distance(this->getPosition()) < _vision) return true;
        return false;
    }

}