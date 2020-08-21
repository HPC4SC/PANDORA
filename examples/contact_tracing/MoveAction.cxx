#include <MoveAction.hxx>
#include <Client.hxx>

#include <Exception.hxx>
#include <GeneralState.hxx>

#include <list>

namespace Examples 
{

MoveAction::MoveAction() {}

MoveAction::~MoveAction() {}

void MoveAction::execute(Engine::Agent & agent) { // TODO fer servir A*
	std::cout << "Move Agents by: " << agent.getId() << std::endl;
	Client& client = (Client&)agent;
	Engine::World* world = agent.getWorld();
	if (client.getStopCounter() > 0) client.decreaseStopCounter();
	else {
		if (Engine::GeneralState::statistics().getUniformDistValue() < client.getStopping()) {
			client.setStopCounter((int)Engine::GeneralState::statistics().getUniformDistValue()*client.getStopTime());
		}
		else {
			Engine::Point2D<int> newPosition;
			if (client.getMemory().empty()) {
				client.setMemory(client.getSuper()->getShortestPath(client.getPosition(),client.getTargetPosition()));
			}
			newPosition = client.getMemory().front();
			client.popFrontMemory();
			if (not world->checkPosition(newPosition) or Engine::GeneralState::statistics().getUniformDistValue() < client.getWander()) {
				newPosition = client.getPosition();
				int modX = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
				int modY = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
				newPosition._x += modX;
				newPosition._y += modY;
				int count = 0;
				while (not world->checkPosition(newPosition) or world->getStaticRaster("layout").getValue(newPosition) == 0) {
					modX = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
					modY = Engine::GeneralState::statistics().getUniformDistValue(-1,1);
					newPosition._x += modX;
					newPosition._y += modY;
					count++;
					if (count > 9) break;
				}
			}
			std::cout << "I'm " << client.getId() << " and my newPosition is: " << newPosition;
			if (world->checkPosition(newPosition))
			{
				agent.setPosition(newPosition);
			}
		}
	}
}

std::string MoveAction::describe() const {
    return "MoveAction";
}

}

