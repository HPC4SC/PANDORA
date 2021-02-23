
#ifndef __RandomWorld_hxx
#define __RandomWorld_hxx

#include <World.hxx>
#include <RNGNormal.hxx>
#include <RNGUniformDouble.hxx>

namespace Examples 
{

class RandomWorldConfig;

class RandomWorld : public Engine::World {
	
	int _maxProductionRate; // attributhe that represents the maximum amount of food that each position can generate in one step
	
public:
	// creates a RandomWorld instance
	RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler = 0);
	// destroys a RandomWorld instance
	virtual ~RandomWorld();
	
	// method that creates and initializes the rasters of the simulation
	void createInitialRasters();
	// method that creates and initializes the agents of the simulation
	void createInitialAgents();
	
	// getter of the _maxProductionRate attribute
	int getMaxProductionRate() const;
	// setter of the _maxProductionRate attribute
	void setMaxProductionRate(const int& maxProductionRate);
	// method that describes what happens in the World in one step
	void step();
	// method that advances the world one step in time
	void stepEnvironment() override;
};

} // namespace Examples 

#endif // __RandomWorld_hxx

