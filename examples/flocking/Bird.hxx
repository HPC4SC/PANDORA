
#ifndef __Bird_hxx__
#define __Bird_hxx__

#include <Agent.hxx>
#include <Action.hxx>
#include <GeneralState.hxx>

#include <string>
#include <vector>

namespace Examples {

class Bird : public Engine::Agent 
{
	
	int _velocity; // MpiBasicAttribute: attribute that represents the vlocity of the bird Agent
	int _sigth; // MpiBasicAttribute: attribute that represents the radius of the vision of the birds
	int _mindist; // MpiBasicAttribute: attribute that represents the minim distance between bird Agents
	float _heading; // MpiBasicAttribute: attribute that represents the direction in witch the bird is moving towards
	float _max_A_turn; // MpiBasicAttribute: attribute that represents the maximum turn that a bird will do when it aligns
	float _max_C_turn; // MpiBasicAttribute: attribute that represents the maximum turn that a bird will do when it coheres
	float _max_S_turn; // MpiBasicAttribute: attribute that represents the maximum turn that a bird will do when it separates

public:
	// creates a bird Agent instance and initiallazes its attributes with the in values
	Bird(const std::string & id, const int &velocity, const int &sigth, const int &mindist, const float &max_A_turn, const float &max_C_turn, const float &max_S_turn);
	// destroys a bird Agent instance
	virtual ~Bird();
	// selects wich actions and in wich order the bird Agent executes them
	void selectActions();
	// checks that the heading of the bird Agent is between the correct range
	void updateState();
	// returns the value of the _velocity attribute of the bird Agent
	int getVelocity() const;
	// returns the value of the _sigth attribute of the bird Agent
	int getSigth() const;
	// returns the value of the _mindist attribute of the bird Agent
	int getMindist() const;
	// sets the value of the _heading attribute of the bird Agent to h
	void setHeading(float h);
	// returns the value of the _heading attribute of the bird Agent
	float getHeading();
	// returns the value of the _max_A_turn attribute of the bird Agent
	float getMaxATurn() const;
	// returns the value of the _max_C_turn attribute of the bird Agent
	float getMaxCTurn() const;
	// returns the value of the _max_S_turn attribute of the bird Agent
	float getMaxSTurn() const;

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
    Bird( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __Bird_hxx__

