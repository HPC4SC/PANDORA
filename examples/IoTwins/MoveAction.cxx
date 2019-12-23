#include <MoveAction.hxx>
#include <Person.hxx>
#include <GeneralState.hxx>
#include <DynamicRaster.hxx>
#include <vector>
#include <tuple>

namespace Examples {

    MoveAction::MoveAction() {
    }

    MoveAction::~MoveAction() {
    }

    void MoveAction::execute(Engine::Agent&agent) {
        Engine::World *world = agent.getWorld();
        Engine::Point2D<int> newPosition = selectNextPositon(agent,world);
        std::cout << "position al acabar: (" << newPosition << ")" << std::endl;
        if(world->checkPosition(newPosition)) agent.setPosition(newPosition);
    }

    Engine::Point2D<int> MoveAction::selectNextPositon(Engine::Agent &agent, Engine::World *world) {
        // pair es el punt i la distancia al finalTarget
        std::vector<std::pair<Engine::Point2D<int>, int>> positionsInReach = lookAround(agent,world);
        int betterPositionIndex = 0;
        int betterPositionDistance = 0;
        for (int i = 0; i < positionsInReach.size(); i++) {
            if (positionsInReach[i].second != -1) {
                if (positionsInReach[i].second < betterPositionDistance) {
                    betterPositionIndex = i;
                    betterPositionDistance = positionsInReach[i].second;
                }
            }
        }
        Engine::Point2D<int> newPosition = positionsInReach[betterPositionIndex].first;
        return newPosition;
    }

    std::vector<std::pair<Engine::Point2D<int>, int>> MoveAction::lookAround(Engine::Agent &agent,
            Engine::World *world) {
        std::vector<std::pair<Engine::Point2D<int>, int>> positionsInReach;
        Person &person = dynamic_cast<Person&>(agent);
        Engine::Point2D<int> currentPosition = person.getPosition();
        std:: cout << "el meu target es: (" << person.getFinalTarget() << ")" << std::endl;
        std::cout << "position abans de començar: (" << currentPosition << ")" << std::endl;
        int firstI, firstJ, lastI, lastJ;
        firstI = firstJ = lastI = lastJ = 0;
        defineLoopBounds(firstI,firstJ,lastI,lastJ,currentPosition._x,currentPosition._y,person.getVision(),world);
        for (int i = firstI; i < lastI; i++) {
             for (int j = firstJ; j < lastJ; j++) {
                 if (i != j) {
                     Engine::Point2D<int> point = Engine::Point2D<int>(i,j);
                     if (world->getStaticRaster("buildings").getValue(point) == 1) {
                         std::pair<Engine::Point2D<int>, int> newPoint(point,assignPriority(point,agent,world));
                         positionsInReach.push_back(newPoint);
                     } else {
                         std::pair<Engine::Point2D<int>, int> newPoint(point,-1);
                         positionsInReach.push_back(newPoint);
                     }
                 }
             }
        }
        return positionsInReach;
    }

    int MoveAction::assignPriority(Engine::Point2D<int> point, Engine::Agent &agent, Engine::World *world) {
        Person &person = dynamic_cast<Person&>(agent);
        int priority = point.distance(person.getFinalTarget());
        if (nearWall(point,agent,world)) priority += 1;
        else if (nearAgent(point,agent,world)) priority += 1;
        return priority;
    }

    bool MoveAction::nearWall(const Engine::Point2D<int> point, const Engine::Agent &agent, Engine::World *world) {
        int firstI, firstJ, lastI, lastJ;
        firstI = firstJ = lastI = lastJ = 0;
        defineLoopBounds(firstI,firstJ,lastI,lastJ,point._x,point._y,1,world);
        for (int i = firstI; i < lastI; i++) {
            for (int j = firstJ; j < lastJ; j++) {
                Engine::Point2D<int> newPoint = Engine::Point2D<int>(i,j);
                if (point.distance(newPoint) == 1 and world->getStaticRaster("buildings").getValue(newPoint) == 0) {
                    return false;
                }
            }
        }
        return true;
    }

    bool MoveAction::nearAgent(const Engine::Point2D<int> point, const Engine::Agent &agent, Engine::World *world) {
        Engine::Agent * p_agent = world->getAgent(agent.getId());
        Engine::AgentsVector neighbours = world->getNeighbours(p_agent, 1);
        for (int i = 0; i < neighbours.size(); i++) {
            if (point.distance(neighbours[i]->getPosition()) <= 1) return false;
        }
        return true;
    }

    void MoveAction::defineLoopBounds(int &firstI,int &firstJ, int &lastI, int &lastJ, const int &posX, const int &posY,
            const int &capability, Engine::World *world) {
        Engine::Rectangle<int> boundaries = world->getBoundaries();
        firstI = posX - capability;
        firstJ = posY - capability;
        lastI = posX + capability;
        lastJ = posY + capability;
        if (firstI < boundaries.left()) firstI = boundaries.left();
        else if (firstJ < boundaries.top()) firstJ = boundaries.top();
        else if (lastI > boundaries.right()) lastI = boundaries.right();
        else if (lastJ > boundaries.bottom()) lastJ = boundaries.bottom();
    }

    std::string MoveAction::describe() const {
        return "MoveAction";
    }

}