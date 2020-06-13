#ifndef __EspaiConfig_hxx__
#define __EspaiConfig_hxx__

#include <Config.hxx>

namespace Examples {

    class EspaiConfig : public Engine::Config
            {
        std::string _mapRoute;
        std::string _entrancesRoute;
        std::string _finalTargetsRoute;
        std::string _targetsRoute;
        std::string _countersRoute;
        std::string _museumRoute;
        std::string _acces9Route;
        std::string _acces15Route;

        int _numAgents;
        int _numCounters;

        int _minAgentVision;
        int _maxAgentVision;
        int _minAgentVelocity;
        int _maxAgentVelocity;
        int _minAgentAge;
        int _maxAgentAge;
        float _provTourist;
        int _minWallDistance;
        int _minAgentDistance;
        int _maxWallDistance;
        int _maxAgentDistance;
        int _maxDistanceBAgents;
        int _provFollow;
        int _provMuseum;
        
        float _Calpha;
        float _Cbeta;
        float _Cdelta;
        float _Csigma;
        float _Ualpha;
        float _Ubeta;
        float _Udelta;
        float _Usigma;

        float _entrance9SD;
        float _entrance9M;
        float _entrance15SD;
        float _entrance15M;

    public:

        EspaiConfig(const std::string &xmlFile);

        virtual ~EspaiConfig();

        void loadParams();

        friend class EspaiBarca;

        float getCostAlpha() const;

        float getCostBeta() const;

        float getCostDelta() const;

        float getCostSigma() const;

        float getUtilityAlpha() const;

        float getUtilityBeta() const;

        float getUtilityDelta() const;

        float getUtilitySigma() const;

    };

}

#endif