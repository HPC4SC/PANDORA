
#ifndef __Bug_hxx__
#define __Bug_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples 
{

class Bug : public Engine::Agent
{
	
	int _size; // MpiBasicAttribute: attribute that represents the size of the Bug agent
	int _maxBugMovement; // MpiBasicAttribute: attribute that represents the maximum movement distance for the agent
	int _maxConsumptionRate; // MpiBasicAttribute: attribute that represents the maximum conspumtion of food of a Bug agent in one step
	int _survivalProbability; // MpiBasicAttribute: attribute that represents the probability of a Bug agent to survive a sudden death

public:
	
	// creates a Bug agent with his attributes inicialized with the in values
	Bug(const std::string & id, const int& maxMovement, const int &maxConsumptionRate, const int& survivalProbability, const int &size);
	// destroys a Bug agent
	virtual ~Bug();
	
	// selects wich actions and in wich order each Bug agent
	void selectActions();
	// selects wich attributes of the simulation are registered
	void registerAttributes();
	// gives value to the registered attributes
	void serialize();
	void updateState( );
	void updateKnowledge( );

	// setter of the _size attribute
	void setSize(const int &size);
	// getter of the _size attribute
	int getSize() const;
	// getter for the _maxBugMovement attribute
	int getMaxMovement() const;
	// getter of the _maxConsumptionRate attribute
	int getMaxConsumptionRate() const;
	// getter of the _survivalProbability attribute
	int getSurvivalProbability() const;
	// method that generates a new Bug agent (a child)
	void reproduce(const std::string &childId);


	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Bug( void * );
	void * fillPackage() const override;
	void freePackage(void* package) const override;
	bool hasTheSameAttributes(const Engine::Agent&) const override;
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif 

