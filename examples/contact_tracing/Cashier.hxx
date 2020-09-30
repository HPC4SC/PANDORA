#ifndef __Cashier_hxx__
#define __Cashier_hxx__

#include <Phone.hxx>
#include <HumanBeeing.hxx>

#include <Action.hxx>

#include <vector>
#include <utility>

namespace Examples 
{

class Cashier : public Examples::HumanBeeing 
{
    private:

		int _shift; //MpiBasicAttribute
		int _workedTime; //MpiBasicAttribute

    public:
    
        Cashier(const std::string& id, const int& sick, const int& shift, const int& workedTime, 
		const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRaius, const int& encounterRadius);

        ~Cashier();

        void selectActions();

		void updateKnowledge();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Cashier( void * );
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