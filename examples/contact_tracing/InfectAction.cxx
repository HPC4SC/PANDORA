#include <InfectAction.hxx>

#include <Exception.hxx>

namespace Examples 
{

InfectAction::InfectAction() {}

InfectAction::~InfectAction() {}

void InfectAction::execute(Engine::Agent & agent) {
    
}

std::string InfectAction::describe() const {
    return "InfectAction";
}

}