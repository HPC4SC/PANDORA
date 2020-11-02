#ifndef __GymConfig_hxx__
#define __GymConfig_hxx__

#include <Config.hxx>

#include <string>

namespace Examples
{

class GymConfig : public Engine::Config {

private:

    std::string _mapRoute;

    int _mintimeOfExercice;
    int _agentRate;

    float _infectiousness;
    int _infectoinRadius;
    float _sickRate;
    int _encounterRadius;

    int _signalRadius;
    float _applicationRate;
    int _phoneThreshold1;
    int _phoneThreshold2;

public:

    GymConfig(const std::string& xmlFile);

    ~GymConfig();

    void loadParams();

    float getInfectiousness() const;

    friend class Gym;

};

}

#endif