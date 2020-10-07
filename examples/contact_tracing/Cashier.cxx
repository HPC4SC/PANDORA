#include <Cashier.hxx>
#include <InfectAction.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

Cashier::Cashier(const std::string& id, const int& sick, const int& shift, const int& workedTime, 
        const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius) 
    : HumanBeeing(id,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _shift(shift), _workedTime(workedTime) {}

Cashier::~Cashier() {}

void Cashier::selectActions() {
    _actions.push_back(new InfectAction());
}

}