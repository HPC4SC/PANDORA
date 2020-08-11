#include <Phone.hxx>

namespace Examples 
{

Phone::Phone(const int& threshold1, const int& threshold2, const bool& hasApplicattion, const int& signalRadius) 
: _threshold1(_threshold1), _threshold2(threshold2), _hasApplication(hasApplicattion), _signalRadius(signalRadius) {}

Phone::~Phone() {}

}