#include <GymConfig.hxx>

namespace Examples 
{

    GymConfig::GymConfig(const std::string& xmlFile) : Config(xmlFile){}

    GymConfig::~GymConfig() {}

    void GymConfig::loadParams() {
        _mapRoute = getParamStr("Raster", "mapRoute");

        _mintimeOfExercice = getParamInt("Athlete","mintimeOfExercice");
        _agentRate = getParamInt("Athlete","agentRate");
        
        _infectiousness = getParamFloat("Epidemiology","infectiousness");
        _infectoinRadius = getParamInt("Epidemiology","infectoinRadius");
        _sickRate = getParamFloat("Epidemiology","sickRate");
        _encounterRadius = getParamInt("Epidemiology","encounterRadius");

        _signalRadius = getParamInt("Phone","signalRadius");
        _applicationRate = getParamFloat("Phone","applicationRate");
        _phoneThreshold1 = getParamInt("Phone","phoneThreshold1");
        _phoneThreshold2 = getParamInt("Phone","phoneThreshold2");
    }

    float GymConfig::getInfectiousness() const {
        return _infectiousness;
    }

}