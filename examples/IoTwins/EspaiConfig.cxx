#include <EspaiConfig.hxx>

namespace Examples {

    EspaiConfig::EspaiConfig(const std::string &xmlFile) : Config(xmlFile) {}

    EspaiConfig::~EspaiConfig() {}

    void EspaiConfig::loadParams() {
        _mapRoute = getParamStr("inputData", "map");
        _numAgents = getParamInt("inputData", "numAgents");
        _minAgentVision = getParamInt("agentData", "minVision");
        _maxAgentVision = getParamInt("agentData", "maxVision");
        _minAgentVelocity = getParamInt("agentData", "minVelocity");
        _maxAgentVelocity = getParamInt("agentData", "maxVelocity");
        _minAgentAge = getParamInt("agentData", "minAge");
        _maxAgentAge = getParamInt("agentData", "maxAge");
        _provTourist = getParamFloat("agentData", "porvTourist");
    }

}