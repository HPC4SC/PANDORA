#ifndef __Client_hxx__
#define __Client_hxx__

#include <Phone.hxx>

#include <Agent.hxx>
#include <Action.hxx>
#include <Point2D.hxx>

namespace Examples 
{

class Client : public Engine::Agent 
{
    private:

        bool _sick; //MpiBasicAttribute
        bool _infected; //MpiBasicAttribute
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
		Phone* _phone;
    
    public:

        Client(const std::string& id, const int& sick, const float& purchaseSpeed, const float& stopping, const int& stopTime, 
		const int& entryTime,const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius);

        ~Client();

        void selectActions();

		bool isSick();

		void createPhone(const int& threshold1, const int& threshold2, const bool& hasApplication, const int& signalRadius);

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Client( void * );
	void * fillPackage();
	void freePackage(void* package) const override;
	bool hasTheSameAttributes(const Engine::Agent&) const override;
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

}

#endif