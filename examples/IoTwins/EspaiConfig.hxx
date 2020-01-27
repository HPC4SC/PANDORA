#ifndef __EspaiConfig_hxx__
#define __EspaiConfig_hxx__

#include <Config.hxx>

namespace Examples {

    class EspaiConfig : public Engine::Config {

        std::string _mapRoute;

        std::string _entrancesRoute;

        int _numAgents;

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

    public:

        EspaiConfig(const std::string &xmlFile);

        virtual ~EspaiConfig();

        void loadParams();

        friend class EspaiBarca;

    };

}

#endif