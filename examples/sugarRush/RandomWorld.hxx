
#ifndef __RandomWorld_hxx
#define __RandomWorld_hxx

#include <World.hxx>

namespace Examples 
{

class RandomWorldConfig;

class RandomWorld : public Engine::World
{
	// creates and initializes the Rasters of the simulation
	void createRasters();
	// creates and initializes the Agents of the simulation
	void createAgents();
public:
	// creates a RandomWold instance
	RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler = 0);
	// destroys a RandomWorld instance
	virtual ~RandomWorld();
	// executes the simulation one time-step 
	void step();
	// executes and chacks the changes of the rasters of the simulation one time-step 
	void stepEnvironment();
};

} // namespace Examples 

#endif // __RandomWorld_hxx

