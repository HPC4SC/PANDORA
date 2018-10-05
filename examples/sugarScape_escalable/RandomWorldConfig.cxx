
#include <RandomWorldConfig.hxx>

namespace Examples
{

RandomWorldConfig::RandomWorldConfig( const std::string & xmlFile ) : Config(xmlFile)
{
}

RandomWorldConfig::~RandomWorldConfig()
{
}

void RandomWorldConfig::loadParams() {
	_numAgents = getParamInt ( "Agents", "numAgents");
}

	
} // namespace Examples

