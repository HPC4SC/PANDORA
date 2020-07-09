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

#include <mpi.h>

namespace Engine
{

    class LoadBalanceTree;
    class OpenMPIMultiNodeLogs;

    class OpenMPIMultiNode : public Scheduler
    {
        public:

            struct MPINode {
                Rectangle<int> ownedAreaWithoutInnerOverlap;        //! Area of this node without inner (this node)   overlaps.
                Rectangle<int> ownedArea;                           //! Area of this node with    inner (this node)   overlaps.
                Rectangle<int> ownedAreaWithOuterOverlaps;          //! Area of this node with    outer (other nodes) overlaps.
                std::map<int, MPINode*> neighbours;                 //! Map<neighbouringNodeId, neighbouringNodeSpaces> containing the neighbours information for communication.
                std::map<int, Rectangle<int>> neighboursOverlap;    //! Map<neighbouringNodeId, neighbouringNodeOverlap>. Used for efficient agents and rasters communication.
            };

            typedef std::map<std::string, std::map<int, AgentsList>> NeighbouringAgentsMap;
            typedef std::map<int, MPINode> MPINodesMap;

            friend class OpenMPIMultiNodeLogs;

        protected:

            /** Base data structures for the space partitioning (master node only) **/
            LoadBalanceTree* _tree;

            MPINodesMap _mpiNodesMapToSend;                         //! Map<nodeId, nodeInformation> containing the leafs of _tree, where the 'value' field is now the 'ownedArea', and the IDs of the 'neighbours' are mapped with their coordinates.
            NeighbouringAgentsMap _neighbouringAgents;              //! <agentsType, <nodeID, agentsList>> with the neighbouring agents, classified first by their types. Used to send/receive agents from master to the rest of the nodes.

            /** Node own data structures (nodes0..n) **/
            MPINode _nodeSpace;                                     //! Areas of this node.

            /** MPI Data Structures **/
            int _masterNodeID;                                      //! ID of the master node. Used for communication.
            bool _assignLoadToMasterNode;                           //! True if the master node also processes agents, false if it's only used for partitioning and communication.

            struct Coordinates {
                int top, left, bottom, right;
            };                                                      //! Struct used to parse in/out the to-be-send/received coordinates.
            MPI_Datatype* _coordinatesDatatype;                     //! Own MPI Datatype used to send/receive the coordinates of a node.

            /** Other structures **/
            OpenMPIMultiNodeLogs* _schedulerLogs;

            double _initialTime;                                    //! Initial running time.
            //Serializer _serializer;                                 //! Serializer instance.

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
             */
            void receiveSpacesFromNode(const int& masterNodeID);

            /**
             * @brief Generates and fills the 'mpiNode'.ownedAreaWithoutInnerOverlap and the 'mpiNode'.ownedAreaWithOuterOverlaps, based on the 'mpiNode'.ownedArea.
             * 
             * @param mpiNode MPINode&
             */
            void generateOverlapAreas(MPINode& mpiNode);

            /**
             * @brief Fills the own structures with the information in 'mpiNodeInfo'.
             * 
             * @param mpiNodeInfo const MPINodeToSend&
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
            void sendAgentsToNodeByType(const AgentsList& agentsToSend, const int& currentNode, const std::string& agentType);

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

            /**
             * @brief Initializes everything needed before creation of agents and rasters. 
             * 
             * @param argc Not used.
             * @param argv Not used.
             */
            void init(int argc, char *argv[]);

            /**
             * @brief Calls the creation of rasters and agents (in child) and initialize all the the data processes (MPI structures & partition of the space into nodes).
             * 
             */
            void initData();

            /** RUN PUBLIC METHODS (INHERIT) **/

            /**
             * @brief Executes the agents and updates the world.
             * 
             */
            void executeAgents();

            /**
             * @brief Executes needed after the simulation (e.g. finish communications for parallel nodes).
             * 
             */
            void finish();

            /**
             * @brief Get a random Point2D within the area owned by this node/scheduler
             * 
             * @return Point2D<int> 
             */
            Point2D<int> getRandomPosition() const;

            /**
             * @brief Get the wall time for the MPI scheduler.
             * 
             * @return double 
             */
            double getWallTime() const;

            size_t getNumberOfTypedAgents( const std::string & type ) const {}
            void removeAgents() {}
            void removeAgent(Agent* agent) {}
            Agent* getAgent(const std::string& id) {}

            /**
             * @brief Get the Agent object
             * 
             * @param position const Point2D<int>&
             * @param type const std::string&
             * @return AgentsVector 
             */
            AgentsVector getAgent(const Point2D<int>& position, const std::string& type = "all") override;

            int countNeighbours(Agent* target, const double& radius, const std::string& type) {}
            AgentsVector getNeighbours(Agent* target, const double& radius, const std::string& type) {}
            void addStringAttribute( const std::string& type, const std::string& key, const std::string& value) {}
            void addIntAttribute(const std::string& type, const std::string& key, int value) {}
            void addFloatAttribute(const std::string & type, const std::string& key, float value) {}
            void serializeAgents(const int& step) {}
            void serializeRasters(const int& step) {}
            void setValue(DynamicRaster& raster, const Point2D<int>& position, int value) {}
            int getValue(const DynamicRaster& raster, const Point2D<int>& position) const {}
            void setMaxValue(DynamicRaster& raster, const Point2D<int>& position, int value) {}
            int getMaxValue(const DynamicRaster& raster, const Point2D<int>& position) const {}

    };

} // namespace Engine

#endif // __OpenMPIMultiNode_hxx__