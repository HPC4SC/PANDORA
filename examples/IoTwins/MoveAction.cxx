#include <MoveAction.hxx>
#include <Person.hxx>
#include <EspaiConfig.hxx>

#include <GeneralState.hxx>
#include <DynamicRaster.hxx>
#include <RNGUniformInt.hxx>

#include <vector>
#include <tuple>

namespace Examples {

    MoveAction::MoveAction() {}

    MoveAction::~MoveAction() {}

    void MoveAction::execute(Engine::Agent& agent) {
        //std::cout << "I'm " << agent.getId() <<  " I'm in: "  << agent.getPosition() << std::endl;
        Engine::World *world = agent.getWorld();
        Person &person = dynamic_cast<Person&>(agent);
        Engine::Point2D<int> currentTarget;
        if (person.getTarget().isEqual(Engine::Point2D<int>(-1,-1))) {
            currentTarget = person.getFinalTarget();
        }
        else currentTarget = person.getTarget();
        //std::cout << "I'm " << agent.getId() << " and my currentTarget is: " << currentTarget << " and my position is: " << person.getPosition() << std::endl;
        Engine::Point2D<int> newPosition = selectNextPosition(agent,world,currentTarget); //minDist A*-ish
        //std::cout << "newPosition is: " << newPosition << std::endl;
        if(world->checkPosition(newPosition)) {
            agent.setPosition(newPosition);
            person.addVisited(newPosition);
            //person.printVisited();
            std::cout << "I'm " << agent.getId() << " I'm heading to " << person.getHeading() <<  " and I move to "  << newPosition << " and my finalTarget is " << person.getFinalTarget() << " the distance to the target is: " << newPosition.distance(person.getFinalTarget()) << std::endl;
        }
    }

    Engine::Point2D<int> MoveAction::selectNextPosition(Engine::Agent& agent, Engine::World *world, Engine::Point2D<int>& currentTarget) {
        // pair es el punt i la distancia al finalTarget
        //std::cout << "I'm going to select position" << std::endl;
        std::vector<std::pair<Engine::Point2D<int>, int>> positionsInReach = lookAround(agent,world,currentTarget);
        Person &person = dynamic_cast<Person&>(agent);
        if(positionsInReach.size() > 0) {
            int betterPositionIndex = 0;
            //std::cout << "I'm going to check the positions in reach, size: " << positionsInReach.size() << std::endl;
            int betterPositionPriority = positionsInReach[0].second;
            for (unsigned int i = 1; i < positionsInReach.size(); i++) {
                //std::cout << "I consider position: " << positionsInReach[i].first << std::endl;
                if (positionsInReach[i].second != -1 and (not person.haveVisited(positionsInReach[i].first))) {
                    if (positionsInReach[i].second < betterPositionPriority) {
                        betterPositionIndex = i;
                        betterPositionPriority = positionsInReach[i].second;
                    }
                }
            }
            Engine::Point2D<int> newPosition = positionsInReach[betterPositionIndex].first;
            //std::cout << "The better position is: " << newPosition << std::endl;
            if (newPosition.isEqual(agent.getPosition())) {
                Engine::RNGUniformInt distr = Engine::RNGUniformInt(person.getEspai()->getSeedRun(),0,positionsInReach.size() - 1);
                newPosition = positionsInReach[distr.draw()].first;
                //std::cout << "I selected " << newPosition << " at random" << std::endl;
            }
            return newPosition;
        }
        //std::cout << "No positions in reach" << std::endl;
        return person.getPosition();
        //if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 1) person.turnHeadingLeft();
        //else person.turnHeadingRight();
        //selectNextPosition(agent, world);
    }

    std::vector<std::pair<Engine::Point2D<int>, int>> MoveAction::lookAround(Engine::Agent &agent,
            Engine::World *world, Engine::Point2D<int>& currentTarget) {
       // std::cout << "I'm going to look around" << std::endl;
        std::vector<std::pair<Engine::Point2D<int>, int>> positionsInReach;
        Person &person = dynamic_cast<Person&>(agent);
        Engine::Point2D<int> currentPosition = person.getPosition();
        int firstI, firstJ, lastI, lastJ;
        firstI = firstJ = lastI = lastJ = 0;
        defineLoopBounds(firstI,firstJ,lastI,lastJ,currentPosition._x,currentPosition._y,person.getVision(),world);
        for (int i = firstI; i <= lastI; i++) {
            for (int j = firstJ; j <= lastJ; j++) {
               // std::cout << "The point is: " << i << "," << j << std:: endl;
                //if (validPosition(i,j,person)) {
                   // std::cout << "The point is valid" << std::endl;
                    Engine::Point2D<int> point = Engine::Point2D<int>(i,j);
                    if (point == person.getFinalTarget()) {
                        std::pair<Engine::Point2D<int>, int> newPoint(point,0);
                        positionsInReach.push_back(newPoint);
                        break;
                    }
                    if (world->getStaticRaster("buildings").getValue(point) == 1) {
                        std::pair<Engine::Point2D<int>, int> newPoint(point,assignPriority(point,agent,world,currentTarget));
                        positionsInReach.push_back(newPoint);
                    }
                //}
               // std::cout << "the point is not valid" << std::endl;
            }
        }
        return positionsInReach;
    }

    int MoveAction::assignPriority(Engine::Point2D<int> point, Engine::Agent& agent, Engine::World* world, Engine::Point2D<int>& currentTarget) {
        const EspaiConfig& config = (const EspaiConfig &) world->getConfig();
        int priority = int(point.distance(currentTarget)) * config.getCostAlpha();
        int iniPrio = priority;
        if (nearAgent(point,agent,world)) priority += int(iniPrio * config.getCostBeta());
        if (nearWall(point,agent,world) and not targetNearWall(agent,world)) priority += int(iniPrio * config.getCostDelta());
        if (tooFarFromAgent(point,agent,world)) priority += int(iniPrio * config.getCostSigma());
        return priority;
    }

    bool MoveAction::nearWall(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world) {
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

    bool MoveAction::nearAgent(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world) {
        Engine::Agent * p_agent = world->getAgent(agent.getId());
        Person &person = dynamic_cast<Person&>(agent);
        Engine::AgentsVector neighbours = world->getNeighbours(p_agent,person.getAgentDistance());
        for (unsigned int i = 0; i < neighbours.size(); i++) {
            if (point.distance(neighbours[i]->getPosition()) <= person.getAgentDistance()) return false;
        }
        return true;
    }

    bool MoveAction::tooFarFromAgent(const Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world) {
        Engine::Agent * p_agent = world->getAgent(agent.getId());
        Person &person = dynamic_cast<Person&>(agent);
        Engine::AgentsVector neighbours = world->getNeighbours(p_agent,person.getMaxDistanceBetweenAgents());
        if (neighbours.empty()) return true;
        return false;
    }

    bool MoveAction::targetNearWall(Engine::Agent& agent, Engine::World *world) {
        int firstI, firstJ, lastI, lastJ;
        firstI = firstJ = lastI = lastJ = 0;
        Person &person = dynamic_cast<Person&>(agent);
        Engine::Point2D<int> point = person.getFinalTarget();
        defineLoopBounds(firstI,firstJ,lastI,lastJ,point._x,point._y,person.getWallDistance(),world);
        for (int i = firstI; i <= lastI; i++) {
            for (int j = firstJ; j <= lastJ; j++) {
                Engine::Point2D<int> newPoint = Engine::Point2D<int>(i,j);
                if (point.distance(newPoint) <= person.getWallDistance() and
                    world->getStaticRaster("buildings").getValue(newPoint) == 0) return true;
            }
        }
        return false;
    }

    void MoveAction::defineLoopBounds(int &firstI,int &firstJ, int &lastI, int &lastJ, const int &posX, const int &posY,
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

    std::string MoveAction::describe() const {
        return "MoveAction";
    }

}