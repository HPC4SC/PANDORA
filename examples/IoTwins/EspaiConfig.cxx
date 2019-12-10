#include <EspaiConfig.hxx>

namespace Examples {

    EspaiConfig::EspaiConfig(const std::string &xmlFile) : Config(xmlFile) {}

    EspaiConfig::~EspaiConfig() {}

    void EspaiConfig::loadParams() {
        _mapRoute = getParamStr("inputData", "map");
        _numAgents = getParamInt("inputData", "numAgents");
    }

}