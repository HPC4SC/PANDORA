#include <Passanger.hxx>
#include <InfectAction.hxx>
#include <SeatAction.hxx>
#include <RandomMoveAction.hxx>
#include <LeaveAction.hxx>

#include <GeneralState.hxx>

namespace Examples
{

Passanger::Passanger(const std::string& id, const bool& sick, const int& encounterRadius, const int& phoneT1, 
    const int& phoneT2, const bool& phoneApp, const int& signalRadius, const float& move, const bool& wilSeat, Train* train) : 
    HumanBeeing(id,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _move(move), _willSeat(wilSeat), _train(train),
    _exiting(false), _entering(true) {}

Passanger::~Passanger() {}

void Passanger::selectActions() {
    _actions.push_back(new InfectAction);
    if (_train->getAgentsToLeave() > 0) {
        goingToLeave();
        _train->agentLeaves();
    }
    if (_train->atStop()) {
        if (_exiting and getPosition() == _targetPosition) _actions.push_back(new LeaveAction);
        else if (_exiting) {
            _willSeat = false;
            if (not _outDoorSelected){
                _targetPosition = _train->findClosestDoor(getPosition());
                setPath();
                popCurrentPosition();
            }
            _actions.push_back(new SeatAction);
        }
        else if (_entering or _willSeat) {
            if (not _willSeat and _train->getUniZeroOne() < _move) _actions.push_back(new RandomMoveAction);
            else {
                if (_targetPath.empty()) {
                    std::vector<Engine::Point2D<int>> avaliableSeats = _train->getAvaliableSeats();
                    if (not avaliableSeats.empty()) {
                        _targetPosition = avaliableSeats[_train->getRandomIndexAvaliableSeats()];
                        setPath();
                        popCurrentPosition();
                    }
                }
                if (not _targetPath.empty()) {
                    _actions.push_back(new RandomMoveAction);
                    if (getPosition() == _targetPosition) _willSeat = false;
                } 
                else {
                    _willSeat = false;
                    if (_train->getUniZeroOne() < _move) _actions.push_back(new RandomMoveAction);
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

}