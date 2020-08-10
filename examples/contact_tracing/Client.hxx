#ifndef __Client_hxx__
#define __Client_hxx__

#include <Agent.hxx>
#include <Action.hxx>

namespace Examples 
{

class Client : public Engine::Agent 
{
    private:

        bool _sick; //MpiBasicAttribute
        bool _infected;
    
    public:

        Client(const std::string &id,const int &sick);

        ~Client();

        void selectActions();

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