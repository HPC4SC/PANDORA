#ifndef __TrainConfig_hxx__
#define __TrainConfig_hxx__

#include <Config.hxx>

#include <string>

namespace Examples 
{

class TrainConfig : public Engine::Config {
    private:
        std::string _mapRoute;
        std::string _travelTimes;
        int _seatCapacity;

        float _move;
        float _sittingPreference;
        std::string _passangerEntry;
        std::string _passangerExit;

        float _infectiosness;
        int _infectoinRadius;
        float _sickRate;
        float _encounterRadius;

        int _signalRadius;
        float _applicationRate;
        int _phoneThreshold1;
        int _phoneThreshold2;


    public:

        TrainConfig(const std::string& xmlFile);

        virtual ~TrainConfig();

        void loadParams();

        float getInfectiousness() const;

        friend class Train;
};

}

#endif