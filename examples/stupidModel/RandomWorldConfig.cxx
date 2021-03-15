
#include <RandomWorldConfig.hxx>

namespace Examples 
{

RandomWorldConfig::RandomWorldConfig( const std::string & xmlFile ) : Config(xmlFile), _numBugs(0) {}

RandomWorldConfig::~RandomWorldConfig() {}

void RandomWorldConfig::loadParams() {
	// this are the in parameters that you can find and change in the config.xml file
	_numBugs = getParamInt( "Bugs", "numBugs");
	_maxBugMovement = getParamInt( "Bugs", "maxBugMovement");
	_bugMaxConsumptionRate = getParamInt("Bugs", "maxFoodConsumption");
	_survivalProbability = getParamInt("Bugs", "survivalProbability");
	_initialBugSizeMean = getParamInt("Bugs", "initialBugSizeMean");
	_initialBugSizeSD = getParamFloat("Bugs", "initialBugSizeSD");
	_numPredators = getParamInt("Predators", "numPredators");
	_maxPredatorHuntDistance = getParamInt("Predators", "maxPredatorHuntDistance");
	_maxFoodProduction = getParamInt("Raster", "maxFoodProduction");
}
	
} // namespace Examples

