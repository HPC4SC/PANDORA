#include <WanderAction.hxx>
#include <GeneralState.hxx>

namespace Examples {
    WanderAction::WanderAction() {}

    WanderAction::~WanderAction() {}

    void WanderAction::execute(Engine::Agent &agent) {
        Engine::World *world = agent.getWorld();
        Engine::Point2D<int> newPosition = selectNextPosition(agent,world);
        if(world->checkPosition(newPosition)) {
            agent.setPosition(newPosition);
        }
    }

    Engine::Point2D<int> WanderAction::selectNextPosition(Engine::Agent &agent, Engine::World *world) {
        int firstI, firstJ, lastI, lastJ;
        firstI = firstJ = lastI = lastJ = 0;
        std::vector<std::pair<Engine::Point2D<int>, int>> positionsInReach;
        Person &person = dynamic_cast<Person&>(agent);
        defineLoopBounds(firstI,firstJ,lastI,lastJ,person.getPosition()._x,person.getPosition()._y,person.getVelocity(),world);
        for (int i = firstI; i <= lastI; i++) {
            for (int j = firstJ; j <= lastJ; j++) {
                if (i != j) {
                    Engine::Point2D<int> point = Engine::Point2D<int>(i,j);
                    std::pair<Engine::Point2D<int>, int> newPoint(point,nearPeople(point,agent,world));
                    positionsInReach.push_back(newPoint);
                }
            }
        }
        Engine::Point2D<int> nextPosition = positionsInReach[0].first;
        int maxNearbyAgents = positionsInReach[0].second;
        for (int i = 1; i < positionsInReach.size(); i++) {
            if (positionsInReach[i].second > maxNearbyAgents) {
               nextPosition = positionsInReach[i].first;
               maxNearbyAgents = positionsInReach[i].second;
            }
        }
        return nextPosition;
    }

    int WanderAction::nearPeople(Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world) {
        Person &person = dynamic_cast<Person&>(agent);
        Engine::Agent * p_agent = world-> getAgent(agent.getId());
        Engine::AgentsVector neighbours = agent.getWorld()->getNeighbours(p_agent,person.getVision());
        return neighbours.size();
    }

    void WanderAction::defineLoopBounds(int &firstI,int &firstJ, int &lastI, int &lastJ, const int &posX, const int &posY,
                                      const int &capability, Engine::World *world) {
        Engine::Rectangle<int> boundaries = world->getBoundaries();
        firstI = posX - capability;
        firstJ = posY - capability;
        lastI = posX + capability;
        lastJ = posY + capability;
        if (firstI < boundaries.left()) firstI = boundaries.left();
        if (firstJ < boundaries.top()) firstJ = boundaries.top();
        if (lastI > boundaries.right()) lastI = boundaries.right();
        if (lastJ > boundaries.bottom()) lastJ = boundaries.bottom();
    }

    std::string WanderAction::describe() const {
        return "WanderAction";
    }
}
