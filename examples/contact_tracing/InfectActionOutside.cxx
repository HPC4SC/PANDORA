#include <InfectActionOutside.hxx>

#include <Exception.hxx>
#include <GeneralState.hxx>
#include<RNGUniformDouble.hxx>

#include <iostream>
#include <vector>
#include <cstdlib>

namespace Examples 
{

InfectActionOutside::InfectActionOutside() {}

InfectActionOutside::~InfectActionOutside() {}

void InfectActionOutside::execute(Engine::Agent & agent) {
    HumanBeeing& person = (HumanBeeing&)agent;
    Engine::Agent* p_agent = agent.getWorld()->getAgent(agent.getId());
    Engine::AgentsVector neighbours = person.getWorld()->getNeighbours(p_agent, person.getEncounterRadius(),"all");
    Engine::World* world = agent.getWorld();
    if (neighbours.size() > 0 and person.isSick()) {
        Engine::AgentsVector::iterator neighbour = neighbours.begin();
        while (neighbour != neighbours.end()) {
            Engine::Agent* candidate = neighbour->get();
			HumanBeeing* other = dynamic_cast<HumanBeeing*>(candidate);
            double draw = Engine::RNGUniformDouble(agent.getWorld()->getConfig().getSeed(),0.,1.).draw();
            if (not (other->isSick() or other->isInfected()) and draw < person.getInfectiousness()) {
                std::cout << "F " << person.getId() << " infected " << other->getId() << std::endl;
                other->getInfected();
                other->setInfectionTime(world->getCurrentStep());
                person.incCountInfected();
            }
			neighbour++;
        }
    }
}

std::string InfectActionOutside::describe() const {
    return "InfectActionOutside";
}

}