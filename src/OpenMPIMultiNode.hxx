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
#include <Serializer.hxx>

#include <mpi.h>

namespace Engine
{

    class OpenMPIMultiNode : public Scheduler
    {
        protected:

            /** Base structures for the space partitioning (node0 only) **/
            template <typename T>
            struct node {
                T value;
                node* left;
                node* right;
            };

            node<Rectangle<int>>* _root;                            //! Tree used for the uneven partitioning of the space in _numTasks nodes.
            
            struct MPINodeToSend {
                Rectangle<int> ownedArea;
                std::map<int, Rectangle<int>> neighbours;
            };

            std::map<int, MPINodeToSend> _mpiNodesMapToSend;        //! Map<nodeId, nodeInformation> containing the leafs of _root, where the 'value' field is now the 'ownedArea', and the IDs of the 'neighbours' are mapped with their coordinates.

            /** MPI Structures **/
            struct Coordinates {
                int top, left, bottom, right;
            };                                                      //! Struct used to parse in/out the to-be-send/received coordinates.
            MPI_Datatype* _coordinatesDatatype;                     //! Own MPI Datatype used to send/receive the coordinates of a node.

            /** Node own structures (nodes0..n) **/
            struct MPINode {
                Rectangle<int> ownedAreaWithoutInnerOverlap;        //! Area of this node without inner (this node)   overlaps.
                Rectangle<int> ownedArea;                           //! Area of this node with    inner (this node)   overlaps.
                Rectangle<int> ownedAreaWithOuterOverlaps;          //! Area of this node with    outer (other nodes) overlaps.
                std::map<int, MPINode*> neighbours;                 //! Map<neighbouringNodeId, neighbouringNodeSpaces> containing the neighbours information for communication.
            };

            MPINode _nodesSpace;                                    //! Areas of this node.

            /** Other structures **/
            double _initialTime;                                    //! Initial running time.
            std::map<int, std::string> _logFileNames;               //! Names of the log files for each of the MPI processes.
            //Serializer _serializer;                               //! Serializer instance.

            /** METHODS TO CREATE THE PARTITIONS AND BASIC STRUCTURES FOR NODES **/

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
             * @brief Initialize the tree starting at _root.
             * 
             */
            void initializeTree();

            /**
             * @brief Registers the data structs to be send/received to/from the nodes.
             * 
             */
            void registerMPIStructs();

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
            void fillOwnStructures(const MPINodeToSend& mpiNodeInfo);

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
            void sendNeighboursToNode(const int& nodeID, const std::map<int, Rectangle<int>>& neighbours) const;

            /**
             * @brief It blocking receives the own node coordinates from the master node with ID 'masterNodeID'.
             * 
             * @param masterNodeID const int&
             */
            void receiveOwnAreaFromNode(const int& masterNodeID, MPINodeToSend& mpiNodeInfo) const;

            /**
             * @brief It blocking receives the 'neighbours' information from 'masterNodeID'.
             * 
             * @param masterNodeID const int&
             */
            void receiveNeighboursFromNode(const int& masterNodeID, MPINodeToSend& mpiNodeInfo) const;

            /**
             * @brief Recursively return the number of nodes of the tree starting at 'node' at level 'desiredDepth'.
             * 
             * @param node node<Rectangle<int>>*
             * @param desiredDepth const int&
             * @param currentDepth int
             * @return int 
             */
            int numberOfNodesAtDepthRecursive(node<Rectangle<int>>* node, const int& desiredDepth, int currentDepth) const;

            /**
             * @brief Return the number of nodes of the tree starting at 'node' at level 'desiredDepth'
             * 
             * @param node node<Rectangle<int>>*
             * @param desiredDepth const int&
             * @return int 
             */
            int numberOfNodesAtDepth(node<Rectangle<int>>* node, const int& desiredDepth) const;

            /**
             * @brief Check whether a 'x' is a power of 2 or not
             * 
             * @param x const int&
             * @return bool
             */
            bool isPowerOf2(const int& x) const;

            /**
             * @brief Return the number of current leaf nodes in tree 'node'.
             * 
             * @param node node<Rectangle<int>>*
             * @return int 
             */
            int numberOfLeafs(node<Rectangle<int>>* node) const;

            /**
             * @brief Return whether the tree should still procreate or not.
             * 
             * @param currentHeight const int&
             * @return bool
             */
            bool stopProcreating(const int& currentHeight) const;

            /**
             * @brief It inserts a new node, with value 'rectangle', from 'treeNode'. It justs looks whether left or right is NULL (in this order) and insert the new node there. It is not recursive!
             * 
             * @param rectangle const Rectangle<int>&
             * @param treeNode node<Rectangle<int>>*
             * @return node* 
             */
            node<Rectangle<int>>* insertNode(const Rectangle<int>& rectangle, node<Rectangle<int>>* treeNode);

            /**
             * @brief It destroys the whole tree starting at 'leaf'.
             * 
             * @param leaf node<Rectangle<int>>*
             */
            void destroyTree(node<Rectangle<int>>* leaf);

            /**
             * @brief Gets the weight of the 'agent'.
             * 
             * @param agent const Agent&
             * @return double
             */
            double getAgentWeight(const Agent& agent) const;

            /**
             * @brief Gets the total weight of the agents in 'agentsVector'.
             * 
             * @param agentsVector const AgentsVector&
             * @return double
             */
            double getAgentsWeight(const AgentsVector& agentsVector) const;

            /**
             * @brief Get the weight of all the agents in the simulation.
             * 
             * @return double 
             */
            double getAllAgentsWeight() const;

            /**
             * @brief Get an Agents list which are in 'position'
             * 
             * @param position const Point2D<int>&
             * @param type const string&
             * @return AgentsVector 
             */
            AgentsVector getAgentsInPosition(const Point2D<int>& position, const std::string& type = "all") const;

            /**
             * @brief Get the total agents weight that appear in cell <row, column>
             * 
             * @param row const int&
             * @param column const int&
             * @return double 
             */
            double getAgentsWeightFromCell(const int& row, const int& column) const;

            /**
             * @brief Explore the space in 'treeNode' from left to right (vertical partitioning). j = rows, i = columns.
             * 
             * @param treeNode node<Rectangle<int>>*
             * @param totalWeight const double& totalWeight
             * @param currentHeight const int& currentHeight
             */
            void exploreHorizontallyAndKeepDividing(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentHeight);

            /**
             * @brief Explore the space in 'treeNode' from top to bottom (horizontal partitioning). i = rows , j = columns.
             * 
             * @param treeNode node<Rectangle<int>>*
             * @param totalWeight const double& totalWeight
             * @param currentHeight const int& currentHeight
             */
            void exploreVerticallyAndKeepDividing(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentHeight);

            /**
             * @brief Creates an uneven partitioning of the Rectangle<int> in 'treeNode' (->value) based on the 'totalWeight' and the current position of the agents in the _world.
             * 
             * @param treeNode node<Rectangle<int>>*
             * @param totalWeight const double&
             * @param currentDepth const int&
             */
            void divideSpaceRecursively(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& maxDepth);

            /**
             * @brief Get the created partitions in the leafs of the _root, leaving it in 'partitions'.
             * 
             * @param node node<Rectangle<int>>*
             * @param partitions std::vector<Rectangle<int>>&
             */
            void getPartitionsFromTree(node<Rectangle<int>>* node, std::vector<Rectangle<int>>& partitions) const;

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

            /**
             * @brief Prints nodes partitioning and neighbours for each one.
             * 
             */
            void printNodesBeforeMPI() const;

            /**
             * @brief Prints the nodes structure (ID + Coordinates) in _nodesSpace.
             * 
             */
            void printOwnNodeStructureAfterMPI() const;

            /** METHODS TO RUN THE SIMULATION AND SEND/RECEIV AGENTS BETWEEN NODES **/

        public:

            /**
             * @brief Construct a new OpenMPIMultiNode object.
             * 
             */
            OpenMPIMultiNode();

            /**
             * @brief Destroy the OpenMPIMultiNode object
             * 
             */
            virtual ~OpenMPIMultiNode();

            /**
             * @brief Initializes everything needed before creation of agents and rasters. 
             * 
             * @param argc Not used.
             * @param argv Not used.
             */
            void init(int argc, char *argv[]);

            /**
             * @brief It creates the binary tree '_root' representing the partitions of the world for each of the MPI tasks. Besides, it creates the nodes structs to be send to each one of the slaves.
             * 
             */
            void divideSpace() override;

            /**
             * @brief It fills own structures for node 0 and sends the created spaces to the rest of MPI processes.
             * 
             */
            void sendSpacesAmongNodes() override;

            /**
             * @brief It receives the created spaces from the MPI process 0.
             * 
             */
            void receiveSpacesFromMaster() override;

            void initData() {}
            void executeAgents() {}
            void finish() {}

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

#endif // __SpacePartition_hxx__