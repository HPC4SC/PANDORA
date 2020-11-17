#ifndef __Customer_hxx__
#define __Customer_hxx__

#include <HumanBeeing.hxx>
#include <Restaurant.hxx>

#include <Point2D.hxx>

#include <vector>
#include <list>

namespace Examples
{

class Customer : public Examples::HumanBeeing
{

private:
	Engine::Point2D<int> _targetPosition;
	std::list<Engine::Point2D<int>> _targetPath;
	int _entryTime; //MpiBasicAttribute
	Restaurant* _restaurant;
    int _eatTime; //MpiBasicAttribute
	int _table;

public:
    Customer(const std::string& id, const double& infectiousness, const int& sick, const int& entryTime,const int& phoneT1, const int& phoneT2, 
        const bool& phoneApp, const int& signalRadius, const int& encounterRadius, Restaurant* restaurant, const int& eatTime, const int& table);

    ~Customer();

    void selectActions();

	std::list<Engine::Point2D<int>> getTargetPath();

	Restaurant* getRestaurant();

	void popTargetPath();

	void calculatePath();

};

}

#endif