
#include <RandomWorldConfig.hxx>

namespace Examples {

RandomWorldConfig::RandomWorldConfig( const std::string & xmlFile ) : Config(xmlFile), _numBirds(0) {}

RandomWorldConfig::~RandomWorldConfig() {}

void RandomWorldConfig::loadParams() {
	_numBirds = getParamInt( "Birds", "numBirds");
	_agentVelocity = getParamInt("Birds", "velocity");
	_agentSigth = getParamInt("Birds", "sigth");
	_agentMindist = getParamInt("Birds", "mindist");
	_agentMaxATrun = getParamFloat("Birds", "max_A_turn");
	_agentMaxCTrun = getParamFloat("Birds", "max_C_turn");
	_agentMaxSTrun = getParamFloat("Birds", "max_S_turn");
}
	
} // namespace Examples

