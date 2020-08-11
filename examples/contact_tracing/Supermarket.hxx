#ifndef __Supermarket_hxx__
#define __Supermarket_hxx__

#include <SupermarketConfig.hxx>

#include <World.hxx>

#include <vector>

namespace Examples 
{

class Supermarket : public Engine::World 
{
    private:

    const SupermarketConfig &_supermarketConfig = (const SupermarketConfig &) getConfig();

    std::vector<Engine::Point2D<int>> _obstacles;

    std::vector<Engine::Point2D<int>> _cashierWorkplace;

    std::vector<Engine::Point2D<int>> _cashierTill;

    std::vector<Engine::Point2D<int>> _entry;

    std::vector<Engine::Point2D<int>> _exit;
    
    std::vector<Engine::Point2D<int>> _exitZone;

    std::vector<Engine::Point2D<int>> _purchaseTargets;

    int _cashierId = 0;

    int _clientId = 0;

    int _currentSickCashiers = 0;

    public:
        
        Supermarket(Engine::Config* config, Engine::Scheduler* scheduler = 0);

        ~Supermarket();

        void createRasters();
	
	    void createAgents();

        void createCashier();

        void createClient();

        void step();

        void devideLayout();

        bool isObstacle(Engine::Point2D<int> point);

        bool isExit(Engine::Point2D<int> point);
        
        bool isPossibleTarget(Engine::Point2D<int> point);
};

}

#endif