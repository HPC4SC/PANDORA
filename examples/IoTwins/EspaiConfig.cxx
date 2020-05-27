#include <EspaiConfig.hxx>

namespace Examples {

    EspaiConfig::EspaiConfig(const std::string &xmlFile) : Config(xmlFile) {}

    EspaiConfig::~EspaiConfig() {}

    void EspaiConfig::loadParams() {
        _mapRoute = getParamStr("topology", "map");
        _entrancesRoute = getParamStr("topology", "entrances");
        _finalTargetsRoute = getParamStr("topology", "finalTargets");
        _targetsRoute = getParamStr("topology", "targets");

        _numAgents = getParamInt("inputData", "numAgents");

        _minAgentVision = getParamInt("agentData", "minVision");
        _maxAgentVision = getParamInt("agentData", "maxVision");
        _minAgentVelocity = getParamInt("agentData", "minVelocity");
        _maxAgentVelocity = getParamInt("agentData", "maxVelocity");
        _minAgentAge = getParamInt("agentData", "minAge");
        _maxAgentAge = getParamInt("agentData", "maxAge");
        _provTourist = getParamFloat("agentData", "porvTourist");
        _minWallDistance = getParamInt("agentData", "minWallDistance");
        _maxWallDistance = getParamInt("agentData", "minWallDistance");
        _minAgentDistance = getParamInt("agentData", "minAgentDistance");
        _maxAgentDistance = getParamInt("agentData", "minAgentDistance");
        _maxDistanceBAgents = getParamInt("agentData", "maxDistanceBAgents");
        _provFollow = getParamInt("agentData", "provFollow");

        _Calpha = getParamFloat("coeficients", "Calpha");
        _Cbeta = getParamFloat("coeficients", "Cbeta");
        _Cdelta = getParamFloat("coeficients", "Cdelta");
        _Csigma = getParamFloat("coeficients", "Csigma");
        _Ualpha = getParamFloat("coeficients", "Ualpha");
        _Ubeta = getParamFloat("coeficients", "Ubeta");
        _Udelta = getParamFloat("coeficients", "Udelta");
        _Usigma = getParamFloat("coeficients", "Usigma");
    }

    float EspaiConfig::getCostAlpha() const {
        return _Calpha;
    }

    float EspaiConfig::getCostBeta() const {
        return _Cbeta;
    }

    float EspaiConfig::getCostDelta() const {
        return _Cdelta;
    }

    float EspaiConfig::getCostSigma() const {
        return _Csigma;
    }

    float EspaiConfig::getUtilityAlpha() const {
        return _Ualpha;
    }

    float EspaiConfig::getUtilityBeta() const {
        return _Ubeta;
    }

    float EspaiConfig::getUtilityDelta() const {
        return _Udelta;
    }

    float EspaiConfig::getUtilitySigma() const {
        return _Usigma;
    }

}