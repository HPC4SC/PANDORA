#ifndef __RestaurantConfig_hxx__
#define __RestaurantConfig_hxx__

#include <Config.hxx>

#include <string>

namespace Examples
{

class RestaurantConfig : public Engine::Config {

private:

    std::string _mapRoute;

    int _minEatTime;
    int _maxEatTime;
    int _agentRate;
    int _minGroupSize;
    int _maxGroupSize;

    float _infectiousness;
    int _infectoinRadius;
    float _sickRate;
    int _encounterRadius;

    int _signalRadius;
    float _applicationRate;
    int _phoneThreshold1;
    int _phoneThreshold2;

public:

    RestaurantConfig(const std::string& xmlFile);

    ~RestaurantConfig();

    void loadParams();

    float getInfectiousness() const;

    friend class Restaurant;

};

}

#endif