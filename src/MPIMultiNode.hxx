/*
 * Copyright ( c ) 2014
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACI-N
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software;
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or ( at your option ) any later version.
 *
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MPIMultiNode_hxx__
#define __MPIMultiNode_hxx__

#include <World.hxx>
#include <Scheduler.hxx>
#include <MPILoadBalanceTree.hxx>
#include <MPIMultiNodeLogs.hxx>
#include <MPIAutoAdjustment.hxx>
#include <Serializer.hxx>

#include <set>
#include <mpi.h>

namespace Engine
{

    class MPILoadBalanceTree;
    class MPIMultiNodeLogs;
    class MPIAutoAdjustment;

    class MPIMultiNode : public Scheduler
    {

        public:

            struct MPINode {
                Rectangle<int> ownedAreaWithoutInnerOverlap;                //! Area of this node without inner (this node)   overlaps. // Filled up to depth 1 (from neighbours->second).
                Rectangle<int> ownedArea;                                   //! Area of this node with    inner (this node)   overlaps. // Filled up to depth 1 (from neighbours->second).
                Rectangle<int> ownedAreaWithOuterOverlaps;                  //! Area of this node with    outer (other nodes) overlaps. // Filled up to depth 1 (from neighbours->second).
                std::map<int, MPINode*> neighbours;                         //! Map<neighbouringNodeId, neighbouringNodeSpaces> containing the neighbours information for communication. // Filled up to depth 0 (from neighbours->second).

                std::map<int, Rectangle<int>> innerSubOverlaps;             //! Sub-overlaps (Sub areas of the inner overlap). Should be 8 in total. Map<subOverlapID, subOverlapArea>, where subOverlapID = Engine::SubOverlapType enum. // Filled up to depth 0 (from neighbours->second).
                std::map<int, std::list<int>> innerSubOverlapsNeighbours;   //! Sub-overlaps neighbouring nodes. Map<subOverlapID, list<nodeID>>. Used for efficient agents and rasters communication. // Filled up to depth 0 (from neighbours->second).
            };

            typedef std::map<int, MPINode> MPINodesMap;

            typedef std::map<Point2D<int>, int> MapOfPositionsAndValues;
            typedef std::map<int, MapOfPositionsAndValues> MapOfValuesByRaster;

            friend class MPIMultiNodeLogs;
            friend class MPIAutoAdjustment;

        protected:

            /** Base data structures for the space partitioning (master node only) **/
            MPILoadBalanceTree* _loadBalanceTree;                           //! Tree used to divide the space and balance the load of the simulation.
            MPIAutoAdjustment* _autoAdjustment;                             //! Instance for the autoadjustment in the number of used nodes.

            MPINodesMap _mpiNodesMapToSend;                                 //! Map<nodeId, nodeInformation> containing the leafs of _loadBalancetree, where the 'value' field is now the 'ownedArea', and the IDs of the 'neighbours' are mapped with their coordinates.

            /** Node own data structures (nodes0..n) **/
            MPINode _nodeSpace;                                             //! Areas and neighbours information for this node.

            std::set<std::string> _executedAgentsInStep;                    //! Set containing the IDs of the agents that have been already executed in the current step.

            std::vector<std::pair<int, Point2D<int>>> _changedRastersCells; //! Raster cells changed in the current step.

            /** MPI Data Structures **/
            int _masterNodeID;                                              //! ID of the master node. Used for communication.

            int _numberOfActiveProcesses;                                   //! Current number of active processes.
            bool _justAwaken, _justFinished;
            MPI_Comm _activeProcessesComm;                                  //! Communicator for active MPI processes.

            struct Coordinates {
                int top, left, bottom, right;
            };                                                              //! Struct used to parse in/out the to-be-send/received coordinates.
            MPI_Datatype* _coordinatesDatatype;                             //! Own MPI Datatype used to send/receive the coordinates of a neighbouring node.

            struct PositionAndValue {
                int x, y, value;
            };                                                              //! Struct used to parse in/out the to-be-send/received raster positions and values.
            MPI_Datatype* _positionAndValueDatatype;                        //! Own MPI Datatype used to send/receive the positions and values of an updated raster.

            std::list<MPI_Request*> _sendRequests;

            /** Other structures **/
            MPIMultiNodeLogs* _schedulerLogs;

            std::vector<Agent*> _agentsToBeRemoved;                         //! Vector or Agents to be removed at the end of the step.

            double _initialTime;                                            //! Initial running time.

            Serializer _serializer;                                         //! Serializer instance.

            /** INITIALIZATION PROTECTED METHODS **/

            /**
             * @brief Aborts all the MPI processes associated with the current Communicator.
             * 
             */
            void terminateAllMPIProcesses() const;

            /**
             * @brief Creates the names of the log files for each of the MPI processes, appending them in the _logFileNames member.
             * 
             */
            void initLogFileNames();

            /**
             * @brief Initializes the _loadBalanceTree instance.
             * 
             */
            void initLoadBalanceTree();

            /**
             * @brief Initializes the _autoAdjustment instance.
             * 
             */
            void initAutoAdjustment();

            /**
             * @brief Used just to initially stablish the _boundaries and the _ownedArea members, needed to let the model to first create the agents.
             * 
             */
            void stablishInitialBoundaries();

            /**
             * @brief Registers the data structs to be send/received to/from the nodes.
             * 
             */
            void registerMPIStructs();

            /**
             * @brief Calls the creation of the rasters.
             * 
             */
            void createInitialRasters();

            /**
             * @brief Calls the creation of the agents.
             * 
             */
            void createInitialAgents();

            /**
             * @brief Enables 'numberOfProcessesToEnable' processes as active processes, sorted by their rank. To do so, it updates the active processes communicator '_activeProcessesComm'.
             * 
             * @param numberOfProcessesToEnable const int&
             */
            void enableOnlyProcesses(const int& numberOfProcessesToEnable);

            /**
             * @brief Puts the calling process to sleep, waiting for an awake signal from the master node 'masterNodeID'.
             * 
             * @param masterNodeID const int&
             */
            void waitSleeping(const int& masterNodeID);

            /**
             * @brief It creates the binary tree '_root' representing the partitions of the world for each of the MPI tasks. Besides, it creates the nodes structs to be send to each one of the slaves.
             * 
             */
            void divideSpace();

            /**
             * @brief It fills up own structures for _masterNodeID and sends the created spaces to the rest of MPI processes if necessray (_numTasks > 1).
             * 
             * @param masterNodeID const int&
             */
            void sendInitialSpacesToNodes(const int& masterNodeID);

            /**
             * @brief It receives the created spaces from the MPI master node identified by 'masterNodeID'.
             * 
             * @param const int&
             */
            void receiveInitialSpacesFromNode(const int& masterNodeID);

            /**
             * @brief Generates and fills the 'mpiNode'.ownedAreaWithoutInnerOverlap and the 'mpiNode'.ownedAreaWithOuterOverlaps, based on the 'mpiNode'.ownedArea.
             * 
             * @param mpiNode MPINode&
             */
            void generateOverlapAreas(MPINode& mpiNode) const;

            /**
             * @brief Squeezes 'rectangle' in all four directions according to the parameters and returns it.
             * 
             * @param rectacgle const Rectangle<int>&
             * @param squeezeTop const int&
             * @param squeezeBottom const int&
             * @param squeezeLeft const int&
             * @param squeezeRight const int&
             * @return Rectangle<int> 
             */
            Rectangle<int> squeezeRectangle(const Rectangle<int>& rectangle, const int& squeezeTop, const int& squeezeBottom, const int& squeezeLeft, const int& squeezeRight) const;

            /**
             * @brief Generate inner sub-overlap areas for 'mpiNode'.
             * 
             * @param mpiNode MPINode&
             */
            void generateInnerSubOverlapAreas(MPINode& mpiNode) const;

            /**
             * @brief Fills up the 'mpiNode'.innerSubOverlapsNeighbours list with the neighbouring sub overlaps to 'neighbourNode'.
             * 
             * @param mpiNode MPINode&
             * @param neighbourID const int&
             * @param neighbourNode const MPINode&
             */
            void generateInnerSubOverlapNeighbours(MPINode& mpiNode, const int& neighbourID, const MPINode& neighbourNode) const;

            /**
             * @brief Fills the own structures with the information in 'mpiNodeInfo'.
             * 
             * @param mpiNodeInfo const MPINode&
             */
            void fillOwnStructures(const MPINode& mpiNodeInfo);

            /**
             * @brief It blocking sends the 'nodeCoordinates' to the corresponding 'nodeID'.
             * 
             * @param nodeID const int&
             * @param nodeCoordinates const Rectangle<int>&
             */
            void sendOwnAreaToNode(const int& nodeID, const Rectangle<int>& nodeCoordinates) const;

            /**
             * @brief It blocking sends the 'neighbours' information to 'nodeID', gathering their coordinates from the _mpiNodesMapToSend member.
             * 
             * @param nodeID const int&
             * @param neighbours std::map<int, Rectangle<int>>&
             */
            void sendNeighboursToNode(const int& nodeID, const std::map<int, MPINode*>& neighbours) const;

            /**
             * @brief It blocking receives the own node coordinates from the master node with ID 'masterNodeID'.
             * 
             * @param masterNodeID const int&
             */
            void receiveOwnAreaFromNode(const int& masterNodeID, MPINode& mpiNodeInfo) const;

            /**
             * @brief It blocking receives the 'neighbours' information from 'masterNodeID'.
             * 
             * @param masterNodeID const int&
             */
            void receiveNeighboursFromNode(const int& masterNodeID, MPINode& mpiNodeInfo) const;

            /**
             * @brief Check whether 'agent' is within the 'agentsList'
             * 
             * @param agent const Agent&
             * @param agentsList const AgentsList&
             * @return bool
             */
            bool isAgentInList(const Agent& agent, const AgentsList& agentsList) const;

            /**
             * @brief Removes Agents in 'agentsToRemove' from the node's _world.
             * 
             * @param agentsToRemove const std::vector<Agent*>&
             */
            void removeAgentsInVector(const std::vector<Agent*>& agentsToRemove);

            /**
             * @brief Discards the agents that does not belong to the node calling this method.
             * 
             */
            void filterOutInitialAgents();

            /**
             * @brief Discards the raster cells that does not belong to the node calling this method.
             * 
             */
            void filterOutInitialRasters();

            /**
             * @brief Stablishes boundaries (_boundaries member) for the node calling this method.
             * 
             */
            void stablishBoundaries();

            /**
             * @brief Adds into the _mpiNodeMap the partition <nodeId, partitions[neighbourIndex]>.
             * 
             * @param partitions const std::vector<Rectangle<int>>&
             * @param neighbourIndex const int&
             */
            void addMPINodeToSendInMapItIsNot(const std::vector<Rectangle<int>>& partitions, const int& neighbourIndex);

            /**
             * @brief Creates a rectangle like 'rectangle' expanded exactly 'expansion' cells in all directions (if possible). A 'expansion' < 0 == contraction. If 'contractInTheLimits' == false means that, if 'rectangle' is already in the limits of the simulation space ([_root->value.top(), _root->value.left()] or [_root->value.bottom(), _root->value.right()], then the resulting rectangle will NOT be modified (neither contracted nor expanded). This is normally used only when 'expansion' < 0.
             * 
             * @pre For contractions ('expansion' < 0), 'rectangle'.getSize().getWidth() and 'rectangle'.getSize().getHeight() should be > 2*'expansion'.
             * @param rectangle const Rectangle<int>&
             * @param expansion const int&
             * @param noContractInTheLimits const bool&
             */
            void expandRectangleConsideringLimits(Rectangle<int>& rectangle, const int& expansion, const bool& contractInTheLimits = true) const;

            /**
             * @brief Check whether 'rectangleA' and 'rectangleB' overlap.
             * 
             * @param rectangleA const Rectangle<int>&
             * @param rectangleB const Rectangle<int>&
             * @return bool
             */
            bool doOverlap(const Rectangle<int>& rectangleA, const Rectangle<int>& rectangleB) const;

            /**
             * @brief Check whether areas of influence of 'rectangleA' and 'rectangleB' collides.
             * 
             * @param rectangleA const Rectangle<int>&
             * @param rectangleB const Rectangle<int>&
             * @return bool
             */
            bool areTheyNeighbours(const Rectangle<int>& rectangleA, const Rectangle<int>& rectangleB) const;

            /**
             * @brief Create all the information needed for the processing MPI nodes to properly be executed and communicate with the rest of the nodes.
             * 
             */
            void createNodesInformationToSend();

            /**
             * @brief Check whether each of the created partitions fullfil the first condition of being their widths and heights > 2*_overlapSize.
             * 
             * @return bool
             */
            bool arePartitionsSuitable();

            /** INITIALIZATION DEBUGGING METHODS **/

            /**
             * @brief Prints nodes partitioning and neighbours for each one.
             * 
             */
            void printPartitionsBeforeMPI() const;

            /**
             * @brief Prints the nodes structure (ID + Coordinates) in _nodeSpace.
             * 
             */
            void printOwnNodeStructureAfterMPI() const;

            /**
             * @brief Prints the agents for the current node executing this method.
             * 
             */
            void printNodeAgents() const;

            /**
             * @brief Prints the rasters for the current node executing this method.
             * 
             */
            void printNodeRasters() const;

            /**
             * @brief Prints the active and inactive processes at the current time.
             * 
             */
            void printActiveAndInactiveProcesses() const;

            /** RUN PROTECTED METHODS (CALLED BY INHERIT METHODS) **/

            /**
             * @brief Performs everything that is needed to save the state of agents and rasters among sub-overlap processing chunks.
             * 
             */
            void initializeAgentsAndRastersState();

            /**
             * @brief Updates the average time that agents have spent executing phase of type 'executingPhaseType'.
             * 
             * @param executingPhaseType const int&
             * @param initialTime const double&
             */
            void updateTotalAccordingToExecutingPhase(const int& executingPhaseType, const double& initialTime);

            /**
             * @brief Calls and monitors the 'agent'::updateKnowledge method.
             * 
             * @param agent Agent*
             */
            void agent_updateKnowledge(Agent* agent);

            /**
             * @brief Calls and monitors the 'agent'::selectActions method.
             * 
             * @param agent Agent*
             */
            void agent_selectActions(Agent* agent);

            /**
             * @brief Calls and monitors the 'agent'::executeActions method.
             * 
             * @param agent Agent*
             */
            void agent_executeActions(Agent* agent);

            /**
             * @brief Calls and monitors the 'agent'::updateState method.
             * 
             * @param agent Agent*
             */
            void agent_updateState(Agent* agent);

            /**
             * @brief Shuffles all the agents in 'agentsToExecute' and then call their executing methods. It uses OpenMP in case it has been stated so.
             * 
             * @param agentsToExecute AgentsVector&
             */
            void randomlyExecuteAgents(AgentsVector& agentsToExecute);

            /**
             * @brief Checks whether agent identified by 'agentID' is in set _executedAgentsInStep.
             * 
             * @param agentID const std::string&
             * @return bool
             */
            bool hasBeenExecuted(const std::string& agentID) const;

            /**
             * @brief Gathers all the agents inside sub-overlap identified by 'subOverlapID', executes them in random order and leave them up-to-date in 'agentsVector'.
             * 
             * @param agentsVector AgentsVector&
             * @param subOverlapID const int&
             */
            void executeAgentsInSubOverlap(AgentsVector& agentsVector, const int& subOverlapID);

            /**
             * @brief Initializes the passed-by-reference map 'agentsByTypeAndNode', with the neighbouring nodes in the key, and an empty map <typeName, agentsList>.
             * 
             * @param agentsByNode std::map<int, std::map<std::string, AgentsList>>&
             */
            void initializeAgentsToSendMap(std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode) const;

            /**
             * @brief Sends a non-blocking request of 'numberOfElements', from 'data' typed as 'mpiDataType, to 'destinationNode', tagged with 'tag', by the default communicator MPI_COMM_WORLD.
             * 
             * @param data void*
             * @param numberOfElements const int&
             * @param mpiDatatype const MPI_Datatype&
             * @param destinationNode const int&
             * @param tag const int&
             */
            void sendDataRequestToNode(void* data, const int& numberOfElements, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag);

            /**
             * @brief Sends agents in 'agentsByTypeAndNode'. The corresponding node is indicated in the key of the map. 'subOverlapID' is only used for instrumentation purposes.
             * 
             * @param agentsByTypeAndNode const std::map<int, std::map<std::string, AgentsList>>&
             * @param subOverlapID const int&
             */
            void sendGhostAgentsInMap(const std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode, const int& subOverlapID);

            /**
             * @brief Non-blockingly receives agents from the neighbouring nodes. 'subOverlapID' is only used for instrumentation purposes.
             * 
             * @param subOverlapID const int&
             */
            void receiveGhostAgentsFromNeighbouringNodes(const int& subOverlapID);

            /**
             * @brief Retrieves the neighbours from 'potentialNeighbours' that contains the 'agentPosition', letting the result in 'subOverlapNeighboursIDs'.
             * 
             * @param potentialNeighbours const std::list<int>&
             * @param agentPosition const Point2D<int>& 
             * @param subOverlapNeighboursIDs std::list<int> 
             */
            void getRealNeighboursForAgent(std::list<int>& subOverlapNeighboursIDs, const std::list<int>& potentialNeighbours, const Point2D<int>& agentPosition) const;

            /**
             * @brief Gets the neighbouring node IDs that the agent should be sent to (by its current position). It assumes it is inside the inner overlap of the node calling this method, so it only look in the sub-overlap list.
             * 
             * @param neighbouringNodeIDs std::list<int>&
             * @param agent const Agent&
             */
            void getNeighboursToSendAgentInsideInnerSubOverlap(std::list<int>& neighbouringNodeIDs, const Agent& agent) const;

            /**
             * @brief Sends agents in 'agentsVector' and receives them, if it is necessary (i.e. if they are currently in some suboverlap area). 'subOverlapID' is only used for instrumentation purposes.
             * 
             * @param agentsVector const AgentsVector&
             * @param subOverlapID const int&
             */
            void synchronizeAgentsIfNecessary(const AgentsVector& agentsVector, const int& subOverlapID);

            /**
             * @brief Checks neighbours of sub-overlap 'subOverlapID', and add to the passed-by-ref list 'subOverlapNeighboursIDs' only those that contains the 'agentPosition'. 'subOverlapID' needs to be between [eMinSubOverlaps_Mode9, eMaxSubOverlaps_Mode9] in mode9, or in [eMinSubOverlaps_Mode4, eMaxSubOverlaps_Mode4] in mode4.
             * 
             * @param subOverlapNeighboursIDs 
             * @param subOverlapID 
             * @param agentPosition const Point2D<int>&
             */
            void addSubOverlapNeighboursFromPosition(std::list<int>& subOverlapNeighboursIDs, const int& subOverlapID, const Point2D<int>& agentPosition) const;

            /**
             * @brief Gets the neighbouring nodes that the agent needs to be send to (for deletion).
             * 
             * @param subOverlapNeighboursIDs std::list<int>&
             * @param originalSubOverlapAreaID const int&
             * @param agent const Agent&
             */
            void getNeighboursThatNeedToRemoveAgent(std::list<int>& subOverlapNeighboursIDs, const int& originalSubOverlapAreaID, const Agent& agent) const;
            
            /**
             * @brief Gets the neighbouring nodes that the agent needs to be send to (for addition).
             * 
             * @param subOverlapNeighboursIDs std::list<int>&
             * @param originalSubOverlapAreaID const int&
             * @param agent const Agent&
             */
            void getNeighboursThatNeedToAddAgent(std::list<int>& subOverlapNeighboursIDs, const int& originalSubOverlapAreaID, const Agent& agent) const;

            /**
             * @brief Gets the neighbours that 'agent' needs to be send to, using the 'subpartitionMode' and the 'originalSubOverlapAreaID'. Lets the result in 'neighbouringNodeIDs'.
             * 
             * @param neighbouringNodeIDs std::list<int>&
             * @param originalSubOverlapAreaID const int&
             * @param agent const Agent&
             * @param subpartitioningMode const int&
             */
            void getNeighboursForAgentAccordingToSubpartitioningMode(std::list<int>& neighbouringNodeIDs, const int& originalSubOverlapAreaID, const Agent& agent, const int& subpartitioningMode) const;

            /**
             * @brief Non-blockingly sends the agents in 'agentsVector' to the corresponding neighbours of the overlap area identified by 'originalSubOverlapAreaID' and the suboverlap to which the agent has just moved to (currently is).
             * 
             * @param agentsVector const AgentsVector&
             * @param originalSubOverlapAreaID const int&
             * @param subpartitioningMode const int&
             */
            void sendGhostAgentsToNeighbours(const AgentsVector& agentsVector, const int& originalSubOverlapAreaID, const int& subpartitioningMode);

            /**
             * @brief Initializes the passed-by-reference map 'rasterValuesByNode', with the neighbouring nodes in the key, and an map in the value, which contains in turn the rasterIndex in the key, and a list of empty pair<position,value> in the value which are the modified values.
             * 
             * @param rasterValuesByNode const std::map<int, std::map<int, std::list<std::pair<Point2D<int>, int>>>>&
             */
            void initializeRasterValuesToSendMap(std::map<int, MapOfValuesByRaster>& rasterValuesByNode) const;

            /**
             * @brief Gets the list of the neighour IDs, which need to know about the recently changed raster value at 'point'. 'point' should be inside the current's node inner sub-overlap area.
             * 
             * @param point const Point2D<int>&
             * @return std::list<int> 
             */
            void getNeighbourIDsInInnerSubOverlap(const Point2D<int>& point, std::list<int>& neighbourIDs) const;

            /**
             * @brief Gets the list of the neighour IDs, which need to know about the recently changed raster value at 'point'. 'point' should be insde the current's node outer sub-overlap area.
             * 
             * @param point const Point2D<int>&
             * @return std::list<int> 
             */
            void getNeighbourIDsInOuterSubOverlap(const Point2D<int>& point, std::list<int>& neighbourIDs) const;

            /**
             * @brief Securely adds the pair <position,value> in the 'map'.
             * 
             * @param map MapOfPositionsAndValues&
             * @param position const Point2D<int>&
             * @param value const int&
             */
            void addPositionAndValueToMap(MapOfPositionsAndValues& map, const Point2D<int>& position, const int& value);

            /**
             * @brief Sends rasters in 'rasterValuesByNode'. Check 'initializeRasterValuesToSendMap(...)' documentation to know what is exactly the map. 'subOverlapID' is only used for instrumentation purposes.
             * 
             * @param rasterValuesByNode std::map<int, std::map<int, std::list<std::pair<Point2D<int>, int>>>>&
             * @param subOverlapID const int&
             */
            void sendRasterValuesInMap(const std::map<int, MapOfValuesByRaster>& rasterValuesByNode, const int& subOverlapID);

            /**
             * @brief Non-blockingly sends rasters to the other nodes. It only takes into account those positions around the sub-overlap area identifed by 'subOverlapAreaID' (i.e. the area expanded _overlapSize).
             * 
             * @param subOverlapAreaID const int&
             */
            void sendRastersToNeighbours(const int& subOverlapAreaID = -1);

            /**
             * @brief Non-blockingly receives rasters from the other nodes. 'subOverlapID' is only used for instrumentation purposes.
             * 
             * @param subOverlapID const int&
             */
            void receiveRasters(const int& subOverlapID = -1);

            /**
             * @brief Waits for the send/receive messages to complete and free the communication buffers (in _sendRequests).
             * 
             */
            void clearRequests();

            /**
             * @brief Finishes the currently sleeping processes.
             * 
             */
            void finishSleepingProcesses();

            /**
             * @brief Gets the total number of overlapping cells for all the nodes in 'tree'.
             * 
             * @param tree const MPILoadBalanceTree&
             * @return int 
             */
            int getTotalNumberOfOverlappingCells(const MPILoadBalanceTree& tree) const;

        public:

            /** INITIALIZATION PUBLIC METHODS **/

            /**
             * @brief Construct a new MPIMultiNode object.
             * 
             */
            MPIMultiNode();

            /**
             * @brief Destroy the MPIMultiNode object.
             * 
             */
            virtual ~MPIMultiNode();

            /**
             * @brief Performs a divide test, returning the number of overlapping cells resulting from the test. Called by the AutoAdjustment subsystem.
             * 
             * @param numberOfProcesses const int&
             * @return int 
             */
            int performDivideTest(const int& numberOfProcesses);

            /**
             * @brief Resets the partitioning variables in order to perform a new partitioning (rebalancing). Called by the AutoAdjustment subsystem.
             * 
             * @param newNumberOfProcesses const int&
             */
            void resetPartitioning(const int& newNumberOfProcesses);

            /** INITIALIZATION PUBLIC METHODS (INHERIT) **/

            /**
             * @brief Initializes everything needed before creation of agents and rasters. 
             * 
             * @param argc Not used.
             * @param argv Not used.
             */
            void init(int argc, char *argv[]) override;

            /**
             * @brief Calls the creation of rasters and agents (in child) and initialize all the the data processes (MPI structures & partition of the space into nodes).
             * 
             */
            void initData() override;

            /** RUN PUBLIC METHODS (INHERIT) **/

            /**
             * @brief Returns true if a signal has been sent from the master process to this process in order to finalize its flow. Controlled by the member variable _justFinished.
             * 
             * @return bool
             */
            bool hasBeenTaggedAsJustFinished() const override;

            /**
             * @brief Returns true if a signeal has been sent from the master process to this process in order be added to the computing (active) processes of the simulation. Controlled by the member variable _justAwaken.
             * 
             * @return bool 
             */
            bool hasBeenTaggedAsJustAwaken() const override;

            /**
             * @brief Updates the resources modified in the World::stepEnvironment() method.
             * 
             */
            void updateEnvironmentState() override;

            /**
             * @brief Rebalances the space among all the available nodes if necessary.
             * 
             */
            void checkForRebalancingSpace() override;

            /**
             * @brief Executes the agents and updates the world.
             * 
             */
            void executeAgents() override;

            /**
             * @brief Executes everything needed to finalize the simulation (e.g. finish communications for parallel nodes).
             * 
             */
            void finish() override;

            /**
             * @brief Get a random Point2D within the area owned by this node/scheduler
             * 
             * @return Point2D<int> 
             */
            Point2D<int> getRandomPosition() const override;

            /**
             * @brief Get the wall time in seconds for the MPI scheduler.
             * 
             * @return double 
             */
            double getWallTime() const override;

            /**
             * @brief 
             * 
             */
            void removeAgents() override;

            /**
             * @brief 
             * 
             * @param agent 
             */
            void addAgentToBeRemoved(Agent* agent) override;

            /**
             * @brief Get the agent identified by 'id'. It should be able to be seen by the calling MPI node (i.e. it should be an agent within _nodeSpace.ownedAreaWithOuterOverlaps). Otherwise, it returns an iterator pointing to _world->endAgents().
             * 
             * @param id const std::string.
             * @return Agent*
             */
            Agent* getAgent(const std::string& id) override;

            /**
             * @brief Get the Agent object
             * 
             * @param position const Point2D<int>&
             * @param type const std::string&
             * @return AgentsVector 
             */
            AgentsVector getAgent(const Point2D<int>& position, const std::string& type = "all") override;

            /**
             * @brief Counts the total number of neighbours for the given parameters.
             * 
             * @param target Agent* target
             * @param radius const double& radius
             * @param type const std::string& type
             * @return int 
             */
            int countNeighbours(Agent* target, const double& radius, const std::string& type) override;

            /**
             * @brief Get the Neighbours object
             * 
             * @param target 
             * @param radius 
             * @param type 
             * @return AgentsVector 
             */
            AgentsVector getNeighbours(Agent* target, const double& radius, const std::string& type) override;

            /**
             * @brief Gets the total number of agents of 'type', updating the result in the different nodes.
             * 
             * @param type const std::string& type
             * @return size_t 
             */
            size_t getNumberOfTypedAgents(const std::string& type) const override;

            /**
             * @brief Sets the value of the 'position' in the 'raster' object.
             * 
             * @param raster DynamicRaster&
             * @param position const Point2D<int>&
             * @param value int
             */
            void setValue(DynamicRaster& raster, const Point2D<int>& position, int value) override;

            /**
             * @brief Gets the value of the 'position' in the 'raster' object.
             * 
             * @param raster const DynamicRaster& raster
             * @param position const Point2D<int>& position
             * @return int 
             */
            int getValue(const DynamicRaster& raster, const Point2D<int>& position) const override;

            /**
             * @brief Sets the maximum value of the 'position' in the 'raster' object.
             * 
             * @param raster DynamicRaster& raster
             * @param position const Point2D<int>& position
             * @param value int value
             */
            void setMaxValue(DynamicRaster& raster, const Point2D<int>& position, int value) override;

            /**
             * @brief Gets the value of the 'position' in the 'raster' object.
             * 
             * @param raster const DynamicRaster&
             * @param position const Point2D<int>&
             * @return int 
             */
            int getMaxValue(const DynamicRaster& raster, const Point2D<int>& position) const override;

            /** SERIALIZATION METHODS **/

            /**
             * @brief 
             * 
             * @param type 
             * @param key 
             * @param value 
             */
            void addStringAttribute(const std::string& type, const std::string& key, const std::string& value) override;

            /**
             * @brief 
             * 
             * @param type 
             * @param key 
             * @param value 
             */
            void addIntAttribute(const std::string& type, const std::string& key, int value) override;

            /**
             * @brief 
             * 
             * @param type 
             * @param key 
             * @param value 
             */
            void addFloatAttribute(const std::string & type, const std::string& key, float value) override;

            /**
             * @brief 
             * 
             * @param step 
             */
            void serializeAgents(const int& step) override;

            /**
             * @brief 
             * 
             * @param step 
             */
            void serializeRasters(const int& step) override;

            /**
             * @brief 
             * 
             * @return const Rectangle<int>& 
             */
            const Rectangle<int>& getOwnedArea() const;

            /**
             * @brief Gets the _overlapSize member.
             * 
             * @return const int&
             */
            const int& getOverlap() const;

    };

} // namespace Engine

#endif // __MPIMultiNode_hxx__