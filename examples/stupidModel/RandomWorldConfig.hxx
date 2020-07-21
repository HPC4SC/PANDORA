
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples 
{

class RandomWorldConfig : public Engine::Config {
		
	int _numBugs; // attribute that represents the initial number of Bug agents in the simulation
	int _maxBugMovement; // attribute that represents the maximum movement distance for all bugs.
	int _bugMaxConsumptionRate; // attribute that represents the maximum conspumtion of food of a Bug agent in one step
	int _survivalProbability; // attribute that represents the probability of a Bug agent to survive a sudden death
	int _initialBugSizeMean; // attribute that represents the initial mean Bug agent size 
	float _initialBugSizeSD; // attribute that represents the initial Standard Deviation of the Bug agent size
	int _numPredators; // attribute that represents the initial number of Predator agents in the simulation
	int _maxPredatorHuntDistance; // attribute that represents the maximum hunting distance for predators.
	int _maxFoodProduction; // attributhe that represents the maximum amount of food that each position can generate in one step

public:
	// creates a RandomWorldConfig instance
	RandomWorldConfig( const std::string & xmlFile );
	// destroys a RandomWordlConfig instance
	virtual ~RandomWorldConfig();

	// method that gets the initial values from the config file
	void loadParams();

	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

