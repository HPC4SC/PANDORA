#include <SupermarketConfig.hxx>

namespace Examples 
{

SupermarketConfig::SupermarketConfig(const std::string& xmlFile) : Config(xmlFile) {}

SupermarketConfig::~SupermarketConfig() {}

void SupermarketConfig::loadParams() {
    _clientRate = getParamInt("Client","clientRate");
    _drifting = getParamFloat("Client","drifting");
    _stopping = getParamFloat("Client","stopping");
    _stopTime = getParamInt("Client","stopTime");
    _wander = getParamFloat("Client","wander");

    _numCashiers = getParamInt("Cashier","numCashiers");
    _sickCashiers = getParamInt("Cashier", "sickCashiers");
    _cashierShift = getParamInt("Cashier","cashierShift");

    _mapRoute = getParamStr("Raster","mapRoute");

    _infectiosness = getParamFloat("Epidemiology","infectiousness");
    _infectoinRadius = getParamInt("Epidemiology","infectoinRadius");
    _sickRate = getParamFloat("Epidemiology","sickRate");
    _encounterRadius = getParamInt("Epidemiology","encounterRadius");
    _maxSick = getParamInt("Epidemiology","maxSick");

    _signalRadius = getParamInt("Phone","signalRadius");
    _applicationRate = getParamFloat("Phone","applicationRate");
    _phoneThreshold1 = getParamInt("Phone","phoneThreshold1");
    _phoneThreshold2 = getParamInt("Phone","phoneThreshold2");
}

float SupermarketConfig::getInfectiousness() const{
    return _infectiosness;
}

}