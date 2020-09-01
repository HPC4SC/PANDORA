#ifndef __HumanBeeing_hxx__
#define __HumanBeeing_hxx__

#include <Phone.hxx>

#include <Agent.hxx>
#include <Action.hxx>

#include <vector>
#include <utility>
#include <string>

namespace Examples 
{

    class HumanBeeing : public Engine::Agent
    {
        private:

            Phone* _phone;
            std::vector<std::pair<std::string,int>> _encountersReal;
            std::vector<std::pair<std::string,int>> _encountersRecorded;
            int _countInfected;
            int _infectionTime;
            int _phoneActiveCount;

        protected:
        
            bool _sick; //MpiBasicAttribute
            bool _infected; //MpiBasicAttribute
            int _timeSpentWithOthers;
            int _encounterRadius;

        public:

            HumanBeeing(const std::string& id, const bool& sick, const int& encounterRadius, const int& phoneT1, 
                const int& phoneT2, const bool& phoneApp, const int& signalRadius);

            ~HumanBeeing();

            bool isSick();

            bool isInfected();

            Phone* getPhonePointer();

            int getEncounterRadius();

            void updateKnowledge();

            void createPhone(const int& threshold1, const int& threshold2, const bool& hasApplication, const int& signalRaius);

            void countEncountersReal();

            void countEncountersRecorded();
            
            bool phoneBroadcast();

            int phoneListen(const bool& sick, const double& distance);

            void getInfected();

            void incCountInfected();

            void setInfectionTime(const int& infectionTime);

            void printEncounters();

            void registerAttributes();

		    void serialize();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	HumanBeeing( void * );
	void * fillPackage();
	void freePackage(void* package) const;
	bool hasTheSameAttributes(const Engine::Agent&) const;
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

    };

}

#endif