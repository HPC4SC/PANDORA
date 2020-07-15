
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

        eNumGhostAgents = 40,
        eGhostAgentType = 41,
        eGhostAgent = 42,



        eRasterData = 5,
        eRasterMaxData = 6,
        eVectorAttribute = 7,
        eSizeVector = 8,
        eNumModifiedAgents = 9,
        eModifiedAgent = 10
    };

    enum SubOverlapType
    {
        eTopCenter = 1,
        eBottomCenter = 2,
        eLeftCenter = 3,
        eRightCenter = 4,
        eTopLeft = 5,
        eTopRight = 6,
        eBottomLeft = 7,
        eBottomRight = 8,
        eNumberOfSubOverlaps = 8
    };

} // namespace Engine

#endif // __PandoraTypedefs_hxx__

