#ifndef __StreetConfig_hxx__
#define __StreetConfig_hxx__

#include <Config.hxx>

#include <string>

namespace Examples 
{

class StreetConfig : public Engine::Config {

private:
        
    int _walkerRate;
    float _drifting;
    float _stopping;
    int _stopTime;
    float _wander;
    int _speed;
    float _initialDensity;

    float _infectiousness;
    int _infectoinRadius;
    float _sickRate;
    int _encounterRadius;
    int _maxSick;

    int _signalRadius;
    float _applicationRate;
    int _phoneThreshold1;
    int _phoneThreshold2;

public:

    StreetConfig(const std::string& xmlFile);

    virtual ~StreetConfig();

    void loadParams();

    float getInfectiousness() const;

    friend class Street;

};

}

#endif