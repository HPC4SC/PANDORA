
#include <MoveAction.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <Agent.hxx>
#include <Point2D.hxx>

#include <iostream>
#include <cmath>
#include <string>


#include <omp.h>
#include <thread>
#include <chrono>


namespace Examples {

MoveAction::MoveAction() {}

MoveAction::~MoveAction() {}

void MoveAction::execute( Engine::Agent & agent ) {
	Engine::World * world = agent.getWorld();
	Bird & birdAgent = (Bird&)agent;

	// look around
	Engine::Agent * p_agent = agent.getWorld()->getAgent(agent.getId());
	Engine::AgentsVector flockmates = agent.getWorld()->getNeighbours(p_agent,birdAgent.getSigth(),"Bird");
	// change heading
	if (! flockmates.empty()) correctHeading(birdAgent,flockmates);
	// go forward velocity cells in the heading direction
	Engine::Point2D<int> newPosition = agent.getPosition();
	int agentVelocity = birdAgent.getVelocity();
	advanceForward(newPosition,agentVelocity,birdAgent);

	// checks if the position is occupied
	agent.getWorld()->changingWorld();
	if(world->checkPosition(newPosition)) {
		agent.setPosition(newPosition);
	}
	agent.getWorld()->worldChanged();
}

std::string MoveAction::describe() const {
	return "MoveAction";
}

void MoveAction::correctHeading(Bird & birdAgent, const Engine::AgentsVector & flockmates) {
	float nearestHeading;
	// checks which of the turns must the bird execute
	if (distNearestFlockmate(birdAgent,flockmates,nearestHeading) < birdAgent.getMindist()) {
		MoveAction::separate(birdAgent,nearestHeading);
	}
	else {
		MoveAction::align(birdAgent,calcMeanHeading(birdAgent,flockmates));
		MoveAction::cohere(birdAgent,calcMeanTowardsHeading(birdAgent,flockmates));
	}
}

void MoveAction::separate(Bird & birdAgent, const float &nearestHeading) {
	turnAway(nearestHeading,birdAgent.getMaxSTurn(),birdAgent);
}

void MoveAction::align(Bird & birdAgent, const float &meanHeading) {
	turnTowards(meanHeading,birdAgent.getMaxATurn(),birdAgent);
}

void MoveAction::cohere(Bird & birdAgent, const float &meanTowardsHeading) {
	turnTowards(meanTowardsHeading,birdAgent.getMaxCTurn(),birdAgent);
}

int MoveAction::distNearestFlockmate(Bird & birdAgent, const Engine::AgentsVector &flockmates, float &nearestHeading) {
	Bird & nearestAgent = (Bird&)flockmates[0];
	int index_nearest = 0;
	// for loop to identify the nearest flockmate
	for (int i = 1; i < flockmates.size(); ++i) {
		Bird  & candidate = (Bird&)flockmates[i];
		if (birdAgent.getPosition().distance(nearestAgent.getPosition()) > birdAgent.getPosition().distance(candidate.getPosition())) index_nearest = i;
	}
	Bird & res = (Bird&)flockmates[index_nearest];
	nearestHeading = res.getHeading();
	return birdAgent.getPosition().distance(res.getPosition());
}

void MoveAction::advanceForward(Engine::Point2D<int> &newPosition,const int &agentVelocity, Bird & birdAgent) {
	float heading = birdAgent.getHeading();
	Engine::Rectangle<int> r = birdAgent.getWorld()->getBoundaries();
	switch (translateHeading(heading)){
		case 1: // E
			if (inside(newPosition._x,newPosition._y + agentVelocity,r)) {
				newPosition._y += agentVelocity;
			}
			else { // since the sapce is toroidal the bird must warp to the other edge of the world
				Engine::Point2D<int> limit = Engine::Point2D<int>(newPosition._x,r.right());
				int distanceToWarp = newPosition.distance(limit);
				newPosition._y = r.left();
				advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
			}
			break;

		case 2: // NE
			if (inside(newPosition._x - agentVelocity,newPosition._y + agentVelocity,r)) {
				newPosition._x -= agentVelocity;
				newPosition._y += agentVelocity;

			}
			// since the sapce is toroidal the bird must warp to the other edge of the world
			else {
				if (((newPosition._x - agentVelocity) < r.top()) and ((newPosition._y + agentVelocity) > r.right())) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(r.top(),r.right());
					int distanceToWarp = newPosition.distance(limit);
					newPosition._x = r.bottom();
					newPosition._y = r.left();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
				else if ((newPosition._x - agentVelocity) < r.top()) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(r.top(),newPosition._y);
					int distanceToWarp = newPosition.distance(limit);
					newPosition._x = r.bottom();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
				else if ((newPosition._y + agentVelocity) > r.right()) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(newPosition._x,r.right());
					int distanceToWarp = newPosition.distance(limit);
					newPosition._y = r.left();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
			}
			break;

		case 3: // N
			if (inside(newPosition._x - agentVelocity,newPosition._y,r)) {
				newPosition._x -= agentVelocity;
			}
			else { // since the sapce is toroidal the bird must warp to the other edge of the world
				Engine::Point2D<int> limit = Engine::Point2D<int>(r.top(),newPosition._y);
				int distanceToWarp = newPosition.distance(limit);
				newPosition._x = r.bottom();
				advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
			}
			break;

		case 4: // NW
			if (inside(newPosition._x - agentVelocity,newPosition._y - agentVelocity,r)) {
				newPosition._x -= agentVelocity;
				newPosition._y -= agentVelocity;
			}
			else { // since the sapce is toroidal the bird must warp to the other edge of the world
				if (((newPosition._x - agentVelocity) < r.top()) and ((newPosition._y - agentVelocity) < r.left())) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(r.top(),r.left());
					int distanceToWarp = newPosition.distance(limit);
					newPosition._x = r.bottom();
					newPosition._y = r.right();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
				else if ((newPosition._x - agentVelocity) < r.top()) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(r.top(),newPosition._y);
					int distanceToWarp = newPosition.distance(limit);
					newPosition._x = r.bottom();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
				else if ((newPosition._y - agentVelocity) < r.left()) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(newPosition._x,r.left());
					int distanceToWarp = newPosition.distance(limit);
					newPosition._y = r.right();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
			}
			break;

		case 5: // W
			if (inside(newPosition._x,newPosition._y - agentVelocity,r)) {
				newPosition._y -= agentVelocity;
			}
			else { // since the sapce is toroidal the bird must warp to the other edge of the world
				Engine::Point2D<int> limit = Engine::Point2D<int>(newPosition._x,r.left());
				int distanceToWarp = newPosition.distance(limit);
				newPosition._y = r.right();
				advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
			}
			break;

		case 6: // SW
			if (inside(newPosition._x + agentVelocity,newPosition._y - agentVelocity,r)) {
				newPosition._x += agentVelocity;
				newPosition._y -= agentVelocity;
			}
			else { // since the sapce is toroidal the bird must warp to the other edge of the world
				if (((newPosition._x + agentVelocity) > r.bottom()) and ((newPosition._y - agentVelocity) < r.left())) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(r.bottom(),r.left());
					int distanceToWarp = newPosition.distance(limit);
					newPosition._x = r.top();
					newPosition._y = r.right();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
				else if ((newPosition._x + agentVelocity) > r.bottom()) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(r.bottom(),newPosition._y);
					int distanceToWarp = newPosition.distance(limit);
					newPosition._x = r.top();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
				else if ((newPosition._y - agentVelocity) < r.left()) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(newPosition._x,r.left());
					int distanceToWarp = newPosition.distance(limit);
					newPosition._y = r.right();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
			}
			break;

		case 7: // S
			if (inside(newPosition._x + agentVelocity,newPosition._y,r)) {
				newPosition._x += agentVelocity;
			}
			else { // since the sapce is toroidal the bird must warp to the other edge of the world
				Engine::Point2D<int> limit = Engine::Point2D<int>(r.bottom(),newPosition._y);
				int distanceToWarp = newPosition.distance(limit);
				newPosition._x = r.top();
				advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
			}
			break;

		case 8: // SE
			if (inside(newPosition._x + agentVelocity,newPosition._y + agentVelocity,r)) {
				newPosition._x += agentVelocity;
				newPosition._y += agentVelocity;
			}
			else { // since the sapce is toroidal the bird must warp to the other edge of the world
				if (((newPosition._x + agentVelocity) > r.bottom()) and ((newPosition._y + agentVelocity) > r.right())) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(r.bottom(),r.right());
					int distanceToWarp = newPosition.distance(limit);
					newPosition._x = r.top();
					newPosition._y = r.left();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
				else if ((newPosition._x + agentVelocity) > r.bottom()) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(r.bottom(),newPosition._y);
					int distanceToWarp = newPosition.distance(limit);
					newPosition._x = r.top();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
				else if ((newPosition._y + agentVelocity) > r.right()) {
					Engine::Point2D<int> limit = Engine::Point2D<int>(newPosition._x,r.right());
					int distanceToWarp = newPosition.distance(limit);
					newPosition._y = r.left();
					advanceForward(newPosition,agentVelocity - distanceToWarp,birdAgent);
				}
			}
			break;

		default:
			break;
	}
}

int MoveAction::translateHeading(const float &heading) {
	/*
	 *  this coddes are used to identify the heading for the
	 * switch structure in the advanceForward method
	 */
	if(337.5 < heading && heading <= 22.5) return 1; // E
	else if(22.5 < heading && heading <= 67.5) return 2; // NE
	else if(67.5 < heading && heading <= 112.5) return 3; // N
	else if(112.5 < heading && heading <= 157.5) return 4; // NW
	else if(157.5 < heading && heading <= 202.5) return 5; // W
	else if(202.5 < heading && heading <= 247.5) return 6; // SW
	else if(247.5 < heading && heading <= 292.5) return 7; // S
	else return 8; // SE
}

float MoveAction::calcMeanHeading(Bird & birdAgent, const Engine::AgentsVector &flockmates) {
	// calculates the mean heading of all the bird's flockmates
	int x_comp = 0;
	int y_comp = 0;
	for (int i = 0; i < flockmates.size(); ++i) {
		Bird & flockmate = (Bird&)flockmates[i];
		x_comp += sin(flockmate.getHeading());
		y_comp += cos(flockmate.getHeading());
	}
	if (x_comp == 0 && y_comp == 0) return birdAgent.getHeading();
	else return (float)atan2(x_comp,y_comp);
}

float MoveAction::calcMeanTowardsHeading(Bird & birdAgent, const Engine::AgentsVector &flockmates) {
	// calculates the mean heading of the flockmates that go towards the bird
	int x_comp = 0;
	int y_comp = 0;
	for (int i = 0; i < flockmates.size(); ++i) {
		Bird & flockmate = (Bird&)flockmates[i];
		x_comp += sin(flockmate.getHeading() + 180);
		y_comp += cos(flockmate.getHeading() + 180);
	}
	x_comp /= flockmates.size();
	y_comp /= flockmates.size();
	if (x_comp == 0 && y_comp == 0) return birdAgent.getHeading();
	else return (float)atan2(x_comp,y_comp);
}

void MoveAction::turnAway(const float &meanHeading,const float &max_turn, Bird & birdAgent) {
	turnAtMost(birdAgent.getHeading() - meanHeading,max_turn,birdAgent);
}

void MoveAction::turnTowards(const float &meanHeading,const float &max_turn, Bird & birdAgent) {
	turnAtMost(meanHeading - birdAgent.getHeading(),max_turn,birdAgent);
}

void MoveAction::turnAtMost(const float &turn, const float &max_turn, Bird & birdAgent) {
	if (abs(turn) > abs(max_turn)) {
		if (turn > 0) {
			birdAgent.setHeading(birdAgent.getHeading() + max_turn);
		}
		else {
			birdAgent.setHeading(birdAgent.getHeading() - max_turn);
		}
	}
	else {
		birdAgent.setHeading(birdAgent.getHeading() + turn);
	}
}

bool MoveAction::inside(int i, int j, Engine::Rectangle<int> r) {
	return ((j >= r.top() and j <= r.bottom()) and (i >= r.left() and i <= r.right()));
}

} // namespace Examples

