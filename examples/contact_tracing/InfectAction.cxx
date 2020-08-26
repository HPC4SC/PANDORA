#include <InfectAction.hxx>
#include <SupermarketConfig.hxx>
#include <Supermarket.hxx>

#include <Exception.hxx>
#include <GeneralState.hxx>

#include <iostream>
#include <vector>

namespace Examples 
{

InfectAction::InfectAction() {}

InfectAction::~InfectAction() {}

void InfectAction::execute(Engine::Agent & agent) {
    HumanBeeing& person = (HumanBeeing&)agent;
    Engine::Agent* p_agent = agent.getWorld()->getAgent(agent.getId());
    Engine::AgentsVector neighbours = person.getWorld()->getNeighbours(p_agent, person.getEncounterRadius(),"all");
    Engine::World* world = agent.getWorld();
    if (neighbours.size() > 0 and person.isSick()) {    
        Engine::AgentsVector::iterator neighbour = neighbours.begin();
        std::cout << person.getId() << " trying to infect: " << neighbours.size() << std::endl;
        while (neighbour != neighbours.end()) {
            Engine::Agent* candidate = neighbour->get();
			HumanBeeing* other = dynamic_cast<HumanBeeing*>(candidate);
            const SupermarketConfig& config = (const SupermarketConfig &) world->getConfig();
            float draw = Engine::GeneralState::statistics().getNormalDistValueMinMax(0.,1.);
            if (not (other->isSick() or other->isInfected()) and not barrier(person,other,world) and draw < config.getInfectiousness()) {
                std::cout << "F " << other->getId() << " infected by: " << person.getId() << std::endl;
                other->getInfected();
                other->setInfectionTime(world->getCurrentStep());
                person.incCountInfected();
            }
			neighbour++;
        }
    }
}

bool InfectAction::barrier(const HumanBeeing& person, const HumanBeeing* other, const Engine::World* world) {
    std::vector<Engine::Point2D<int>> path = getShortestPath(person.getPosition(),other->getPosition());
    Supermarket& super = (Supermarket&)world;
    for (int i = 0; i < path.size(); i++) {
        if (super.isObstacle(path[i])) return true;
    }
    return false;
}

std::vector<Engine::Point2D<int>> InfectAction::getShortestPath(const Engine::Point2D<int> p1, const Engine::Point2D<int> p2) {
    std::vector<Engine::Point2D<int>> result;
    result.push_back(p1);
    Engine::Point2D<int> currentPos = p1;
    while (result.end() == find(result.begin(),result.end(),p2)) {
        Engine::Point2D<int> localBest = Engine::Point2D<int>(currentPos._x - 1,currentPos._y - 1);
        double minDist = localBest.distance(p2);
        for (int i = currentPos._x - 1; i <= currentPos._x + 1; i++) {
            for (int j = currentPos._y - 1; j <= currentPos._y + 1; j++) {
                if (not (i == currentPos._x and j == currentPos._y) and (i != currentPos._x - 1 and j != currentPos._y - 1)) {
                    Engine::Point2D<int> candidate = Engine::Point2D<int>(i,j);
                    double dist = candidate.distance(p2);
                    if (dist < minDist) {
                        localBest = candidate;
                        minDist = dist;
                    }
                }
            }
        }
        result.push_back(localBest);
        currentPos = localBest;
    }
    return result;
}

std::string InfectAction::describe() const {
    return "InfectAction";
}

}