#ifndef __Cashier_hxx__
#define __Cashier_hxx__

#include <Phone.hxx>

#include <Agent.hxx>
#include <Action.hxx>

#include <vector>
#include <utility>

namespace Examples 
{

class Cashier : public Engine::Agent 
{
    private:

        bool _sick; //MpiBasicAttribute
        bool _infected; //MpiBasicAttribute
		int _shift; //MpiBasicAttribute
		int _workedTime; //MpiBasicAttribute
		int _encounterRadius;
		Phone* _phone;
		std::vector<std::pair<std::string,int>> _encountersReal;
		std::vector<std::pair<std::string,int>> _encountersRecorded;
		int _timeSpentWithOthers;

    public:
    
        Cashier(const std::string& id, const int& sick, const int& shift, const int& workedTime, 
		const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRaius, const int& encounterRadius);

        ~Cashier();

        void selectActions();

		void updateKnowledge();

		bool isSick();

		void createPhone(const int& threshold1, const int& threshold2, const bool& hasApplication, const int& signalRaius);

		void countEncountersReal();

		void countEncountersRecorded();
		
		bool phoneBroadcast();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Cashier( void * );
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