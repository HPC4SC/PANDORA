
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include <Action.hxx>
#include <Bird.hxx>
#include <string>

namespace Engine
{
	class Agent;
}

namespace Examples
{

class MoveAction : public Engine::Action
{
public:
	MoveAction();
	virtual ~MoveAction();
	void execute( Engine::Agent & agent );
	

	std::string describe() const;
	
	//aux methods-------------------------------------------------------
		
	void correctHeading(Bird & birdAgent, const Engine::AgentsVector & flockmates);
	void separate(Bird & birdAgent, const float &nearestHeading);
	void align(Bird & birdAgent, const float &meanHeading);
	void cohere(Bird & birdAgent, const float &meanHeading);
	int distNearestFlockmate(const Engine::Agent &birdAgent, const Engine::AgentsVector &flockmates, float &nearestHeading);
	void advanceForward(Engine::Point2D<int> &newPosition,const int &agentVelocity, Bird & birdAgent);
	int translateHeading(const float &heading);
	float calcMeanHeading(Bird & birdAgent, const Engine::AgentsVector &flockmates);
	float calcMeanTowardsHeading(Bird & birdAgent, const Engine::AgentsVector &flockmates);
	
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

