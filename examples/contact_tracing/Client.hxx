#ifndef __Client_hxx__
#define __Client_hxx__

#include <Phone.hxx>

#include <Agent.hxx>
#include <Action.hxx>
#include <Point2D.hxx>

#include <vector>
#include <utility>

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
		int _encounterRadius;
		std::vector<std::pair<std::string,int>> _encountersReal;
		std::vector<std::pair<std::string,int>> _encountersRecorded;
		int _timeSpentWithOthers;
    
    public:

        Client(const std::string& id, const int& sick, const float& purchaseSpeed, const float& stopping, const int& stopTime, 
		const int& entryTime,const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius);

        ~Client();

        void selectActions();

		bool isSick();

		void createPhone(const int& threshold1, const int& threshold2, const bool& hasApplication, const int& signalRadius);

		void countEncountersReal();

		void countEncountersRecorded();

		bool phoneBroadcast();

		int phoneListen(const bool& sick, const double& distance);

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