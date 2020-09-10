#include <Walker.hxx>

namespace Examples 
{

Walker::Walker(const std::string& id, const bool& sick, const int& encounterRadius, 
    const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int&signalRadius, const float& stopping,
	const int& stopTime, const int& entryTime, const float& wander, Street* street) 
    : HumanBeeing(id,sick,encounterRadius,phoneT1,phoneT2,phoneApp,signalRadius), _stopping(stopping), _stopTime(stopTime),
    _stopCounter(0), _entryTime(entryTime), _wander(wander), _street(street) {}

Walker::~Walker() {}

void Walker::selectActions() {}

}