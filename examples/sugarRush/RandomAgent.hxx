
#ifndef __RandomAgent_hxx__
#define __RandomAgent_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples
{

class RandomAgent : public Engine::Agent
{
	int _wealth;
	int _vision;
	int _metabolicRate;
	int _currentAge;
	int _maxAge;
	
public:
	// todo remove environment from here
	RandomAgent(const std::string & id, const int &wealth, const int &vision, const int &metabolicRate, const int &currentAge, const int &maxAge);
	virtual ~RandomAgent();
	
	void selectActions();
	void registerAttributes();
	void serialize();

	int getWealth() const;
	void setWealth(const int& wealth);
	int getVision() const;
	int getMetabolicRate() const;
	int getCurrentAge() const;
	void incCurrentAge();
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

