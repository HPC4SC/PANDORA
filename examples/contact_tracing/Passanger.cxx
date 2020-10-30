#include <Passanger.hxx>
#include <InfectActionOutside.hxx>
#include <SeatAction.hxx>
#include <RandomMoveAction.hxx>
#include <MoveAction.hxx>
#include <LeaveAction.hxx>

#include <GeneralState.hxx>

namespace Examples
{

Passanger::Passanger(const std::string& id, const double& infectiousness, const bool& sick, const int& encounterRadius, const int& phoneT1, 
    const int& phoneT2, const bool& phoneApp, const int& signalRadius, const float& move, const bool& wilSeat, Train* train, const bool& onPlatform) : 
    HumanBeeing(id,infectiousness,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _move(move), _willSeat(wilSeat), _train(train), _onPlatform(onPlatform),
    _exiting(false), _entering(true) {}

Passanger::Passanger(const std::string& id, const double& infectiousness, const bool& sick, const int& encounterRadius, const int& phoneT1, 
    const int& phoneT2, const bool& phoneApp, const int& signalRadius, const float& move, const bool& wilSeat, Train* train, const Engine::Point2D<int>& out) : 
    HumanBeeing(id,infectiousness,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _move(move), _willSeat(wilSeat), _train(train), _targetPosition(out),
    _exiting(true) {
        _onPlatform = true;
    }

Passanger::~Passanger() {}

void Passanger::selectActions() {
    _actions.push_back(new InfectActionOutside);
    if (_train->getAgentsToLeave() > 0) {
        std::cout << getId() << " have to leave" << std::endl;
        goingToLeave();
        _train->agentLeaves();
    }
    if (_onPlatform) {
        if (_entering) {
            Engine::Point2D<int> oldPoint = getPosition();
            if (_train->atStop()) {
                _targetPosition = _train->findClosestDoor(getPosition());
                setPath();
                popCurrentPosition();
                _actions.push_back(new SeatAction);
                if (getPosition() == _targetPosition) _actions.push_back(new LeaveAction);
            }
            else {
                if (_willSeat and _targetPath.empty()) {
                    _targetPosition = _train->findClosestSeat(getPosition());
                    setPath();
                    popCurrentPosition();
                }
                if (_targetPosition != getPosition()) _actions.push_back(new SeatAction);
                else if (not _willSeat) _actions.push_back(new RandomMoveAction);
            }
            if (oldPoint == getPosition()) {
                setPath();
                popCurrentPosition();
            }
        }
        else if (_exiting) {
            Engine::Point2D<int> oldPoint = getPosition();
            if (_targetPath.empty()) {
                setPath();
                popCurrentPosition();
            }
            _actions.push_back(new SeatAction);
            if (oldPoint == getPosition()) {
                setPath();
                popCurrentPosition();
            }
            if (getPosition() == _targetPosition) _actions.push_back(new LeaveAction);
        }
    }
    else {
        if (_train->atStop()) {
            if (_exiting) {
                _targetPosition = _train->findClosestDoor(getPosition());
                setPath();
                popCurrentPosition();
                _actions.push_back(new SeatAction);
                if (getPosition() == _targetPosition) _actions.push_back(new LeaveAction);
            }
            else if (_entering) {
                if (_train->isDoor(getPosition())) {
                    if (_targetPath.empty()) _targetPosition = _train->randomClosePosition(getPosition());
                    setPath();
                    popCurrentPosition();
                    _actions.push_back(new SeatAction);
                }
                else if (not _willSeat and _train->getUniZeroOne() < _move) _actions.push_back(new RandomMoveAction);
                else {
                    if (_targetPath.empty()) {
                        std::vector<Engine::Point2D<int>> avaliableSeats = _train->getAvaliableSeats();
                        if (not avaliableSeats.empty()) {
                            if (_targetPath.empty()) _targetPosition = avaliableSeats[_train->getRandomIndexAvaliableSeats()];
                            setPath();
                            popCurrentPosition();
                        }
                        _actions.push_back(new SeatAction);
                    }
                    if (not _targetPath.empty()) {
                        _actions.push_back(new SeatAction);
                        if (getPosition() == _targetPosition) _willSeat = false;
                    } 
                    /*else {
                        _willSeat = false;
                        //if (_train->getUniZeroOne() < _move) _actions.push_back(new RandomMoveAction);
                    }*/
                }
            }
        }
    }
}

void Passanger::goingToLeave() {
    _exiting = true;
    _entering = false;
}

void Passanger::setPath() {
    _targetPath = _train->getShortestPath(getPosition(),_targetPosition,_exiting);
}

Engine::Point2D<int> Passanger::nextPosition() {
    return _targetPath.front();
}

void Passanger::popCurrentPosition() {
    if (not _targetPath.empty()) _targetPath.pop_front();
}

bool Passanger::getExiting() {
    return _exiting;
}

Train* Passanger::getTrain() {
    return _train;
}

bool Passanger::targetPathSet() {
    return not _targetPath.empty();
}

std::list<Engine::Point2D<int>> Passanger::getTargetPath() {
    return _targetPath;
}

}