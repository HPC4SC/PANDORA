#include <MoveAction.hxx>
#include <Client.hxx>
#include <Supermarket.hxx>

#include <Exception.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>

#include <list>
namespace Examples 
{

MoveAction::MoveAction() {}

MoveAction::~MoveAction() {}

void MoveAction::execute(Engine::Agent & agent) {
	Client& client = (Client&)agent;
	Engine::World* world = agent.getWorld();
	Supermarket* super = client.getSuper();
	if (client.getStopCounter() > 0) client.decreaseStopCounter();
	else {
		if (super->getUniformZeroOne() < client.getStopping()) {
			client.setStopCounter((int)super->getUniformZeroOne()*client.getStopTime());
		}
		else {
			Engine::Point2D<int> newPosition;
			if (client.getMemory().empty()) {
				client.setMemory(client.getSuper()->getShortestPath(client.getPosition(),client.getTargetPosition()));
			}
			Engine::Point2D<int> nextMemoryPosition = client.getMemory().front();
			newPosition = nextMemoryPosition;
			if (not world->checkPosition(newPosition) and super->getUniformZeroOne() < client.getWander()) {
				newPosition = client.getPosition();
				int modX = super->getUniformMinusOneOne();
				int modY = super->getUniformMinusOneOne();
				newPosition._x += modX;
				newPosition._y += modY;
				int count = 0;
				while (not world->checkPosition(newPosition) or world->getStaticRaster("layout").getValue(newPosition) == 0) {
					newPosition = client.getPosition();
					modX = super->getUniformMinusOneOne();
					modY = super->getUniformMinusOneOne();
					newPosition._x += modX;
					newPosition._y += modY;
					count++;
					if (count > 8) break;
				}
				client.setMemory(client.getSuper()->getShortestPath(client.getPosition(),client.getTargetPosition()));
			}
			if (world->checkPosition(newPosition)) {
				if (agent.getPosition().distance(newPosition) < 2) client.popFrontMemory();
				agent.setPosition(newPosition);
			}
		}
	}
}

std::string MoveAction::describe() const {
    return "MoveAction";
}

}

