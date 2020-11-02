#ifndef __Athlete_hxx__
#define __Athlete_hxx__

#include <HumanBeeing.hxx>
#include <Gym.hxx>

#include <Point2D.hxx>

#include <list>

namespace Examples
{

class Athlete : public Examples::HumanBeeing 
{

private:
    std::list<Engine::Point2D<int>> _targetPath;
    Engine::Point2D<int> _targetPosition;
    std::list<int> _exerciceList;
    int _exerciceTime; //MpiBasicAttribute
    Gym* _gym;

public:
    Athlete(const std::string& id, const double& infectiousness, const int& sick, const int& entryTime, 
        const int& phoneT1, const int& phoneT2, const bool& phoneApp, const int& signalRadius, const int& encounterRadius,
		Gym* gym, const std::list<int>& exerciceList);

    ~Athlete();

    void selectActions();

};

}

#endif