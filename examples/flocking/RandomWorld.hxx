
#ifndef __RandomWorld_hxx
#define __RandomWorld_hxx

#include <World.hxx>

namespace Examples  {

class RandomWorldConfig;

class RandomWorld : public Engine::World {
	
	
public:
	// creates a RandomWorld instance
	RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler = 0);
	// destroys a RandomWorld instance
	virtual ~RandomWorld();
	// method that creates and initializes the Agents
	void createAgents();
};

} // namespace Examples 

#endif // __RandomWorld_hxx

