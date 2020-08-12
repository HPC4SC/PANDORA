#ifndef __Client_hxx__
#define __Client_hxx__

#include <Phone.hxx>
#include <HumanBeeing.hxx>

#include <Action.hxx>
#include <Point2D.hxx>

#include <vector>
#include <utility>

namespace Examples 
{

class Client : public Examples::HumanBeeing 
{
    private:

		bool _purchaseFinished; //MpiBasicAttribute
		Engine::Point2D<int> _targetPosition;
		int _itemsPurchased; //MpiBasicAttribute
		int _itemsPayed; //MpiBasicAttribute
		float _purchaseSpeedFactor; //MpiBasicAttribute
		std::vector<Engine::Point2D<int>> _memory;
		float _stopping; //MpiBasicAttribute
		int _stopTime; //MpiBasicAttribute
		int _stopCounter; //MpiBasicAttribute
		int _entryTime; //MpiBasicAttribute		
    
    public:

        Client(const std::string& id, const int& sick, const float& purchaseSpeed, const float& stopping, const int& stopTime, 
		const int& entryTime,const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius);

        ~Client();

        void selectActions();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Client( void * );
	void * fillPackage();
	void freePackage(void* package) const override;
	bool hasTheSameAttributes(const Examples::HumanBeeing&) const;
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

}

#endif