#include <Passanger.hxx>

namespace Examples
{

Passanger::Passanger(const std::string& id, const bool& sick, const int& encounterRadius, const int& phoneT1, 
	    const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& move, const int& sittingPreference, Train* street) : 
        HumanBeeing(id,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _move(move), _sittingPreference(sittingPreference) {}

Passanger::~Passanger() {}

void Passanger::selectActions() {}

}