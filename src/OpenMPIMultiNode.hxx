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

            template <typename T>
            struct node {
                T value;
                node* left;
                node* right;
            };

            node<Rectangle<int>>* _root;                    //! Tree used for the uneven partitioning of the space in _numTasks nodes.
            
            struct MPINode {
                Rectangle<int> ownedArea;
                std::list<int> neighbours;
            };

            std::map<int, MPINode> _mpiNodesMap;            //! Map<nodeId, nodeInformation> containing the leafs of _root, where the 'value' field is now the 'ownedArea', and the IDs of the neighbours are listed.

            //Serializer _serializer;                       //! Serializer instance.

            Rectangle<int> _ownedAreaWithOuterOverlaps;     //! Area of this node with    outer (other nodes) overlaps.
            Rectangle<int> _ownedArea;                      //! Area of this node with    inner (this node)   overlaps.
            Rectangle<int> _ownedAreaWithoutInnerOverlap;   //! Area of this node without inner (this node)   overlaps.

            /**
             * @brief Splits a string 's' by 'delimiter', appending the results into a vector.
             * 
             * @param s const std::string&
             * @param delimiter const char&
             * @return std::vector<std::string> 
             */
            std::vector<std::string> splitStringByDelimiter(const std::string& s, const char& delimiter);

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
             * @brief Recursively return the number of nodes of the tree starting at 'node' at level 'desiredDepth'.
             * 
             * @param node node<Rectangle<int>>*
             * @param desiredDepth const int&
             * @param currentDepth int
             * @return int 
             */
            int numberOfNodesAtDepthRecursive(node<Rectangle<int>>* node, const int& desiredDepth, int currentDepth);

            /**
             * @brief Return the number of nodes of the tree starting at 'node' at level 'desiredDepth'
             * 
             * @param node node<Rectangle<int>>*
             * @param desiredDepth const int&
             * @return int 
             */
            int numberOfNodesAtDepth(node<Rectangle<int>>* node, const int& desiredDepth);

            /**
             * @brief Check whether a 'x' is a power of 2 or not
             * 
             * @param x const int&
             * @return bool
             */
            bool isPowerOf2(const int& x);

            /**
             * @brief Return the number of current leaf nodes in tree 'node'.
             * 
             * @param node node<Rectangle<int>>*
             * @return int 
             */
            int numberOfLeafs(node<Rectangle<int>>* node);

            /**
             * @brief Return whether the tree should still procreate or not.
             * 
             * @param currentHeight const int&
             * @return bool
             */
            bool stopProcreating(const int& currentHeight);

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
            double getAgentWeight(const Agent& agent);

            /**
             * @brief Gets the total weight of the agents in 'agentsVector'.
             * 
             * @param agentsVector const AgentsVector&
             * @return double
             */
            double getAgentsWeight(const AgentsVector& agentsVector);

            /**
             * @brief Get the weight of all the agents in the simulation.
             * 
             * @return double 
             */
            double getAllAgentsWeight();

            /**
             * @brief Get an Agents list which are in 'position'
             * 
             * @param position const Point2D<int>&
             * @param type const string&
             * @return AgentsVector 
             */
            AgentsVector getAgentsInPosition(const Point2D<int>& position, const std::string& type = "all");

            /**
             * @brief Get the total agents weight that appear in cell <row, column>
             * 
             * @param row const int&
             * @param column const int&
             * @return double 
             */
            double getAgentsWeightFromCell(const int& row, const int& column);

            /**
             * @brief Creates an uneven partitioning of the Rectangle<int> in 'treeNode' (->value) based on the 'totalWeight' an the current position of the agents in the _world.
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
            void getPartitionsFromTree(node<Rectangle<int>>* node, std::vector<Rectangle<int>>& partitions);

            /**
             * @brief Adds into the _mpiNodeMap the partition <nodeId, partitions[neighbourIndex]>.
             * 
             * @param partitions const std::vector<Rectangle<int>>&
             * @param neighbourIndex const int&
             */
            void addMPINodeInMapItIsNot(const std::vector<Rectangle<int>>& partitions, const int& neighbourIndex);

            /**
             * @brief Check whether 'rectangleA' and 'rectangleB' are adjacent.
             * 
             * @param rectangleA const Rectangle<int>&
             * @param rectangleB const Rectangle<int>&
             */
            bool areTheyNeighbours(const Rectangle<int>& rectangleA, const Rectangle<int>& rectangleB);

            /**
             * @brief Create all the information needed for the processing MPI nodes to properly be executed and communicate with the rest of the nodes.
             * 
             */
            void createNodesInformationToSend();

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
             * @brief It creates the binary tree () representing the partitions of the world for each of the MPI tasks.
             * 
             */
            void divideSpace() override;

            /**
             * @brief It sends the created spaces to the rest of MPI processes.
             * 
             */
            void sendSpaces() override;


            void initData() {}
            void executeAgents() {}
            void finish() {}

            /**
             * @brief Get a random Point2D within the area owned by this node/scheduler
             * 
             * @return Point2D<int> 
             */
            Point2D<int> getRandomPosition() const;

            double getWallTime() const {}
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
            AgentsVector getAgent(const Point2D<int>& position, const std::string& type = "all");

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