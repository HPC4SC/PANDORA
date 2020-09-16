#include <Passanger.hxx>
#include <InfectAction.hxx>

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

    }
}

void Passanger::goingToLeave() {
    _exiting = true;
}

}