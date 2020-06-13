#include <EspaiConfig.hxx>

namespace Examples {

    EspaiConfig::EspaiConfig(const std::string &xmlFile) : Config(xmlFile) {}

    EspaiConfig::~EspaiConfig() {}

    void EspaiConfig::loadParams() {
        _mapRoute = getParamStr("topology", "map");
        _entrancesRoute = getParamStr("topology", "entrances");
        _finalTargetsRoute = getParamStr("topology", "finalTargets");
        _targetsRoute = getParamStr("topology", "targets");
        _countersRoute = getParamStr("topology","counters");
        _museumRoute = getParamStr("topology","museum");
        _acces9Route = getParamStr("topology","acces9");
        _acces15Route = getParamStr("topology","acces15");

        _numAgents = getParamInt("inputData", "numAgents");
        _numCounters = getParamInt("inputData", "numCounters");

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
        _provMuseum = getParamInt("agentData", "provMuseum");

        _Calpha = getParamFloat("coefficients", "Calpha");
        _Cbeta = getParamFloat("coefficients", "Cbeta");
        _Cdelta = getParamFloat("coefficients", "Cdelta");
        _Csigma = getParamFloat("coefficients", "Csigma");
        _Ualpha = getParamFloat("coefficients", "Ualpha");
        _Ubeta = getParamFloat("coefficients", "Ubeta");
        _Udelta = getParamFloat("coefficients", "Udelta");
        _Usigma = getParamFloat("coefficients", "Usigma");

        _entrance9SD = getParamFloat("statistics", "entrance9SD");
        _entrance9M = getParamFloat("statistics", "entrance9M");
        _entrance15SD = getParamFloat("statistics", "entrance15SD");
        _entrance15M = getParamFloat("statistics", "entrance15M");
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