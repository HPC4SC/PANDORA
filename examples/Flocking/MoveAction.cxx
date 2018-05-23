
#include <MoveAction.hxx>
#include <World.hxx>
#include <Bird.hxx>
#include <GeneralState.hxx>

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
	
	Engine::Point2D<int> newPosition = agent.getPosition();
	int modX = Engine::GeneralState::statistics().getUniformDistValue(-1,1); 
	newPosition._x += modX;
	int modY = Engine::GeneralState::statistics().getUniformDistValue(-1,1); //new random position
	newPosition._y += modY;
	/*
	int incX, incY;
	calcInc(incX,incY,id_bird);
	newPosition._x += incX;
	newPosition._y += incY;*/

	if(world->checkPosition(newPosition))
	{
		agent.setPosition(newPosition);
	}
}

std::string MoveAction::describe() const
{
	return "MoveAction";
}
/*
void MoveAction::calcInc(int &incX, int &incY, Bird bird) {
	std::vector<Bird> flockmates();
	findFlockmates(flockmates, bird);
	if (! flockmates.empty()) {
		if (distNearestFlockmate() < bird._mindist) {
			MoveAction::separate(incX,incY);
		}
		else {
			MoveAction::align(incX,incY);
			MoveAction::cohere(incX,incY);
		}
	}
}

void MoveAction::separate(Bird bird) {
	if (bird.heading)
	bird.heading += bird._max_S_turn; // del nearest flockmate
}

void MoveAction::align(int &incX, int &incY) {
	bird.heading += bird._max_A_turn; // del heading mitja
}

void MoveAction::cohere(int &incX, int &incY) {
	bird.heading += bird._max_C_turn; // del heading mitja
}

//aux methods-----------------------------------------------------------+

void findFlockmates(vector<Bird> flockmates, Bird  bird) {
	int sigth = bird.getSigth();
	int i = sigth*-1;
	int j = sigth*-1;
	for (i; i <= sigth; ++i) {
		for (j; j <= sigth; ++j) {
			if (abs(i)+abs(j) <= sigth && (i != bird.x && j != bird.y)) flockamtes.push(getAgent(i,j));   
		}
	}
}

int distNearestFlockmate(vector<Bird> flockmates, Bird bird) {
	Bird nearest = flockmates[0];
	for (int i = 1; i < flockamte.size(); ++i) {
		if (distance(bird,nearest) > distance(bird,flockmates[i])) nearest = flockmates[i];
	}
	return distance(bird,nearest);
}*/

} // namespace Examples

