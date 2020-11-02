#include <Athlete.hxx>
#include <InfectActionOutside.hxx>
#include <LeaveAction.hxx>
#include <nextPositionAction.hxx>

namespace Examples
{

Athlete::Athlete(const std::string& id, const double& infectiousness, const int& sick, const int& entryTime, 
        const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius,
		Gym* gym, const std::list<int>& exerciceList) 
        : HumanBeeing(id,infectiousness,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), 
        _exerciceList(exerciceList), _exerciceTime(0), _gym(gym) {}

Athlete::~Athlete() {}

void Athlete::selectActions() {
    if (_exerciceList.empty()) {
        if (_gym->isNotDoor(_targetPosition)) {
            _targetPosition = _gym->getClosestDoor();
            _targetPath = _gym->getShortestPath();
        }
        if (getPosition() == _targetPosition) _actions.push_back(new LeaveAction);
        else _actions.push_back(new nextPositionAction);
    }
    else {
        _exerciceTime = gym->getExerciceTimeFromZone(_exerciceList.front());
        if (_gym->atExerciceZone(getPosition(),_exerciceList.front()) {
            if (_exerciceTime > 0) _exerciceTime--;
            else _actions.push_back(new nextPositionAction);
        }
        if (_exerciceTime == 0) _exerciceList.pop_front();
    }
    _actions.push_back(new InfectActionOutside);
}

}