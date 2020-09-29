#ifndef __Supermarket_hxx__
#define __Supermarket_hxx__

#include <SupermarketConfig.hxx>

#include <World.hxx>

#include <vector>
#include <map>
#include <utility>
#include <queue>

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

    std::map<int,std::map<int,double>> _transitionProbabilities;

    std::map<int,std::vector<Engine::Point2D<int>>> _zoneTargets; 

    std::map<int,double> _zoneProbabilities;

    std::vector<int> _zones = {110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 202};

    std::map<int,Engine::Point2D<int>> _centroids;

    int _cashierId = 0;

    int _clientId = 0;

    int _currentSickCashiers = 0;

    bool _first = true;

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

        bool isCashier(Engine::Point2D<int> point);

        Engine::Point2D<int> getRandomExit();

        int getCurrentZone(const Engine::Point2D<int>& pos);

        std::map<int,double> getTransitionProbabilities(const int& zone);

        void setupTransitionProbabilities();

        void printTransitionProbabilities();

        void setupZoneProbabilities();

        void addPurchaseProbability();

        void calculateCentroids();

        void calculateTransitionProbabilities();
        
        void normalizeTransitionProbabilities();

        Engine::Point2D<int> pickTargetFromZone(const int& zone);

        std::list<Engine::Point2D<int>> getShortestPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target);

        void exploreNeighbours(int& r, int& c, std::vector<std::vector<bool>>& visited, std::queue<int>& rowQueue, std::queue<int>& columnQueue, std::vector<std::vector<Engine::Point2D<int>>>& prev);

        bool validPosition(const int& rr, const int& cc, const std::vector<std::vector<bool>>& visited);

        std::list<Engine::Point2D<int>> reconstructPath(const Engine::Point2D<int>& pos, const Engine::Point2D<int>& target, const std::vector<std::vector<Engine::Point2D<int>>>& prev);
};

}

#endif