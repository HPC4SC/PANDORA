#include <Client.hxx>

#include <World.hxx>
#include <GeneralState.hxx>

#include <iostream>

namespace Examples
{

Client::Client(const std::string &id, const int &sick) : Agent(id), _sick(sick), _infected(false) {}

Client::~Client(){}

void Client::selectActions(){}

}