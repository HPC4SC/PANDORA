#include <EatAction.hxx>

namespace Examples
{

EatAction::EatAction() {}

EatAction::~EatAction() {}

void EatAction::execute(Engine::Agent & agent) {
    std::cout << agent.getId() << " executing EatAction" << std::endl;
    Customer& customer = (Customer&)agent;
    Engine::World* world = agent.getWorld();
     Engine::Point2D<int> nextMemoryPosition = customer.getTargetPath().front();
    if (world->checkPosition(nextMemoryPosition)) {
		customer.popTargetPath();
		agent.setPosition(nextMemoryPosition);
	}
    else {
        customer.calculatePath();
    }
}
    
std::string EatAction::describe() const {
    return "EatAction";
}

}