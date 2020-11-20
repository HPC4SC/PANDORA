#include <Customer.hxx>
#include <InfectActionOutside.hxx>
#include <LeaveAction.hxx>
#include <EatAction.hxx>

namespace Examples
{

Customer::Customer(const std::string& id, const double& infectiousness, const int& sick, const int& entryTime,const int& phoneT1, const int& phoneT2, 
        const bool& phoneApp, const int& signalRadius, const int& encounterRadius, Restaurant* restaurant, const int& eatTime, const int& table)
        : HumanBeeing(id,infectiousness,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _restaurant(restaurant), _eatTime(eatTime), _table(table) {}

Customer::~Customer() {}

void Customer::selectActions() {
    _actions.push_back(new InfectActionOutside);
    if (_eatTime == 0) {
        if (_targetPath.empty() and not _restaurant->targetIsDoor(_targetPosition)) {
            _targetPosition = _restaurant->getDoor();
            while (not _restaurant->checkPosition(_targetPosition)) _targetPosition = _restaurant->getDoor();
            calculatePath();
            _actions.push_back(new EatAction);
        }
        if (getPosition() == _targetPosition) {
            _restaurant->setTableFree(_table);
            _actions.push_back(new LeaveAction);
        }
        else _actions.push_back(new EatAction);
    }
    else {
        if (_restaurant->getPositionValue(getPosition()) == _restaurant->getPositionValue(_targetPosition)) _eatTime--;
        else if (not _restaurant->checkPosition(_targetPosition)) {
            _targetPosition = _restaurant->getTargetFromTable(_table);
            while (not _restaurant->checkPosition(_targetPosition)) _targetPosition = _restaurant->getTargetFromTable(_table);
            calculatePath();
            _actions.push_back(new EatAction);
        }
        else if (_targetPath.empty()) {
            _targetPosition = _restaurant->getTargetFromTable(_table);
            while (not _restaurant->checkPosition(_targetPosition)) _targetPosition = _restaurant->getTargetFromTable(_table);
            calculatePath();
            _actions.push_back(new EatAction);
        }
        else _actions.push_back(new EatAction);
    }
}

std::list<Engine::Point2D<int>> Customer::getTargetPath() {
    return _targetPath;
}

Restaurant* Customer::getRestaurant() {
    return _restaurant;
}

void Customer::popTargetPath() {
    if (not _targetPath.empty()) _targetPath.pop_front();
}

void Customer::calculatePath() {
    _targetPath = _restaurant->getShortestPath(getPosition(),_targetPosition);
    if (_targetPath.empty()) _targetPath.push_back(getPosition());
    else if (_targetPath.size() > 1) popTargetPath();
}

}