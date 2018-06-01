
#include <MoveAction.hxx>
#include <World.hxx>
#include <GeneralState.hxx>
#include <Agent.hxx>

#include <iostream>
#include <cmath>
#include <string>

namespace Examples
{

MoveAction::MoveAction()
{
}

MoveAction::~MoveAction()
{
}

void MoveAction::execute( Engine::Agent & agent )
{	
	Engine::World * world = agent.getWorld();
	Bird & birdAgent = (Bird&)agent;
	
	//mirar
	std::string ID = agent.getId();
	Engine::Agent * p_agent = agent.getWorld()-> getAgent(ID);
	Engine::AgentsVector flockmates = agent.getWorld()->getNeighbours(p_agent,birdAgent.getSigth(),"Bird");
	
	//corregir heading
	if (! flockmates.empty()) correctHeading(birdAgent,flockmates);
	
	//avançar velocity
	Engine::Point2D<int> newPosition = agent.getPosition();
	int agentVelocity = birdAgent.getVelocity();
	advanceForward(newPosition,agentVelocity,birdAgent);

	if(world->checkPosition(newPosition))
	{
		agent.setPosition(newPosition);
	}
}

std::string MoveAction::describe() const
{
	return "MoveAction";
}

//aux methods-----------------------------------------------------------

void MoveAction::correctHeading(Bird & birdAgent, const Engine::AgentsVector & flockmates) {
	float nearestHeading;
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
	switch (translateHeading(heading)){
		case 1: //E
			newPosition._y += agentVelocity;
			break;
			
		case 2: //NE
			newPosition._x -= agentVelocity;
			newPosition._y += agentVelocity;
			break;
			
		case 3: //N
			newPosition._x -= agentVelocity;
			break;
			
		case 4: //NW
			newPosition._x -= agentVelocity;
			newPosition._y -= agentVelocity;
			break;
			
		case 5: //W
			newPosition._y -= agentVelocity;
			break;
			
		case 6: //SW
			newPosition._x += agentVelocity;
			newPosition._y -= agentVelocity;
			break;
			
		case 7: //S
			newPosition._x += agentVelocity;
			break;
			
		case 8: //SE
			newPosition._x += agentVelocity;
			newPosition._y += agentVelocity;
			break;
			
		default:
			break;
	}
}

int MoveAction::translateHeading(const float &heading) {
	if(337.5 < heading && heading <= 22.5) return 1; //E
	else if(22.5 < heading && heading <= 67.5) return 2; //NE
	else if(67.5 < heading && heading <= 112.5) return 3; //N
	else if(112.5 < heading && heading <= 157.5) return 4; //NW
	else if(157.5 < heading && heading <= 202.5) return 5; //W
	else if(202.5 < heading && heading <= 247.5) return 6; //SW
	else if(247.5 < heading && heading <= 292.5) return 7; //S
	else if(292.5 < heading && heading <= 337.5) return 8; //SE
}

float MoveAction::calcMeanHeading(Bird & birdAgent, const Engine::AgentsVector &flockmates) {
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

} // namespace Examples

