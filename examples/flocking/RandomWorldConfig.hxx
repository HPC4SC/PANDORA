
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples {

class RandomWorldConfig : public Engine::Config {
		
	int _numBirds; // attribute that represents the number of bird Agents in the simulation
	int _agentVelocity; // attribute that represents the velocity of the bird Agent
	int _agentSigth; // attribute that represents the sigth of the bird Agent
	int _agentMindist; // attribute that represents the mindist of the bird Agent
	float _agentMaxATrun; // attribute that represents the maximum align turn of a bird Agent
	float _agentMaxCTrun; // attribute that represents the maximum cohere turn of a bird Agent
	float _agentMaxSTrun; // attribute that represents the maximum separation turn of a bird Agent
	
public:
	// creates a RandomWroldConfig instance
	RandomWorldConfig( const std::string & xmlFile );
	// destroys RandomWorldConfig instance
	virtual ~RandomWorldConfig();
	// method that loads the value form the config file
	void loadParams();

	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

