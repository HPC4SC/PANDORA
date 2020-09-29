
#include <MoveAction.hxx>

#include <GeneralState.hxx>
#include <Rectangle.hxx>

#include <Exception.hxx>

namespace Examples 
{

MoveAction::MoveAction() {}

MoveAction::~MoveAction() {}

void MoveAction::execute( Engine::Agent & agent ) {
	Engine::World * world = agent.getWorld();
	Engine::Point2D<int> newPosition = agent.getPosition();
	int new_x = newPosition._x;
	int new_y = newPosition._y;
	// the bug selects the position in it's reach with the most food
	Bug& bug = (Bug&) agent;
	moveBestPos(new_x,new_y,world, bug);
	newPosition._x = new_x;
	newPosition._y = new_y;
	// the bug moves to the selected position if it isn't occupied
	if(world->checkPosition(newPosition)) {
		agent.setPosition(newPosition);
	}
}

void MoveAction::moveBestPos(int &new_x,int &new_y, Engine::World * world, const Bug& bug) {
	Engine::Point2D<int> candidate;
	candidate._x = new_x;
	candidate._y = new_y;
	int maxFood = 0;
	int maxBugMovement = bug.getMaxMovement();
	int ini_i = new_x - maxBugMovement;
	int last_i = new_x + maxBugMovement;
	int ini_j = new_y - maxBugMovement;
	int last_j = new_y + maxBugMovement;
	// the bug looks around and compares the food in the cells within its reach

// std::string lookedupBugID = "Bug_5";
// if (world->getCurrentStep() == 0 and bug.getId() == lookedupBugID)
// {
// 	std::cout << CreateStringStream("[Process # " << world->getId() <<  "] Agent:" << bug.getId() << " BEFORE ; " << " at step: " << world->getCurrentStep() << "\n").str();
// 	new_x = 7;
// 	new_y = 14;
// }
// else if (world->getCurrentStep() == 1 and bug.getId() == lookedupBugID)
// {
// 	std::cout << CreateStringStream("[Process # " << world->getId() <<  "] Agent:" << bug.getId() << " BEFORE ; " << " at step: " << world->getCurrentStep() << "\n").str();
// 	new_x = 6;
// 	new_y = 16;
// }
// else
// {

// int node = 3;
// if (world->getId() == node and bug.getId() == lookedupBugID)
// {
// 	int originalFood = world->getValue("food", bug.getPosition());
// 	std::cout << CreateStringStream("[Process # " << world->getId() <<  "] " << bug.getId() << " position: " << bug.getPosition() << " original food: " << originalFood << " \n").str();
// }
	for(int i = ini_i; i <= last_i; ++i) {
		for(int j = ini_j; j <= last_j; ++j) {
			if(inside(i,j,world)){
				candidate._x = i;
				candidate._y = j;
				int candidateFood = world->getValue("food",candidate);
// if (world->getId() == node and bug.getId() == lookedupBugID)
// {
// 	std::cout << CreateStringStream("[Process # " << world->getId() <<  "] " << bug.getId() << " candidate x=" << candidate._x << " y=" << candidate._y << " where food is: " << candidateFood << " \n").str();
// }
			    // the bug checks if the candidate is a better position
				if (maxFood < candidateFood) {
					maxFood = candidateFood;
					new_x = i;
					new_y = j;
				}
				// if botw cells are equaly good the agent choses randomly with a 50% chance each
				else  if(maxFood == candidateFood and Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) {
					new_x = i;
					new_y = j;
				}
			}
		}
	}
}
//}

bool MoveAction::inside(int i, int j, Engine::World * world) {
	int totalWidth = world->getConfig().getSize().getWidth();
	int totalHeight = world->getConfig().getSize().getHeight();
	return (i >= 0 and j >= 0 and i < totalWidth and j < totalHeight);
}

std::string MoveAction::describe() const {
	return "MoveAction";
}

} // namespace Examples

