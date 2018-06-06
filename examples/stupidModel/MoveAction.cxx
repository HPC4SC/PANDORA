
#include <MoveAction.hxx>

#include <Bug.hxx>
#include <GeneralState.hxx>
#include <Rectangle.hxx>

namespace Examples {

MoveAction::MoveAction() {}

MoveAction::~MoveAction() {}

void MoveAction::execute( Engine::Agent & agent ) {	
	if(agent.exists()) {
		Engine::World * world = agent.getWorld();
		Engine::Point2D<int> newPosition = agent.getPosition();
		int new_x = newPosition._x;
		int new_y = newPosition._y;
		MoveBestPos(new_x,new_y,world);
		newPosition._x = new_x;
		newPosition._y = new_y;
		
		if(world->checkPosition(newPosition)) {
			agent.setPosition(newPosition);
		}
	}
}

void MoveAction::MoveBestPos(int &new_x,int &new_y, Engine::World * world) {
	Engine::Point2D<int> candidate;
	candidate._x = new_x;
	candidate._y = new_y;
	int maxFood = world->getValue("food",candidate);
	for(int i = new_x - 4; i < new_x + 4; ++i) {
		for(int j = new_y - 4; j < new_y + 4; ++j) {
			if(inside(i,j,world)){
				candidate._x = i;
				candidate._y = j;
				int candidateFood = world->getValue("food",candidate);
				if (maxFood < candidateFood) {
					maxFood = candidateFood;
					new_x = i;
					new_y = j;
				}
				else  if(maxFood == candidateFood and Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) {
					maxFood = candidateFood;
					new_x = i;
					new_y = j;
				}
			}
		}
	}
}

bool MoveAction::inside(int i, int j, Engine::World * world) {
	Engine::Rectangle<int> r = world->getBoundaries();
	return ((j >= r.top() and j <= r.bottom()) and (i >= r.left() and i <= r.right()));
}

std::string MoveAction::describe() const {
	return "MoveAction";
}

} // namespace Examples

