#include <Customer.hxx>
#include <InfectActionOutside.hxx>
#include <LeaveAction.hxx>
#include <EatAction.hxx>

namespace Examples
{

Customer::Customer(const std::string& id, const double& infectiousness, const int& sick, const int& entryTime,const int& phoneT1, const int& phoneT2, 
        const bool& phoneApp, const int& signalRadius, const int& encounterRadius, Restaurant* restaurant, const int& eatTime)
        : HumanBeeing(id,infectiousness,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _restaurant(restaurant), _eatTime(eatTime) {}

Customer::~Customer() {}

void Customer::selectActions() {
    _actions.push_back(new InfectActionOutside);
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

void Customer::eat() {
    _eatTime--;
}

}