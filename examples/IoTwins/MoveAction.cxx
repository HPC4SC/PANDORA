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
        if(world->checkPosition(newPosition)) agent.setPosition(newPosition);
    }

    Engine::Point2D<int> MoveAction::selectNextPositon(Engine::Agent &agent, Engine::World *world) {
        // pair es el punt i la distancia al finalTarget
        std::vector<std::pair<Engine::Point2D<int>, int>> positionsInReach = calculateDistances(agent,world);
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
        std::cout << "I'm in: (" << newPosition._x << "," << newPosition._y << ") after moving" << std::endl;
        return newPosition;
    }

    std::vector<std::pair<Engine::Point2D<int>, int>> MoveAction::calculateDistances(Engine::Agent &agent,
            Engine::World *world) {
        std::vector<std::pair<Engine::Point2D<int>, int>> positionsInReach;
        Person &person = dynamic_cast<Person&>(agent);
        Engine::Point2D<int> newPosition = person.getPosition();
        std::cout << "I'm in: (" << newPosition._x << "," << newPosition._y << ") before moving" << std::endl;
        int firstI = 0;
        int firstJ = 0;
        int lastI = 0;
        int lastJ = 0;
        defineLoopBounds(firstI,firstJ,lastI,lastJ,newPosition._x,newPosition._y,person.getVelocity(),world);
        for (int i = firstI; i < lastI; i++) {
            for (int j = firstJ; j < lastJ; j++) {
                if (i != j) {
                    Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
                    if (world->getStaticRaster("map").getValue(candidate) == 1) {
                        std::pair<Engine::Point2D<int>, int> point(candidate,candidate.distance(person.getFinalTarget()));
                        positionsInReach.push_back(point);
                    } else {
                        std::pair<Engine::Point2D<int>, int> point(candidate,-1);
                        positionsInReach.push_back(point);
                    }
                }
            }
        }
        return positionsInReach;
    }

    void MoveAction::defineLoopBounds(int &firstI,int &firstJ, int &lastI, int &lastJ, const int &posX, const int &posY,
            const int &velocity, Engine::World *world) {
        Engine::Rectangle<int> boundaries = world->getBoundaries();
        firstI = posX - velocity;
        firstJ = posY - velocity;
        lastI = posX + velocity;
        lastJ = posY + velocity;
        if (firstI < boundaries.left()) firstI = boundaries.left();
        else if (firstJ < boundaries.top()) firstJ = boundaries.top();
        else if (lastI > boundaries.right()) lastI = boundaries.right();
        else if (lastJ > boundaries.bottom()) lastJ = boundaries.bottom();
    }

    std::string MoveAction::describe() const {
        return "MoveAction";
    }

}