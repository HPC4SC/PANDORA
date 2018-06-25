
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include <Action.hxx>
#include <Bird.hxx>
#include <string>

namespace Engine {
	class Agent;
}

namespace Examples {

class MoveAction : public Engine::Action {
	
public:
	// creates a MoveAction instance
	MoveAction();
	// destroys a MoveAction instance
	virtual ~MoveAction();
	// code that each agent will execute when performing a Move Action
	void execute( Engine::Agent & agent );
	// aux method
	std::string describe() const;
	// changes the heading of the agent depending on its stiuation
	void correctHeading(Bird & birdAgent, const Engine::AgentsVector & flockmates);
	// turns away form the agent in fornt of it
	void separate(Bird & birdAgent, const float &nearestHeading);
	// aligns it's heading witch his flockmates
	void align(Bird & birdAgent, const float &meanHeading);
	// coheres it's heading with all other agents
	void cohere(Bird & birdAgent, const float &meanHeading);
	// retuens the distance with the nearest flockmate
	int distNearestFlockmate(Bird &birdAgent, const Engine::AgentsVector &flockmates, float &nearestHeading);
	// moves the agent _velocity cells into the heading direction
	void advanceForward(Engine::Point2D<int> &newPosition,const int &agentVelocity, Bird & birdAgent);
	// auxiliar function
	int translateHeading(const float &heading);
	// returns the mean heading
	float calcMeanHeading (Bird &birdAgent, const Engine::AgentsVector &flockmates);
	// returns the mean heading towards the agent
	float calcMeanTowardsHeading(Bird & birdAgent, const Engine::AgentsVector &flockmates);
	// modifies the heading of the agent
	void turnAway(const float &meanHeading,const float &max_a_turn, Bird & birdAgent);
	// modifies the heading of the agent
	void turnTowards(const float &meanHeading,const float &max_a_turn, Bird & birdAgent);
	// modifies the heading of the agent
	void turnAtMost(const float &turn, const float &max_turn, Bird & birdAgent);
	// checks if the positoion (i,j) is inside the world
	bool inside(int i, int j, Engine::Rectangle<int> r);
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

