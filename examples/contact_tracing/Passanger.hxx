#ifndef __Passanger_hxx__
#define __Passanger_hxx__

#include <HumanBeeing.hxx>
#include <Train.hxx>

namespace Examples
{

class Passanger : public Examples::HumanBeeing
{

    private:
        float _move;
        float _sittingPreference;
        Train* _train;

    public:
        Passanger(const std::string& id, const bool& sick, const int& encounterRadius, const int& phoneT1, 
	    const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& move, const int& sittingPreference, Train* street);

        ~Passanger();

        void selectActions();

};

}

#endif