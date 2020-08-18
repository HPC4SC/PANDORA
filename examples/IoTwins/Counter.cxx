#include<Counter.hxx>
#include <GeneralState.hxx>

namespace Examples {
    Counter::Counter(const std::string& id) :  Agent(id), _agentCount(0) {}

    Counter::~Counter() {}

    void Counter::updateKnowledge() {
        _agentCount += this->getWorld()->countNeighbours(this,1); 
    }

    void Counter::registerAttributes() {
        registerIntAttribute("agentCount");
    }

    void Counter::serialize() {
        serializeAttribute("agentCount",_agentCount);
    }

}