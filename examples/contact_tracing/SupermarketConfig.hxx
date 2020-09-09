#ifndef __SupermarketConfig_hxx__
#define __SupermarketConfig_hxx__

#include <Config.hxx>

#include <iostream>
#include <string>

namespace Examples 
{

class SupermarketConfig : public Engine::Config {
    private:
        
        int _clientRate;
        float _drifting;
        float _stopping;
        int _stopTime;
        float _wander;

        int _numCashiers;
        int _sickCashiers;
        int _cashierShift;

        std::string _mapRoute;

        float _infectiosness;
        int _infectoinRadius;
        float _sickRate;
        int _encounterRadius;
        int _maxSick;

        int _signalRadius;
        float _applicationRate;
        int _phoneThreshold1;
        int _phoneThreshold2;

    public:

        SupermarketConfig(const std::string& xmlFile);

        virtual ~SupermarketConfig();

        void loadParams();

        float getInfectiousness() const;

        friend class Supermarket;
};

}

#endif