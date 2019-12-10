#ifndef __DifferentSpaces_hxx__
#define __DifferentSpaces_hxx__

#include <mpi.h>
#include <World.hxx>
#include <typedefs.hxx>
#include <Serializer.hxx>
#include <list>
#include <vector>
#include <Scheduler.hxx>

namespace Engine {
    class Agent;

    class DifferentSpaces : public Scheduler {

        SequentialSerializer _serializer;

        double _initialTime;

        AgentsList _removedAgents;

        AgentsList::iterator getAgentIterator(const std::string &id);

        std::vector <Rectangle<int>> _spaces;

        Rectangle<int> _ownedArea;

        Point2D<int> _worldPos;

    public:
        DifferentSpaces();

        ~DifferentSpaces();

        void init(int argc, char *argv[]);

        void initData();

        virtual void executeAgents();

        void stablishBoundaries();

        void removeAgents();

        void removeAgent(Agent *agent);

        Agent *getAgent(const std::string &id);

        AgentsVector getAgent(const Point2D<int> &position, const std::string &type = "all");

        void defineBoundaries();

        double getWallTime() const;

        size_t getNumberOfTypedAgents(const std::string &type) const;
    };
}