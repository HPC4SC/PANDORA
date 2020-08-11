#include <Cashier.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

#include <iostream>

namespace Examples 
{

Cashier::Cashier(const std::string& id, const int& sick, const int& shift, const int& workedTime) 
    : Agent(id), _sick(sick), _infected(false), _shift(shift), _workedTime(workedTime) {}

Cashier::~Cashier() {}

void Cashier::selectActions() {}

bool Cashier::isSick() {
    return _sick;
}

}