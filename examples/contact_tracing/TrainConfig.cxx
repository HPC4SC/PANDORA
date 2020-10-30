#include <TrainConfig.hxx>

namespace Examples
{

TrainConfig::TrainConfig(const std::string& xmlFile) : Config(xmlFile) {}

TrainConfig::~TrainConfig() {}

void TrainConfig::loadParams() {
    _mapRoute = getParamStr("Raster","mapRoute");
    _travelTimes= getParamStr("Raster","travelTimes");
    _seatCapacity = getParamInt("Raster","seatCapacity");

    _move = getParamFloat("Passanger","move");
    _sittingPreference = getParamFloat("Passanger","sittingPreference");
    _passangerEntry = getParamStr("Passanger","passangerEntry");
    _passangerExit = getParamStr("Passanger","passangerExit");
    _agentRate = getParamInt("Passanger","agentRate");

    _infectiosness = getParamFloat("Epidemiology","infectiousness");
    _infectoinRadius = getParamFloat("Epidemiology","infectoinRadius");
    _sickRate = getParamFloat("Epidemiology","sickRate");
    _encounterRadius = getParamFloat("Epidemiology","encounterRadius");

    _signalRadius = getParamFloat("Phone","signalRadius");
    _applicationRate = getParamFloat("Phone","applicationRate");
    _phoneThreshold1 = getParamFloat("Phone","phoneThreshold1");
    _phoneThreshold2 = getParamFloat("Phone","phoneThreshold2");
}

float TrainConfig::getInfectiousness() const {
    return _infectiosness;
}

}