#include <WanderAction.hxx>
#include <GeneralState.hxx>
#include <EspaiConfig.hxx>

namespace Examples {
    WanderAction::WanderAction() {}

    WanderAction::~WanderAction() {}

    void WanderAction::execute(Engine::Agent &agent) {
        //std::cout << "I'm going to wander now" << std::endl;
        Engine::World *world = agent.getWorld();
        Engine::Point2D<int> newPosition = selectNextPosition(agent,world);
        Person &person = dynamic_cast<Person&>(agent);
        if(world->checkPosition(newPosition)) {
            agent.setPosition(newPosition);
            std::cout << "I'm " << agent.getId() << " and I wander to "  << newPosition << " and my finalTarget is "  << person.getFinalTarget() << " the distance to the target is: "  << newPosition.distance(person.getFinalTarget()) << std::endl;
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
                Engine::Point2D<int> point = Engine::Point2D<int>(i,j);
                if (i != j and world->getStaticRaster("buildings").getValue(point) == 1) {
                    std::pair<Engine::Point2D<int>, int> newPoint(point,calculateUtility(point,agent,world));
                    positionsInReach.push_back(newPoint);
                }
            }
        }
        Engine::Point2D<int> nextPosition = positionsInReach[0].first;
        int maxNearbyAgents = positionsInReach[0].second;
        for (unsigned int i = 1; i < positionsInReach.size(); i++) {
            if (positionsInReach[i].second > maxNearbyAgents) {
               nextPosition = positionsInReach[i].first;
               maxNearbyAgents = positionsInReach[i].second;
            }
        }
        return nextPosition;
    }

    int WanderAction::calculateUtility(Engine::Point2D<int> point, Engine::Agent& agent, Engine::World* world) {
        const EspaiConfig& config = (const EspaiConfig &) world->getConfig();
        int utility = nearPeople(agent,world) * config.getUtilityAlpha();
        if (nearAgent(point,agent,world)) utility += 100 * config.getUtilityBeta();
        if (nearWall(point,agent,world)) utility += 100 * config.getUtilityDelta();
        if (tooFarFromAgent(point,agent,world)) utility += 100 * config.getUtilitySigma();
        return utility;
    }

    int WanderAction::nearPeople(Engine::Agent &agent, Engine::World *world) {
        Person &person = dynamic_cast<Person&>(agent);
        Engine::Agent * p_agent = world-> getAgent(agent.getId());
        Engine::AgentsVector neighbours = agent.getWorld()->getNeighbours(p_agent,person.getVision());
        return neighbours.size();
    }

    bool WanderAction::nearWall(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world) {
        int firstI, firstJ, lastI, lastJ;
        firstI = firstJ = lastI = lastJ = 0;
        Person &person = dynamic_cast<Person&>(agent);
        defineLoopBounds(firstI,firstJ,lastI,lastJ,point._x,point._y,person.getWallDistance(),world);
        for (int i = firstI; i <= lastI; i++) {
            for (int j = firstJ; j <= lastJ; j++) {
                Engine::Point2D<int> newPoint = Engine::Point2D<int>(i,j);
                if (point.distance(newPoint) <= person.getWallDistance() and
                    world->getStaticRaster("buildings").getValue(newPoint) == 0) return false;
            }
        }
        return true;
    }

    bool WanderAction::nearAgent(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world) {
        Engine::Agent * p_agent = world->getAgent(agent.getId());
        Person &person = dynamic_cast<Person&>(agent);
        Engine::AgentsVector neighbours = world->getNeighbours(p_agent,person.getAgentDistance());
        for (unsigned int i = 0; i < neighbours.size(); i++) {
            if (point.distance(neighbours[i]->getPosition()) <= person.getAgentDistance()) return false;
        }
        return true;
    }

    bool WanderAction::tooFarFromAgent(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world) {
        Engine::Agent * p_agent = world->getAgent(agent.getId());
        Person &person = dynamic_cast<Person&>(agent);
        Engine::AgentsVector neighbours = world->getNeighbours(p_agent,person.getMaxDistanceBetweenAgents());
        if (neighbours.empty()) return true;
        return false;
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
