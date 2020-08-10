#include <Cashier.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

#include <iostream>

namespace Examples 
{

Cashier::Cashier(const std::string &id, const int &sick) : Agent(id), _sick(sick), _infected(false) {}

Cashier::~Cashier() {}

void Cashier::selectActions() {}

}