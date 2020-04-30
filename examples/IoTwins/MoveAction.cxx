#include <MoveAction.hxx>
#include <Person.hxx>
#include <GeneralState.hxx>
#include <DynamicRaster.hxx>
#include <vector>
#include <tuple>

namespace Examples {

    MoveAction::MoveAction() {}

    MoveAction::~MoveAction() {}

    void MoveAction::execute(Engine::Agent&agent) {
        //std::cout << "I'm " << agent.getId() <<  " I'm in: "  << agent.getPosition() << std::endl;
        Engine::World *world = agent.getWorld();
        Person &person = dynamic_cast<Person&>(agent);
        Engine::Point2D<int> newPosition = selectNextPosition(agent,world); //minDist A*-ish
        if(world->checkPosition(newPosition)) {
            agent.setPosition(newPosition);
            person.addVisited(newPosition);
            person.printVisited();
            std::cout << "I'm " << agent.getId() << " I'm heading to " << person.getHeading() <<  " and I move to "  << newPosition << " and my finalTarget is " << person.getFinalTarget() << " the distance to the target is: " << newPosition.distance(person.getFinalTarget()) << std::endl;
        }
    }

    Engine::Point2D<int> MoveAction::selectNextPosition(Engine::Agent &agent, Engine::World *world) {
        // pair es el punt i la distancia al finalTarget
        //std::cout << "I'm going to select position" << std::endl;
        std::vector<std::pair<Engine::Point2D<int>, int>> positionsInReach = lookAround(agent,world);
        Person &person = dynamic_cast<Person&>(agent);
        if(positionsInReach.size() > 0) {
            int betterPositionIndex = 0;
            //std::cout << "I'm going to check the positions in reach, size: " << positionsInReach.size() << std::endl;
            int betterPositionPriority = positionsInReach[0].second;
            for (int i = 1; i < positionsInReach.size(); i++) {
                //std::cout << "I consider position: " << positionsInReach[i].first << std::endl;
                if (positionsInReach[i].second != -1 and (not person.haveVisited(positionsInReach[i].first))) {
                    if (positionsInReach[i].second < betterPositionPriority) {
                        betterPositionIndex = i;
                        betterPositionPriority = positionsInReach[i].second;
                    }
                }
            }
            Engine::Point2D<int> newPosition = positionsInReach[betterPositionIndex].first;
            if (newPosition.isEqual(agent.getPosition())) {
                newPosition = positionsInReach[Engine::GeneralState::statistics().getUniformDistValue(0,positionsInReach.size() -1)].first;
                //std::cout << "I selected " << newPosition << " at random" << std::endl;
            }
            return newPosition;
        }
        //if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 1) person.turnHeadingLeft();
        //else person.turnHeadingRight();
        selectNextPosition(agent, world);
    }

    std::vector<std::pair<Engine::Point2D<int>, int>> MoveAction::lookAround(Engine::Agent &agent,
            Engine::World *world) {
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
                        std::pair<Engine::Point2D<int>, int> newPoint(point,assignPriority(point,agent,world));
                        positionsInReach.push_back(newPoint);
                    }
                //}
               // std::cout << "the point is not valid" << std::endl;
            }
        }
        return positionsInReach;
    }

    int MoveAction::assignPriority(Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world) {
        Person &person = dynamic_cast<Person&>(agent);
        int priority = int(point.distance(person.getFinalTarget()));
        //if (nearAgent(point,agent,world)) priority += 100 * 0.01;
        //if (nearWall(point,agent,world) and not targetNearWall(agent,world)) priority += 1;
        //if (tooFarFromAgent(point,agent,world)) priority +=1;
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
        for (int i = 0; i < neighbours.size(); i++) {
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

    bool MoveAction::targetNearWall(Engine::Agent &agent, Engine::World *world) {
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

    bool MoveAction::validPosition(const int& i, const int& j, Person& person) {
        /*if (i == j) return false;
        if (pointAlignedWithHeading(i,j,person)) return true;
        else return false;*/
    }

    bool MoveAction::pointAlignedWithHeading(const int& i, const int&j, Person& person) {
        /*Engine::Point2D<int> point = person.getPosition();
        int heading = person.getHeading();
        switch(heading) {
            case 0: {
                return (i <= point._x and j <= point._y);
                break;
            }
            case 1: {
                return i < point._x;
                break;
            }
            case 2: {
                return (i <= point._x and j >= point._y);
                break;
            }
            case 3: {
                return j < point._y;
                break;
            }
            case 4: {
                return j > point._y;
                break;
            }
            case 5: {
                return (i >= point._x and j <= point._y);
                break;
            }
            case 6: {
                return i > point._x;
                break;
            }
            case 7: {
                return (i >= point._x and j >= point._y);
                break;
            }
        }*/
    }

    std::string MoveAction::describe() const {
        return "MoveAction";
    }

}