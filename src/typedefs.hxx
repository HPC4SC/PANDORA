
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
        eNumAgents = 1,
        eAgent = 2,
        eAgentTypeID = 11,
        eNumberOfStaticRasters = 12,
        eNumberOfDynamicRasters = 13,
        eStaticRasterID = 14,
        eStaticRasterName = 15,
        eStaticRasterSerialize = 16,
        eStaticRasterFileName = 17,
        eDynamiRasterID = 18,
        eDynamicRasterName = 19,
        eDynamicRasterSerialize = 20,
        eDynamicRasterMinValue = 21,
        eDynamicRasterMaxValue = 22,
        eDynamicRasterDefaultValue = 23,


        eNumGhostAgents = 3,
        eGhostAgent = 4,
        eRasterData = 5,
        eRasterMaxData = 6,
        eVectorAttribute = 7,
        eSizeVector = 8,
        eNumModifiedAgents = 9,
        eModifiedAgent = 10
    };

    enum MpiMessageTypeSpaces
    {
        eCoordinates = 1,
        eNumNeighbours = 2,
        eNeighbourID = 3,
        eCoordinatesNeighbour = 4
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

