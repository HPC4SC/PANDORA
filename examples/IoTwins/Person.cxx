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
            _distanceBAgents(maxDistanceBAgents), _provFollow(provFollow), _heading(-1) {}

    Person::~Person() {}

    void Person::selectActions() {
        if (_finalTarget == this->getPosition()) {
            _actions.push_back(new LeaveAction());
        }
        else {
            if (Engine::GeneralState::statistics().getUniformDistValue(0,100) < _provFollow) {
                _actions.push_back(new WanderAction());
            } else {
                if (_heading == -1) initialHeading();
                else correctHeading();
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
        /*switch(newHeading) {
            case 0:
                switch(_heading) {
                    case 0:
                        return false;
                        break;
                    case 1:
                        return true;
                        break;
                    case 2:
                        return true;
                        break;
                    case 3:
                        return false;
                        break;
                    case 4:
                        return true;
                        break;
                    case 5:
                        return false;
                        break;
                    case 6:
                        return false;
                        break;
                    case 7:
                        if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) return true;
                        return false;
                        break;
                }
                break;
            case 1:
                switch(_heading) {
                    case 0:
                        return false;
                        break;
                    case 1:
                        return false;
                        break;
                    case 2:
                        return true;
                        break;
                    case 3:
                        return false;
                        break;
                    case 4:
                        return true;
                        break;
                    case 5:
                        return false;
                        break;
                    case 6:
                        if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) return true;
                        return false;
                        break;
                    case 7:
                        return true;
                        break;
                }
                break;
            case 2:
            switch(_heading) {
                    case 0:
                        return false;
                        break;
                    case 1:
                        return false;
                        break;
                    case 2:
                        return false;
                        break;
                    case 3:
                        return false;
                        break;
                    case 4:
                        return true;
                        break;
                    case 5:
                        if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) return true;
                        return false;
                        break;
                    case 6:
                        return true;
                        break;
                    case 7:
                        return true;
                        break;
                }
                break;
            case 3:
                switch(_heading) {
                    case 0:
                        return true;
                        break;
                    case 1:
                        return true;
                        break;
                    case 2:
                        return true;
                        break;
                    case 3:
                        return false;
                        break;
                    case 4:
                        if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) return true;
                        return false;
                        break;
                    case 5:
                        return false;
                        break;
                    case 6:
                        return false;
                        break;
                    case 7:
                        return false;
                        break;
                }
                break;
            case 4:
                switch(_heading) {
                    case 0:
                        return false;
                        break;
                    case 1:
                        return false;
                        break;
                    case 2:
                        return false;
                        break;
                    case 3:
                        if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) return true;
                        return false;
                        break;
                    case 4:
                        return false;
                        break;
                    case 5:
                        return true;
                        break;
                    case 6:
                        return true;
                        break;
                    case 7:
                        return true;
                        break;
                }
                break;
            case 5:
                switch(_heading) {
                    case 0:
                        return true;
                        break;
                    case 1:
                        return true;
                        break;
                    case 2:
                        if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) return true;
                        return false;
                        break;
                    case 3:
                        return true;
                        break;
                    case 4:
                        return false;
                        break;
                    case 5:
                        return false;
                        break;
                    case 6:
                        return false;
                        break;
                    case 7:
                        return false;
                        break;
                }
                break;
            case 6:
                switch(_heading) {
                    case 0:
                        return true;
                        break;
                    case 1:
                        if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) return true;
                        return false;
                        break;
                    case 2:
                        return false;
                        break;
                    case 3:
                        return true;
                        break;
                    case 4:
                        return false;
                        break;
                    case 5:
                        return true;
                        break;
                    case 6:
                        return false;
                        break;
                    case 7:
                        return false;
                        break;
                }
                break;
            case 7:
                switch(_heading) {
                    case 0:
                        if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) return true;
                        return false;
                        break;
                    case 1:
                        false;
                        break;
                    case 2:
                        return false;
                        break;
                    case 3:
                        return true;
                        break;
                    case 4:
                        return false;
                        break;
                    case 5:
                        return true;
                        break;
                    case 6:
                        return true;
                        break;
                    case 7:
                        return false;
                        break;
                }
                break;
        }*/
    }

    bool Person::mustTurnRight(const int& newHeading) {
        /*switch(newHeading) {
            case 0:
                switch(_heading) {
                    case 0:
                        return false;
                        break;
                    case 1:
                        return true;
                        break;
                    case 2:
                        return true;
                        break;
                    case 3:
                        return false;
                        break;
                    case 4:
                        return true;
                        break;
                    case 5:
                        return false;
                        break;
                    case 6:
                        return false;
                        break;
                    case 7:
                        if (Engine::GeneralState::statistics.getUniformDistValue(0,1) == 0) return true;
                        return false;
                        break;
                }
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
        }*/
    }

    void Person::printVisited() {
        std::cout << "I have visited: ";
        for (int i = 0; i < _visitedPositions.size(); i++) std::cout << _visitedPositions[i] << " ";
        std::cout << std::endl;
    }

}