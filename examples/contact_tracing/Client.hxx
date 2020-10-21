#ifndef __Client_hxx__
#define __Client_hxx__

#include <HumanBeeing.hxx>
#include <Supermarket.hxx>

#include <Point2D.hxx>

#include <vector>
#include <utility>
#include <list>

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
		std::list<Engine::Point2D<int>> _memory;
		float _stopping; //MpiBasicAttribute
		int _stopTime; //MpiBasicAttribute
		int _stopCounter; //MpiBasicAttribute
		int _entryTime; //MpiBasicAttribute
		bool _purchaseDecided;
		Supermarket* _super;
		float _wander;
    
    public:

        Client(const std::string& id, const double& infectiousness, const int& sick, const float& purchaseSpeed, const float& stopping, const int& stopTime, 
		const int& entryTime,const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius,
		Supermarket* super, const float& wander);

        ~Client();

        void selectActions();

		int getStopCounter();

		std::list<Engine::Point2D<int>> getMemory();

		void decreaseStopCounter();

		Engine::Point2D<int> getTargetPosition();

		float getStopping();

		void setStopCounter(const int& stopCounter);

		int getStopTime();

		void setMemory(const std::list<Engine::Point2D<int>>& path);

		void popFrontMemory();

		float getWander();

		Supermarket* getSuper();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Client( void * );
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