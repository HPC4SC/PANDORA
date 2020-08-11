#include <Cashier.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

#include <iostream>

namespace Examples 
{

Cashier::Cashier(const std::string& id, const int& sick, const int& shift, const int& workedTime, 
        const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius) 
    : Agent(id), _sick(sick), _infected(false), _shift(shift), _workedTime(workedTime), _encounterRadius(encounterRadius) {
        createPhone(phoneT1,phoneT2,phoneApp,signalRadius);
    }

Cashier::~Cashier() {}

void Cashier::selectActions() {}

void Cashier::updateKnowledge() {
    countEncountersReal();
    countEncountersRecorded();
}

bool Cashier::isSick() {
    return _sick;
}

void Cashier::createPhone(const int& threshold1, const int& threshold2, const bool& hasApplication, const int& signalRadius) {
    _phone = new Phone(threshold1,threshold2,hasApplication,signalRadius);
}

void Cashier::countEncountersReal() {}

void Cashier::countEncountersRecorded() {}

}