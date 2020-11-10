#include <Athlete.hxx>
#include <InfectActionOutside.hxx>
#include <LeaveAction.hxx>
#include <NextPositionAction.hxx>

namespace Examples
{

Athlete::Athlete(const std::string& id, const double& infectiousness, const int& sick, const int& entryTime, 
        const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius,
		Gym* gym, const std::list<int>& exerciceList, const int& workoutType) 
        : HumanBeeing(id,infectiousness,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), 
        _exerciceList(exerciceList), _exerciceTime(0), _gym(gym), _workoutType(workoutType) {}

Athlete::~Athlete() {}

void Athlete::selectActions() {
    _actions.push_back(new InfectActionOutside);
    if (_exerciceList.empty()) {
        std::cout << getId() << " going out" << std::endl;
        if (_gym->isNotDoor(_targetPosition)) {
            _targetPosition = _gym->getDoor(getPosition());
            int count = 0;
            while (not _gym->checkPosition(_targetPosition) and count < 3) {
                _targetPosition = _gym->getDoor(getPosition());
                count++;
            }
            if (count == 3) _targetPosition = _gym->getAlley();
            while (not _gym->checkPosition(_targetPosition)) _targetPosition = _gym->getAlley();
            calculatePath();
        }
        if (getPosition() == _targetPosition) _actions.push_back(new LeaveAction);
        else _actions.push_back(new NextPositionAction);
    }
    else {
        std::cout << getId() << " next exercice" << std::endl;
        if (_exerciceTime == 0) {
            _targetPosition = _gym->getTargetFromZone(_exerciceList.front());
            while (not _gym->checkPosition(_targetPosition)) _targetPosition = _gym->getTargetFromZone(_exerciceList.front());
            calculatePath();            
            _exerciceTime = _gym->getExerciceTimeFromTask(_exerciceList.size(),_workoutType);
        }
        if (getPosition() == _targetPosition) {
            if (_exerciceTime > 0) _exerciceTime--;
            if (_exerciceTime == 0) _exerciceList.pop_front();
        }
        else _actions.push_back(new NextPositionAction);
    }
}

std::list<Engine::Point2D<int>> Athlete::getTargetPath() {
    return _targetPath;
}

Gym* Athlete::getGym() {
    return _gym;
}

void Athlete::popTargetPath() {
    if (not _targetPath.empty()) _targetPath.pop_front();
}

void Athlete::calculatePath() {
    _targetPath = _gym->getShortestPath(getPosition(),_targetPosition);
    if (_targetPath.empty()) _targetPath.push_back(getPosition());
    else if (_targetPath.size() > 1) popTargetPath();
}

}