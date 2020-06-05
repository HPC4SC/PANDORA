#ifndef __Counter_hxx__
#define __Counter_hxx__

#include <Agent.hxx>

namespace Examples {

    class Counter : public Engine::Agent
        {

    private:

    int _agentCount;

    public:

    Counter(const std::string& id);

    ~Counter();

    void updateKnowledge();

    void registerAttributes();

    void serialize();


	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Counter( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

    };
}

#endif