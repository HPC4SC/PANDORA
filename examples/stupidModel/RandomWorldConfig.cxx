
#include <RandomWorldConfig.hxx>

namespace Examples
{

RandomWorldConfig::RandomWorldConfig( const std::string & xmlFile ) : Config(xmlFile), _numBugs(0)
{
}

RandomWorldConfig::~RandomWorldConfig()
{
}

void RandomWorldConfig::loadParams()
{
	_numBugs = getParamInt( "Bugs", "numBugs");
	_bugMaxConsumptionRate = getParamInt("Bugs", "maxFoodConsumption");
	_maxFoodProduction = getParamInt("Raster", "maxFoodProduction");
}
	
} // namespace Examples

