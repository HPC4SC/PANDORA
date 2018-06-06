
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples {

class RandomWorldConfig : public Engine::Config {
		
	int _numBirds;
	int _agentVelocity;
	int _agentSigth;
	int _agentMindist;
	float _agentMaxATrun;
	float _agentMaxCTrun;
	float _agentMaxSTrun;
	
public:
	RandomWorldConfig( const std::string & xmlFile );
	virtual ~RandomWorldConfig();

	void loadParams();

	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

