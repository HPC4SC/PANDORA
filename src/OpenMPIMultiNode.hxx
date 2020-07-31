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

#ifndef __OpenMPIMultiNode_hxx__
#define __OpenMPIMultiNode_hxx__

#include <World.hxx>
#include <Scheduler.hxx>
#include <LoadBalanceTree.hxx>
#include <OpenMPIMultiNodeLogs.hxx>
#include <Serializer.hxx>

#include <set>
#include <mpi.h>

namespace Engine
{

    class LoadBalanceTree;
    class OpenMPIMultiNodeLogs;

    class OpenMPIMultiNode : public Scheduler
    {

        public:

            struct MPINode {
                Rectangle<int> ownedAreaWithoutInnerOverlap;                //! Area of this node without inner (this node)   overlaps.
                Rectangle<int> ownedArea;                                   //! Area of this node with    inner (this node)   overlaps.
                Rectangle<int> ownedAreaWithOuterOverlaps;                  //! Area of this node with    outer (other nodes) overlaps.
                std::map<int, MPINode*> neighbours;                         //! Map<neighbouringNodeId, neighbouringNodeSpaces> containing the neighbours information for communication.

                std::map<int, Rectangle<int>> innerSubOverlaps;             //! Sub-overlaps (Sub areas of the inner overlap). Should be 8 in total. Map<subOverlapID, subOverlapArea>, where subOverlapID = Engine::SubOverlapType enum.
                std::map<int, std::list<int>> innerSubOverlapsNeighbours;   //! Sub-overlaps neighbouring nodes. Map<subOverlapID, list<nodeID>>. Used for efficient agents and rasters communication.
            };

            typedef std::map<int, std::map<std::string, AgentsList>> NeighbouringAgentsMap;
            typedef std::map<int, MPINode> MPINodesMap;

            typedef std::list<std::pair<Point2D<int>, int>> ListOfPositionAndValue;
            typedef std::map<int, ListOfPositionAndValue> ListOfValuesByRaster;

            friend class OpenMPIMultiNodeLogs;

        protected:

            /** Base data structures for the space partitioning (master node only) **/
            LoadBalanceTree* _tree;

            MPINodesMap _mpiNodesMapToSend;                         //! Map<nodeId, nodeInformation> containing the leafs of _tree, where the 'value' field is now the 'ownedArea', and the IDs of the 'neighbours' are mapped with their coordinates.
            NeighbouringAgentsMap _neighbouringAgents;              //! <nodeID, <agentsType, agentsList>> with the neighbouring agents, classified first by their belonging nodes. Used to send/receive agents from master to the rest of the nodes.

            /** Node own data structures (nodes0..n) **/
            MPINode _nodeSpace;                                     //! Areas and neighbours information for this node.

            std::set<std::string> _executedAgentsInStep;            //! Set containing the IDs of the agents that have been already executed in the current step.
            ListOfValuesByRaster _sentRastersInStep;                //! <rasterIndex, list<rasterPosition,positionValue>>. Map containing the list of already sent positions in the current step, by raster.

            /** MPI Data Structures **/
            int _masterNodeID;                                      //! ID of the master node. Used for communication.
            bool _assignLoadToMasterNode;                           //! True if the master node also processes agents, false if it's only used for partitioning and communication.

            struct Coordinates {
                int top, left, bottom, right;
            };                                                      //! Struct used to parse in/out the to-be-send/received coordinates.
            MPI_Datatype* _coordinatesDatatype;                     //! Own MPI Datatype used to send/receive the coordinates of a node.

            struct MpiRequest {
                int packageID;
                void* package;
                int requestType;
                MPI_Request request;
            };
            std::list<MpiRequest*> _sendRequests;
            //std::list<MpiRequest*> _receiveRequests;

            /** Other structures **/
            OpenMPIMultiNodeLogs* _schedulerLogs;

            double _initialTime;                                    //! Initial running time.

            Serializer _serializer;                                 //! Serializer instance.

            /** INITIALIZATION PROTECTED METHODS **/

            /**
             * @brief Creates the names of the log files for each of the MPI processes, appending them in the _logFileNames member.
             * 
             */
            void initLogFileNames();

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
             * @brief It creates the binary tree '_root' representing the partitions of the world for each of the MPI tasks. Besides, it creates the nodes structs to be send to each one of the slaves.
             * 
             */
            void divideSpace();

            /**
             * @brief It fills own structures for _masterNodeID and sends the created spaces to the rest of MPI processes.
             * 
             */
            void sendSpacesToNodes();

            /**
             * @brief It receives the created spaces from the MPI master node identified by 'masterNodeID'.
             * 
             * @param const int&
             */
            void receiveSpacesFromNode(const int& masterNodeID);

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
             * @brief Gets the ID of the nodes that should take care of 'agent', leaving them in the 'agentNodes'.
             * 
             * @param agent const Agent&
             * @param agentNodes std::list<int>&
             */
            void getBelongingNodesOfAgent(const Agent& agent, std::list<int>& agentNodes) const;

            /**
             * @brief Classify the agents of the simulation in their corresponding neighbourhood (node).
             * 
             */
            void createNeighbouringAgents();

            /**
             * @brief Sends the agents in list 'agentsToSend' to 'currentNode'. All the sent agents are 'agentType'.
             * 
             * @param agentsToSend const AgentsList&
             * @param currentNode const inst&
             * @param agentType const std::string&
             */
            void sendAgentsToNodeByType(const AgentsList& agentsToSend, const int& currentNode, const std::string& agentType) const;

            /**
             * @brief Check whether 'agent' is within the 'agentsList'
             * 
             * @param agent const Agent&
             * @param agentsList const AgentsList&
             * @return bool
             */
            bool isAgentInList(const Agent& agent, const AgentsList& agentsList) const;

            /**
             * @brief Gets the agent in _world that are identified by 'agentID'.
             * 
             * @param agentID const std::string&
             * @return AgentsList::const_iterator 
             */
            AgentsList::const_iterator getAgentInWorldFromID(const std::string& agentID) const;

            /**
             * @brief Removes agents in the node's _world, based on their IDs.
             * 
             * @param agentIDsToRemove const std::list<std::string>&
             */
            void removeAgentsFromID(const std::list<std::string>& agentIDsToRemove);

            /**
             * @brief Keeps all the agents in the list 'agentsToKeep'. All the remaining agents of the simulation are discarded!
             * 
             * @param agentsToKeepconst AgentsList& 
             */
            void keepAgentsInNode(const AgentsList& agentsToKeep);

            /**
             * @brief Sends the agents corresponding to each node from _masterNodeID to all of the other nodes. Agents belonging to _masterNodeID are kept in the master node, the rest are discarded.
             * 
             */
            void sendAgentsToNodes();

            /**
             * @brief From the 'masterNodeID', it receives the agents that should consider for its assigned space.
             * 
             * @param masterNodeID const int&
             */
            void receiveAgentsFromNode(const int& masterNodeID);

            /**
             * @brief Sends the base data (StaticRaster info) of 'raster' to 'nodeID'. Intenteded to use only at the beginning of the simulation, when distributing load.
             * 
             * @param raster const DynamicRaster&
             */
            void sendRasterBaseInfoToNode(const DynamicRaster& raster, const int& nodeID) const;

            /**
             * @brief Sends the 'raster' fileName to 'nodeID'.
             * 
             * @param raster const DynamicRaster&
             * @param nodeID const int&
             */
            void sendRasterFileNameToNode(const DynamicRaster& raster, const int& nodeID) const;

            /**
             * @brief Sends the dynamic information of 'raster' from the master node to all the rest of the nodes. Intenteded to use only at the beginning of the simulation, when distributing load.
             * 
             * @param raster const DynamicRaster&
             */
            void sendRasterDynamicInfoToNode(const DynamicRaster& raster, const int& nodeID) const;

            /**
             * @brief Sends the rasters corresponding to each node from _masterNodeID to all of the other nodes.
             * 
             */
            void sendRastersToNodes() const;

            /**
             * @brief Receives the incoming data from 'node' identified by 'tag'.
             * 
             * @param nodeID const int&
             * @param tag const int&
             * @return std::string
             */
            std::string receiveStringFromNode(const int& nodeID, const int& tag) const;

            /**
             * @brief Receive the incoming static raster base info.
             * 
             * @param masterNodeID const int&
             * @param rasterID int&
             * @param rasterName std::string&
             * @param rasterSerialize bool&
             */
            void receiveRasterBaseInfo(const int& masterNodeID, int& rasterID, std::string& rasterName, bool& rasterSerialize) const;

            /**
             * @brief Receive the incoming dynamic info of a raster.
             * 
             * @param masterNodeID const int& masterNodeID
             * @param minValue int&
             * @param maxValue int&
             * @param defaultValue int&
             */
            void receiveRasterDynamicInfo(const int& masterNodeID, int& minValue, int& maxValue, int& defaultValue) const;

            /**
             * @brief From the 'masterNodeID', it receives the rasters that should consider for its assigned space.
             * 
             * @param masterNodeID 
             */
            void receiveRastersFromNode(const int& masterNodeID);

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
             * @brief Creates a rectangle like 'rectangle' expanded exactly 'expansion' cells in all directions (if possible). If 'contractInTheLimits' == false means that, if 'rectangle' is already in the limits of the simulation space ([_root->value.top(), _root->value.left()] or [_root->value.bottom(), _root->value.right()], then the resulting rectangle will NOT be expanded (even if 'expansion' < 0 == contraction). This is normally used only when 'expansion' < 0.
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
             * @brief Check whether 'rectangleA' and 'rectangleB' are adjacent.
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
             * @brief Prints the neighbourhoods (agents belonging to nodes).
             * 
             */
            void printNeighbouringAgentsPerTypes() const;

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

            /** RUN PROTECTED METHODS (CALLED BY INHERIT METHODS) **/

            /**
             * @brief Performs everything that is needed to save the state of agents and rasters among sub-overlap processing chunks.
             * 
             */
            void initializeAgentsAndRastersState();

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
             * @brief Gathers all the agents inside 'areaToExecute', executes them in random order and leave them up-to-date in 'agentsList'.
             * 
             * @param areaToExecute const Rectangle<int>&
             * @param agentsVector AgentsVector&
             */
            void executeAgentsInArea(const Rectangle<int>& areaToExecute, AgentsVector& agentsVector);

            /**
             * @brief Initializes the passed-by-reference map 'agentsByNode', with the neighbouring nodes in the key, and an empty list in the value.
             * 
             * @param agentsByNode std::map<int, std::list<Agent*>>&
             */
            void initializeAgentsToSendMap(std::map<int, std::list<Agent*>>& agentsByNode) const;

            /**
             * @brief Sends a non-blocking request of 'data' typed as 'mpiDataType, to 'destinationNode', tagged with 'tag'.
             * 
             * @param data void*
             * @param mpiDatatype const MPI_Datatype&
             * @param destinationNode const int&
             * @param tag const int&
             */
            void sendDataRequestToNode(void* data, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag);

            /**
             * @brief Sends agents in 'agentsToSend'. The corresponding node is indicated in the key of the map.
             * 
             * @param agentsToSend const std::map<int, std::list<Agent*>>&
             */
            void sendGhostAgentsInMap(const std::map<int, std::list<Agent*>>& agentsByNode);

            /**
             * @brief Non-blockingly receives agents from the neighbouring nodes.
             * 
             */
            void receiveGhostAgentsFromNeighbouringNodes();

            /**
             * @brief Gets the neighbours from 'potentialNeighbours' that contains the 'agentPosition'.
             * 
             * @param potentialNeighbours const std::list<int>&
             * @param agentPosition const Point2D<int>& 
             * @return std::list<int> 
             */
            std::list<int> getRealNeighboursForAgent(const std::list<int>& potentialNeighbours, const Point2D<int>& agentPosition) const;

            /**
             * @brief Gets the neighbouring node IDs that the agent should be sent to (by its current position).
             * 
             * @param agent const Agent&
             * @return std::list<int> 
             */
            std::list<int> getNeighboursToSendAgent(const Agent& agent) const;

            /**
             * @brief Sends agents in 'agentsVector' and receives them, if it is necessary (i.e. if they are currently in some suboverlap area).
             * 
             * @param agentsVector const AgentsVector&
             */
            void synchronizeAgentsIfNecessary(const AgentsVector& agentsVector);

            /**
             * @brief Checks neighbours of sub-overlap 'subOverlapID', and add to the passed-by-ref list 'subOverlapNeighboursIDs' only those that contains the 'agentPosition'. 'subOverlapID' needs to be between [1,eNumberOfSubOverlaps].
             * 
             * @param subOverlapNeighboursIDs 
             * @param subOverlapID 
             * @param agentPosition const Point2D<int>&
             */
            void addSubOverlapNeighboursToList(std::list<int>& subOverlapNeighboursIDs, const int& subOverlapID, const Point2D<int>& agentPosition) const;

            /**
             * @brief Non-blockingly sends the agents in 'agentsVector' to the corresponding neighbours of the overlap area identified by 'originalSubOverlapAreaID' and the suboverlap to which the agent has just moved to (currently is).
             * 
             * @param agentsVector const AgentsVector&
             * @param originalSubOverlapAreaID const int&
             */
            void sendGhostAgentsToNeighbours(const AgentsVector& agentsVector, const int& originalSubOverlapAreaID);

            /**
             * @brief Initializes the passed-by-reference map 'rasterValuesByNode', with the neighbouring nodes in the key, and an map in the value, which contains in turn the rasterIndex in the key, and a list of empty pair<position,value> in the value which are the modified values.
             * 
             * @param rasterValuesByNode const std::map<int, std::map<int, std::list<std::pair<Point2D<int>, int>>>>&
             */
            void initializeRasterValuesToSendMap(std::map<int, ListOfValuesByRaster>& rasterValuesByNode);

            /**
             * @brief Checks whether 'point' is inside the outer or inner overlap of the node calling this function.
             * 
             * @param point const Point2D<int>&
             * @return bool
             */
            bool isInOverlapArea(const Point2D<int>& point);

            /**
             * @brief Check whether 'positionToCheck' has changed since the last time rasters were sent to neighbours.
             * 
             * @param positionToCheck const Point2D<int>&
             * @param raster const DynamicRaster&
             * @return bool
             */
            bool hasPositionChangedInRaster(const Point2D<int>& positionToCheck, const DynamicRaster& raster);

            /**
             * @brief Sends rasters in 'rasterValuesByNode'. Check 'initializeRasterValuesToSendMap(...)' documentation to know what is exactly the map.
             * 
             * @param rasterValuesByNode std::map<int, std::map<int, std::list<std::pair<Point2D<int>, int>>>>&
             */
            void sendRasterValuesInMap(const std::map<int, ListOfValuesByRaster>& rasterValuesByNode);

            /**
             * @brief Non-blockingly sends rasters to the other nodes.
             * 
             */
            void sendRastersToNeighbours();

            /**
             * @brief Non-blockingly receives rasters from the other nodes.
             * 
             */
            void receiveRasters();

            /**
             * @brief Waits for the send/receive messages to complete and free the communication buffers (in _sendRequests).
             * 
             */
            void clearRequests();

            /**
             * @brief Gets the iterator pointing to the agent identified by 'agentID'.
             * 
             * @param agentID const std::string&
             * @return AgentsList::const_iterator 
             */
            AgentsList::const_iterator getAgentIteratorFromID(const std::string& agentID);

        public:

            /** INITIALIZATION PUBLIC METHODS **/

            /**
             * @brief Construct a new OpenMPIMultiNode object.
             * 
             */
            OpenMPIMultiNode();

            /**
             * @brief Destroy the OpenMPIMultiNode object.
             * 
             */
            virtual ~OpenMPIMultiNode();

            /**
             * @brief Sets the master node ('_masterNodeID' member)
             * 
             * @param masterNode const int&
             */
            void setMasterNode(const int& masterNode);

            /**
             * @brief Sets the _assignLoadToMasterNode member.
             * 
             * @param loadToMasterNode const bool&
             */
            void assignLoadToMasterNode(const bool& loadToMasterNode);

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
             * @brief Executes the agents and updates the world.
             * 
             */
            void executeAgents() override;

            /**
             * @brief Executes needed after the simulation (e.g. finish communications for parallel nodes).
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
             * @brief Get the wall time for the MPI scheduler.
             * 
             * @return double 
             */
            double getWallTime() const override;

            /**
             * @brief 
             * 
             */
            void removeAgents();

            /**
             * @brief 
             * 
             * @param agent 
             */
            void removeAgent(Agent* agent);

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

#endif // __OpenMPIMultiNode_hxx__