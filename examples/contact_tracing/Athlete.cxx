#include <Athlete.hxx>
#include <InfectActionOutside.hxx>
#include <LeaveAction.hxx>
#include <NextPositionAction.hxx>

namespace Examples
{

Athlete::Athlete(const std::string& id, const double& infectiousness, const int& sick, const int& entryTime, 
        const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius,
		Gym* gym, const std::list<int>& exerciceList, const int& totalExerciceTime, const bool& directedClass) 
        : HumanBeeing(id,infectiousness,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), 
        _exerciceList(exerciceList), _exerciceTime(0), _gym(gym), _totalExerciceTime(totalExerciceTime), _directedClass(directedClass) {}

Athlete::~Athlete() {}

void Athlete::selectActions() {
    _actions.push_back(new InfectActionOutside);
    if (_exerciceList.empty()) {
        if (_gym->isNotDoor(_targetPosition)) {
            _targetPosition = _gym->getClosestDoor(getPosition());
            _targetPath = _gym->getShortestPath(getPosition(),_targetPosition);
        }
        if (getPosition() == _targetPosition) _actions.push_back(new LeaveAction);
        else _actions.push_back(new NextPositionAction);
    }
    else {
        _exerciceTime = _gym->getExerciceTimeFromZone(_exerciceList.front());
        if (_gym->atExerciceZone(getPosition(),_exerciceList.front())) {
            if (_exerciceTime > 0) _exerciceTime--;
            else _actions.push_back(new NextPositionAction);
        }
        if (_exerciceTime == 0) {
            _exerciceList.pop_front();
            _targetPosition = _gym->getTargetFromZone(_exerciceList.front());
            _targetPath = _gym->getShortestPath(getPosition(),_targetPosition);
        }
    }
}

}