
#ifndef __Bug_hxx__
#define __Bug_hxx__

#include <Agent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples 
{

class Bug : public Engine::Agent 
{
	
	int _size;
	int _maxConsumptionRate;
	std::string _color;
	int _survivalProbability = 95;

public:
	// todo remove environment from here
	Bug( const std::string & id , const int &maxConsumptionRate, const int &size);
	virtual ~Bug();
	
	void selectActions();
	void updateState();
	void registerAttributes();
	void serialize();

	void setSize(const int &size);
	int getSize() const;
	void setColor(const int &code);
	std::string getColor() const;
	int getMaxConsumptionRate() const;
	int getSurvivalProbability() const;
	void kill();
	void reproduce(const std::string &childId);

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Bug( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __GubAgent_hxx__

