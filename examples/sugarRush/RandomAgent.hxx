
#ifndef __RandomAgent_hxx__
#define __RandomAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples
{

class RandomAgent : public Engine::Agent
{
	int _wealth; // attribute that represents how well the agent is living
	int _vision; // attribute that represents how far the agent can see
	int _metabolicRate; // attribute that represents how much wealth the agent consumes each time step
	int _currentAge; // attribute that represents how old is the agent
	int _maxAge; // attribute that represents the maximum time that the agent can live
	
public:
	// creates an instance of a RandomAgent with the attributes initialized with the parameter's value
	RandomAgent(const std::string & id, const int &wealth, const int &vision, const int &metabolicRate, const int &currentAge, const int &maxAge);
	// destroys an instance of a RandomAgent
	virtual ~RandomAgent();
	// select which actions and in which order the agentg willl execute them
	void selectActions();
	// selects which attributes will be registered to be analyzed afetrwards
	void registerAttributes();
	// updates the value of the registered attributes each time step
	void serialize();
	// returns the value of the _wealth attribute
	int getWealth() const;
	// sets the _wealth attribute to the wealth value
	void setWealth(const int& wealth);
	// returns the value of the _vision attribute
	int getVision() const;
	// returns the value of the _metabolicRate attribute
	int getMetabolicRate() const;
	// returns the value of the _currentAge attribute
	int getCurrentAge() const;
	// incremenrts the value of the _currentAge attribute by 1
	void incCurrentAge();
	// returns the value of the _maxAge attribute
	int getMaxAge() const;

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	RandomAgent( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __RandomAgent_hxx__

