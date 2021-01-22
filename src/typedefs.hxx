
#ifndef __PandoraTypedefs_hxx__
#define __PandoraTypedefs_hxx__

#include <list>
#include <vector>
#include <memory>
#include <map>
#include <Rectangle.hxx>

namespace Engine
{
    /** Agent typedefs **/

    class Agent;

    typedef std::shared_ptr<Agent> AgentPtr;
    typedef std::list< AgentPtr > AgentsList;
    typedef std::vector< AgentPtr > AgentsVector;
    typedef std::map<std::string, AgentPtr> AgentsMap;
    typedef std::vector<std::vector<AgentsMap>> AgentsMatrix;

    /** Scheduler typedefs **/
    struct MPINode {
        Rectangle<int> ownedAreaWithoutInnerOverlap;                //! Area of this node without inner (this node)   overlaps. // Filled up to depth 1 (from neighbours->second).
        Rectangle<int> ownedArea;                                   //! Area of this node with    inner (this node)   overlaps. // Filled up to depth 1 (from neighbours->second).
        Rectangle<int> ownedAreaWithOuterOverlap;                   //! Area of this node with    outer (other nodes) overlaps. // Filled up to depth 1 (from neighbours->second).
        std::map<int, MPINode*> neighbours;                         //! Map<neighbouringNodeId, neighbouringNodeSpaces> containing the neighbours information for communication. // Filled up to depth 0 (from neighbours->second).

        std::map<int, Rectangle<int>> innerSubOverlaps;             //! Sub-overlaps (Sub areas of the inner overlap). Should be 8 in total for mode9 and 4 for mode4. Map<subOverlapID, subOverlapArea>, where subOverlapID = Engine::SubOverlapType enum. // Filled up to depth 0 (from neighbours->second).
        std::map<int, std::list<int>> innerSubOverlapsNeighbours;   //! Sub-overlaps neighbouring nodes. Map<subOverlapID, list<nodeID>>. Used for efficient agents and rasters communication. // Filled up to depth 0 (from neighbours->second).

        void reset() 
        {
            ownedAreaWithoutInnerOverlap = Rectangle<int>();
            ownedArea = Rectangle<int>();
            ownedAreaWithOuterOverlap = Rectangle<int>();
            neighbours.clear();     // ToDo: this could incur memory problems. We should delete this pointers recursively.

            innerSubOverlaps.clear();
            innerSubOverlapsNeighbours.clear();
        }
    };

    typedef std::map<int, MPINode> MPINodesMap;

    typedef std::map<Point2D<int>, int> MapOfPositionsAndValues;
    typedef std::map<int, MapOfPositionsAndValues> MapOfValuesByRaster;

    /** List of Mpi messages sent by the scheduler **/
    enum MpiMessageType
    {
        eNumProcesses = 1,
        eCoordinates = 2,
        eCoordinatesAux = 3,
        eNumNeighbours = 4,
        eNeighbourID = 5,
        eCoordinatesNeighbour = 6,

        eNumTypes = 10,
        eNumAgents = 11,
        eAgents = 12,
        eAgentsTypeID = 13,
        eNumCoordinates = 14,
        eNumCoordinatesAux = 15,

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
        eGhostAgentsComplexAttributesNumBytes = 44,
        eGhostAgentsComplexAttributes = 45,

        eNumRasters = 50,
        eRasterIndex = 51,
        eNumRasterPositions = 52,
        ePosAndValue = 53,

        eRasterData = 60,
        eRasterMaxData = 61,
        eVectorAttribute = 62,
        eSizeVector = 63,
        eNumModifiedAgents = 64,
        eModifiedAgent = 65,

        eCreateGroupActive = 70,
        eCreateGroupInactive = 71,
        eProcessWakeUp = 72,
        eProcessSleep = 73,
        eSendAgentPhasesTotalTime = 74,
        eCheckToRepartition = 75,
        ePrepareToRepartition = 76,
        eTypeOfEventAfterWakeUp = 77,
        eNumberOfProcessesAndStep = 78,

        eAgentPhasesTotalTime = 80
    };

    enum TypeOfEventAfterWakeUpMPI
    {
        eMessage_Die = 100,
        eMessage_SendAgentPhasesTotalTime_true = 101,
        eMessage_SendAgentPhasesTotalTime_false = 102,
        eMessage_CheckToRepartition_true = 103,
        eMessage_CheckToRepartition_false = 104,
        eMessage_PrepareToRepartition_true = 105,
        eMessage_PrepareToRepartition_false = 106,
        eMessage_AwakeToRepartition = 107
    };

    enum ExecutingPhaseType
    {
        eUpdateKnowledge = 1,
        eSelectActions = 2,
        eExecuteActions = 3,
        eUpdateState = 4
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

    enum ComplexAttributesTags
    {
        eTypeOfDS_vector = 1,
        eTypeOfDS_list = 2,
        eTypeOfDS_queue = 3,
        eTypeOfDS_map = 4,
        eTypeOfDS_point2D = 5,

        eTypeOfElement_int = 6,
        eTypeOfElement_float = 7,
        eTypeOfElement_bool = 8,
        eTypeOfElement_string = 9,
        eTypeOfElement_point2D = 10
    };

} // namespace Engine

#endif // __PandoraTypedefs_hxx__

