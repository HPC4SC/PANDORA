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
    const int& phoneT2, const bool& phoneApp, const int& signalRadius, const float& move, const bool& wilSeat, Train* train) : 
    HumanBeeing(id,infectiousness,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _move(move), _willSeat(wilSeat), _train(train),
    _exiting(false), _entering(true) {}

Passanger::~Passanger() {}

void Passanger::selectActions() {
    _actions.push_back(new InfectActionOutside);
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
        else if (_entering) {
            double draw = _train->getUniZeroOne();
            if (_train->isDoor(getPosition())) {
                _targetPosition = _train->randomClosePosition(getPosition());
                setPath();
                popCurrentPosition();
                _actions.push_back(new SeatAction);
            }
            else if (not _willSeat and draw < _move and _targetPath.empty()) _actions.push_back(new SeatAction);
            else {
                if (_targetPath.empty()) {
                    std::vector<Engine::Point2D<int>> avaliableSeats = _train->getAvaliableSeats();
                    if (not avaliableSeats.empty()) {
                        _targetPosition = avaliableSeats[_train->getRandomIndexAvaliableSeats()];
                        setPath();
                        popCurrentPosition();
                    }
                    _actions.push_back(new SeatAction);
                }
                if (not _targetPath.empty()) {
                    _actions.push_back(new SeatAction);
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
    //std::cout << getId() << " target path after set is the following one: " << std::endl;
    //for (std::list<Engine::Point2D<int>>::iterator it = _targetPath.begin(); it != _targetPath.end(); it++) std::cout << *it << ' ';
    //std::cout << std::endl;
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