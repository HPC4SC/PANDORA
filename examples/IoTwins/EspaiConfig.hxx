#ifndef __EspaiConfig_hxx__
#define __EspaiConfig_hxx__

#include <Config.hxx>

namespace Examples {

    class EspaiConfig : public Engine::Config {

        std::string _mapRoute;
        int _numAgents;

    public:

        EspaiConfig(const std::string &xmlFile);

        virtual ~EspaiConfig();

        void loadParams();

        friend class EspaiBarca;

    };

}

#endif