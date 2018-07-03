
#include <MoveAction.hxx>


#include <RandomAgent.hxx>
#include <GeneralState.hxx>

namespace Examples
{

MoveAction::MoveAction()
{
}

MoveAction::~MoveAction()
{
}

void MoveAction::execute( Engine::Agent & agent ) {	
	if (agent.exists()) {
		Engine::World * world = agent.getWorld();
		RandomAgent & randomAgent = (RandomAgent&)agent;
		randomAgent.incCurrentAge();
		Engine::Point2D<int> newPosition = agent.getPosition();
		int new_x = newPosition._x;
		int new_y = newPosition._y;
		selectBestPos(new_x,new_y,randomAgent.getVision(),world);
		newPosition._x = new_x;
		newPosition._y = new_y;
		int sugarAvaliable = world->getValue("sugar", newPosition);
		world->setValue("sugar", newPosition, 0);
		int currentWealth = randomAgent.getWealth();
		int metabolicRate = randomAgent.getMetabolicRate();
		randomAgent.setWealth(currentWealth + sugarAvaliable - metabolicRate);
		if(world->checkPosition(newPosition)) {
			agent.setPosition(newPosition);
		}
	}
}

void MoveAction::selectBestPos(int &new_x, int &new_y, const int &vision , Engine::World * world) {
	Engine::Point2D<int> candidate_x, candidate_y, max_x, max_y, ini;
	ini._x = new_x;
	ini._y = new_y;
	int maxSugar_x = 0;
	int maxSugar_y = 0;
	candidate_x._y = new_y;
	int nearest_i = ini._x - vision;
	for (int i = ini._x - vision; i < ini._x + vision; i++) {
		if (inside(i,new_y,world)) {
			candidate_x._x = i;
			int candidateSugar = world->getValue("sugar",candidate_x); 
			if (maxSugar_x < candidateSugar) {
				maxSugar_x = candidateSugar;
				max_x = candidate_x;
				nearest_i = i;
			}
			else if (maxSugar_x == candidateSugar and (abs(i - new_x) < abs(nearest_i - new_x))) {
				max_x = candidate_x;
				nearest_i = i;
			}
			else if (maxSugar_x == candidateSugar and (abs(i - new_x) == abs(nearest_i - new_x)) and Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) {
				max_x = candidate_x;
				nearest_i = i;
			}
		}
	}
	candidate_y._x = new_x;
	int nearest_j = ini._x - vision;
	for (int j = ini._y - vision; j < ini._y + vision; ++j) {
		if (inside(new_x,j,world)) {
			candidate_y._y = j;
			int candidateSugar = world->getValue("sugar",candidate_y); 
			if (maxSugar_y < candidateSugar) {
				maxSugar_y = candidateSugar;
				max_y = candidate_y;
				nearest_j = j;
			}
			else if (maxSugar_y == candidateSugar and (abs(j - new_y) < abs(nearest_j - new_y))) {
				max_y = candidate_y;
				nearest_j = j;
			}
			else if (maxSugar_y == candidateSugar and (abs(j - new_y) == abs(nearest_j - new_y)) and Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) {
				max_y = candidate_y;
				nearest_j = j;
			}
		}
	}
	if (maxSugar_x > maxSugar_y) {
		new_x = max_x._x;
		new_y = max_x._y;
	}
	else if (maxSugar_x < maxSugar_y) {
		new_x = max_y._x;
		new_y = max_y._y;
	}
	else if (ini.distance(max_x) < ini.distance(max_y)) {
		new_x = max_x._x;
		new_y = max_x._y;
	}
	else if (ini.distance(max_x) > ini.distance(max_y)) {
		new_x = max_y._x;
		new_y = max_y._y;
	}
	else {
		if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) {
			new_x = max_x._x;
			new_y = max_x._y;
		}
		else {
			new_x = max_y._x;
			new_y = max_y._y;
		}
	}
}

bool MoveAction::inside(int i, int j, Engine::World * world) {
	Engine::Rectangle<int> r = world->getBoundaries();
	return ((j >= r.top() and j <= r.bottom()) and (i >= r.left() and i <= r.right()));
}

std::string MoveAction::describe() const
{
	return "MoveAction";
}

} // namespace Examples

