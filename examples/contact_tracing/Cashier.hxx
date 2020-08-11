#ifndef __Cashier_hxx__
#define __Cashier_hxx__

#include <Agent.hxx>
#include <Action.hxx>

namespace Examples 
{

class Cashier : public Engine::Agent 
{
    private:

        bool _sick; //MpiBasicAttribute
        bool _infected; //MpiBasicAttribute
		int _shift; //MpiBasicAttribute
		int _workedTime; //MpiBasicAttribute

    public:
    
        Cashier(const std::string& id, const int& sick, const int& shift, const int& workedTime);

        ~Cashier();

        void selectActions();

		bool isSick();

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