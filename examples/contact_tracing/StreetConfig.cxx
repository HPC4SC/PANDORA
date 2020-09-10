#include <StreetConfig.hxx>

namespace Examples 
{

StreetConfig::StreetConfig(const std::string& xmlFile) : Config(xmlFile) {}

StreetConfig::~StreetConfig() {}

void StreetConfig::loadParams() {
    _walkerRate = getParamInt("Walker","walkerRate");
    _drifting = getParamFloat("Walker","drifting");
    _stopping = getParamFloat("Walker","stopping");
    _stopTime = getParamInt("Walker","stopTime");
    _wander = getParamFloat("Walker","wander");
    _speed = getParamInt("Walker","speed");
    _initialDensity = getParamFloat("Walker","initialDensity");

    _infectiousness = getParamFloat("Epidemiology","infectiousness");
    _infectoinRadius = getParamInt("Epidemiology","infectoinRadius");
    _sickRate = getParamFloat("Epidemiology","sickRate");
    _encounterRadius = getParamInt("Epidemiology","encounterRadius");
    _maxSick = getParamInt("Epidemiology","maxSick");

    _signalRadius = getParamInt("Phone","signalRadius");
    _applicationRate = getParamFloat("Phone","applicationRate");
    _phoneThreshold1 = getParamInt("Phone","phoneThreshold1");
    _phoneThreshold2 = getParamInt("Phone","phoneThreshold2");
}

float StreetConfig::getInfectiousness() const {
    return _infectiousness;
}

}