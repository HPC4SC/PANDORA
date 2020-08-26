#include <MoveAction.hxx>
#include <Client.hxx>

#include <Exception.hxx>
#include <GeneralState.hxx>

#include <list>

namespace Examples 
{

MoveAction::MoveAction() {}

MoveAction::~MoveAction() {}

void MoveAction::execute(Engine::Agent & agent) {
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
			if (not world->checkPosition(newPosition) and Engine::GeneralState::statistics().getUniformDistValue() < client.getWander()) {
				Engine::Point2D<int> aux = newPosition;
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
					if (not world->checkPosition(newPosition) or world->getStaticRaster("layout").getValue(newPosition) == 0) newPosition = aux;
					if (count > 9) break;
				}
			}
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

