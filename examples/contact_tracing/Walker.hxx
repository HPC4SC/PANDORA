#ifndef __Walker_hxx__
#define __Walker_hxx__

#include <HumanBeeing.hxx>
#include <Street.hxx>

#include <Action.hxx>

namespace Examples 
{

class Walker : public Examples::HumanBeeing
{
    
	private:
		float _stopping; //MpiBasicAttribute
		int _stopTime; //MpiBasicAttribute
		int _stopCounter; //MpiBasicAttribute
		int _entryTime; //MpiBasicAttribute
		float _drifting; //MpiBasicAttribute
		float _speed; //MpiBasicAttribute
		Street* _street;
		bool _directionTop;

	public:
		Walker(const std::string& id, const bool& sick, const int& encounterRadius, const int& phoneT1, 
		const int& phoneT2, const bool& phoneApp, const int& signalRadius, const float& stopping,
		const int& stopTime, const int& entryTime, const float& drifting, const float& speed, Street* street);

		~Walker();

		void selectActions();

		void setDirectionTop();

		void setDirectionBot();

		float getStopping();

		void setStopCount(const int& count);

		int getStopTime();

		bool directionTop();

		float getDrifting();

		float getSpeed();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Walker( void * );
	void * fillPackage() const;
	void freePackage(void* package) const;
	bool hasTheSameAttributes(const Examples::HumanBeeing&) const;
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

}

#endif