
#include <RandomWorldConfig.hxx>

namespace Examples 
{

RandomWorldConfig::RandomWorldConfig( const std::string & xmlFile ) : Config(xmlFile), _numBugs(0) {}

RandomWorldConfig::~RandomWorldConfig() {}

void RandomWorldConfig::loadParams() {
	// this are the in parameters that you can find and change in the config.xml file
	_numBugs = getParamInt( "Bugs", "numBugs");
	_bugMaxConsumptionRate = getParamInt("Bugs", "maxFoodConsumption");
	_initialBugSizeMean = getParamInt("Bugs", "initialBugSizeMean");
	_initialBugSizeSD = getParamFloat("Bugs", "initialBugSizeSD");
	_maxFoodProduction = getParamInt("Raster", "maxFoodProduction");
}
	
} // namespace Examples

