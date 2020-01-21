
#include <RandomWorldConfig.hxx>

namespace Examples
{

RandomWorldConfig::RandomWorldConfig( const std::string & xmlFile ) : Config(xmlFile)
{
}

RandomWorldConfig::~RandomWorldConfig(){}

void RandomWorldConfig::loadParams(){
    _numAgents = getParamInt("inputData", "numAgents");
    _minWealth = getParamInt("inputData", "minWealth");
    _maxWealth = getParamInt("inputData", "maxWealth");
    _minVision = getParamInt("inputData", "minVision");
    _maxVision = getParamInt("inputData", "maxVision");
    _minMr = getParamInt("inputData", "minMr");
    _maxMr = getParamInt("inputData", "maxMr");
    _minMAge = getParamInt("inputData", "minMAge");
    _maxMAge = getParamInt("inputData", "maxMAge");
}

	
} // namespace Examples

