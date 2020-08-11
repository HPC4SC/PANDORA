#include <MoveAction.hxx>

#include <Exception.hxx>

namespace Examples 
{

MoveAction::MoveAction() {}

MoveAction::~MoveAction() {}

void MoveAction::execute(Engine::Agent & agent) {}

std::string MoveAction::describe() const {
    return "MoveAction";
}

}

