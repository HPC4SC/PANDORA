#include <RestaurantConfig.hxx>

namespace Examples
{
    RestaurantConfig::RestaurantConfig(const std::string& xmlFile) : Config(xmlFile) {}

    RestaurantConfig::~RestaurantConfig() {}

    void RestaurantConfig::loadParams() {
        _mapRoute = getParamStr("Raster","mapRoute");

        _minEatTime = getParamInt("Customer","minEatTime");
        _maxEatTime = getParamInt("Customer","maxEatTime");
        _agentRate = getParamInt("Customer","agentRate");

        _infectiousness = getParamFloat("Epidemiology","infectiousness");
        _infectoinRadius = getParamInt("Epidemiology","infectoinRadius");
        _sickRate = getParamFloat("Epidemiology","sickRate");
        _encounterRadius = getParamInt("Epidemiology","encounterRadius");

        _signalRadius = getParamInt("Phone","signalRadius");
        _applicationRate = getParamFloat("Phone","applicationRate");
        _phoneThreshold1 = getParamInt("Phone","phoneThreshold1");
        _phoneThreshold2 = getParamInt("Phone","phoneThreshold2");
    }

    float RestaurantConfig::getInfectiousness() const {
        return _infectiousness;
    }
}