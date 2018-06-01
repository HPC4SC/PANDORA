
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
	//crea una move action que s'enviarà al scheduler
	MoveAction();
	//destructora per defecte	
	virtual ~MoveAction();
	//descriu l'accio que s'executa
	void execute( Engine::Agent & agent );
	//descriu el nom de l'accio
	std::string describe() const;
	
	//aux methods-------------------------------------------------------
	
	//ajusta el heading de l'agent al dels seus flockmates
	void correctHeading(Bird & birdAgent, const Engine::AgentsVector & flockmates);
	//corregeix el heading d'un agent que en te un altre de massa a prop
	void separate(Bird & birdAgent, const float &nearestHeading);
	//corregeix el heading d'un agent cap a la mitjana de heading dels 
	//altres agents del seu flock
	void align(Bird & birdAgent, const float &meanHeading);
	//corregeix el heading d'un agent cap a la mitjana de heading dels
	//altres agents del flock cap a l'agent en questio
	void cohere(Bird & birdAgent, const float &meanHeading);
	//calcula la distancia entre l'agent i l'agent mes proper
	int distNearestFlockmate(Bird &birdAgent, const Engine::AgentsVector &flockmates, float &nearestHeading);
	//mou l'agent velocity caseles cap al seu heading
	void advanceForward(Engine::Point2D<int> &newPosition,const int &agentVelocity, Bird & birdAgent);
	//metode auxliliar per saber cap a on apunta el heading
	int translateHeading(const float &heading);
	//calcula la mitjana de heading delsaltres agents del seu flock
	float calcMeanHeading (Bird &birdAgent, const Engine::AgentsVector &flockmates);
	//calcula la mitjana de heading delsaltres agents del seu flock cap a l'agent en questio
	float calcMeanTowardsHeading(Bird & birdAgent, const Engine::AgentsVector &flockmates);
	//s'allunya del heading
	void turnAway(const float &meanHeading,const float &max_a_turn, Bird & birdAgent);
	//s'apropa el heading
	void turnTowards(const float &meanHeading,const float &max_a_turn, Bird & birdAgent);
	//acota el maxim gir
	void turnAtMost(const float &turn, const float &max_turn, Bird & birdAgent);
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

