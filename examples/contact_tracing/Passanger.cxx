#include <Passanger.hxx>
#include <InfectAction.hxx>
#include <SeatAction.hxx>
#include <RandomMoveAction.hxx>

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
    if (_train->atStop()) {
        if (_exiting) {
            _willSeat = false;
            _targetPosition = _train->findClosestDoor(getPosition());
            setPath();
            _actions.push_back(new SeatAction);
        }
        if (_entering or _willSeat) {
            if (not _willSeat and Engine::GeneralState::statistics().getUniformDistValue() < _move) _actions.push_back(new RandomMoveAction);
            else {
                    if (_targetPath.empty()) {
                        std::vector<Engine::Point2D<int>> avaliableSeats = _train->getAvaliableSeats();
                        if (not avaliableSeats.empty()) {
                            _targetPosition = avaliableSeats[Engine::GeneralState::statistics().getUniformDistValue(0,avaliableSeats.size()-1)];
                            setPath();
                        }
                    }
                    if (not _targetPath.empty()) {
                        _actions.push_back(new SeatAction);
                        if (getPosition() == _targetPosition) _willSeat = false;
                    } 
                    else {
                        _willSeat = false;
                        if (Engine::GeneralState::statistics().getUniformDistValue() < _move) _actions.push_back(new RandomMoveAction);
                    }
            }
        }
    }
}

void Passanger::goingToLeave() {
    _exiting = true;
}

void Passanger::setPath() {
    _targetPath = _train->getShortestPath(getPosition(),_targetPosition);
}

Engine::Point2D<int> Passanger::nextPosition() {
    return _targetPath.front();
}

void Passanger::popCurrentPosition() {
    _targetPath.pop_front();
}

}