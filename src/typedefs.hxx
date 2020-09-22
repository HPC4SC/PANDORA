
#ifndef __PandoraTypedefs_hxx__
#define __PandoraTypedefs_hxx__

#include <list>
#include <vector>
#include <memory>

namespace Engine
{

    class Agent;

    typedef std::shared_ptr<Agent> AgentPtr;
    typedef std::list< AgentPtr > AgentsList;
    typedef std::vector< AgentPtr > AgentsVector;
    typedef std::map<std::string, AgentPtr> AgentsMap;
    typedef std::vector<std::vector<AgentsMap>> AgentsMatrix;

    //! List of Mpi messages sent by the scheduler
    enum MpiMessageType
    {
        eCoordinates = 1,
        eNumNeighbours = 2,
        eNeighbourID = 3,
        eCoordinatesNeighbour = 4,

        eNumTypes = 10,
        eNumAgents = 11,
        eAgent = 12,
        eAgentTypeID = 13,

        eNumberOfStaticRasters = 20,
        eNumberOfDynamicRasters = 21,
        eStaticRasterID = 22,
        eStaticRasterName = 23,
        eStaticRasterSerialize = 24,
        eStaticRasterFileName = 25,
        eDynamiRasterID = 26,
        eDynamicRasterName = 27,
        eDynamicRasterSerialize = 28,
        eDynamicRasterMinValue = 29,
        eDynamicRasterMaxValue = 30,
        eDynamicRasterDefaultValue = 31,

        eNumGhostAgentsType = 40, 
        eGhostAgentsType = 41,
        eNumGhostAgents = 42,
        eGhostAgents = 43,

        eNumRasters = 50,
        eRasterIndex = 51,
        eNumRasterPositions = 52,
        ePosAndValue = 53,


        eRasterData = 5,
        eRasterMaxData = 6,
        eVectorAttribute = 7,
        eSizeVector = 8,
        eNumModifiedAgents = 9,
        eModifiedAgent = 10
    };

    enum SubOverlapType
    {
        eTopLeft_Mode4 = 41,
        eTopRight_Mode4 = 42,
        eBottomLeft_Mode4 = 43,
        eBottomRight_Mode4 = 44,

        eTopCenter_Mode9 = 91,
        eBottomCenter_Mode9 = 92,
        eLeftCenter_Mode9 = 93,
        eRightCenter_Mode9 = 94,
        eTopLeft_Mode9 = 95,
        eTopRight_Mode9 = 96,
        eBottomLeft_Mode9 = 97,
        eBottomRight_Mode9 = 98
    };

    enum SubpartitioningMode
    {
        eMode4 = 4,
        eMode9 = 9
    };

} // namespace Engine

#endif // __PandoraTypedefs_hxx__

