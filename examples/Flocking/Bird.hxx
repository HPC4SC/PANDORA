
#ifndef __Bird_hxx__
#define __Bird_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>
#include <vector>

namespace Examples
{

class Bird : public Engine::Agent
{
	int _velocity;
	int _sigth;
	int _mindist;
	float _heading;
	float _max_A_turn;
	float _max_C_turn;
	float _max_S_turn;

public:
	// todo remove environment from here
	Bird(const std::string & id, const int &velocity, const int &sigth, const int &mindist, const float &max_A_turn, const float &max_C_turn, const float &max_S_turn);
	virtual ~Bird();
	
	void selectActions();
	void updateState();
	void registerAttributes();
	void serialize();

	void setVelocity(int v);
	int getVelocity() const;
	void setSigth(int s);
	int getSigth() const;
	void setMindist(int d);
	int getMindist();
	
	Bird getBird();
	void setHeading(float h);
	float getHeading();
	void setmaxATurn(float maxTurn);
	float getmaxATurn() const;
	void setmaxCTurn(float maxTurn);
	float getmaxCTurn() const;
	void setmaxSTurn(float maxTurn);
	float getmaxSTurn();
	
	int distNearestNeigthbour();

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

