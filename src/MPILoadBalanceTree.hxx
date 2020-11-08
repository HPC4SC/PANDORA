/*
 * Copyright ( c ) 2012
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

#ifndef __MPILoadBalanceTree_hxx__
#define __MPILoadBalanceTree_hxx__

#include <World.hxx>

namespace Engine
{

    class MPILoadBalanceTree
    {
        public:

            template <typename T>
            struct node {
                T value;
                node* left;
                node* right;
            };

        protected:

            node<Rectangle<int>>* _root;            //! Tree used for the uneven partitioning of the space in _numPartitions nodes.

            World* _world;                          //! Pointer to the World of the simulation
            int _numPartitions;                     //! Number of tasks in which the simulation should be split.

            /**
             * @brief Copies deeply the node into a new object which is returned.
             * 
             * @param node node<Rectangle<int>>*
             * @return node<Rectangle<int>>*
             */
            node<Rectangle<int>>* copyTree(node<Rectangle<int>>* node);

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
             * @brief It recursively destroys the whole tree starting at 'leaf'.
             * 
             * @param leaf node<Rectangle<int>>*
             */
            void destroyTree(node<Rectangle<int>>* leaf);

            /**
             * @brief Get the weight of all the agents in the simulation.
             * 
             * @return double 
             */
            double getAllAgentsWeight() const;

            /**
             * @brief Gets the total weight of the agents in 'agentsVector'.
             * 
             * @param agentsVector const AgentsVector&
             * @return double
             */
            double getAgentsWeight(const AgentsVector& agentsVector) const;

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
             * @brief Gets the created partitions in the leafs of the _root, leaving it in 'partitions'.
             * 
             * @param node node<Rectangle<int>>*
             * @param partitions std::vector<Rectangle<int>>&
             */
            void getPartitionsFromTreeRecursively(node<Rectangle<int>>* node, std::vector<Rectangle<int>>& partitions) const;

        public:

            /**
             * @brief Construct a new MPILoadBalanceTree object.
             * 
             */
            MPILoadBalanceTree();

            /**
             * @brief Destroy the MPILoadBalanceTree object.
             * 
             */
            virtual ~MPILoadBalanceTree();

            /**
             * @brief Sets the _world member.
             * 
             * @param world 
             */
            void setWorld(World* world);

            /**
             * @brief Sets the number of tasks (_numPartitions member) in which the space must be split.
             * 
             * @param numberOfPartitions const int&
             */
            void setNumberOfPartitions(const int& numberOfPartitions);

            /**
             * @brief Initialize the tree starting at _root.
             * 
             */
            void initializeTree();

            /**
             * @brief Resets the tree in order to perform a new partitioning.
             * 
             */
            void resetTree();

            /**
             * @brief Assignment operator. NOTE: The _world member object is shallow copied only.
             * 
             * @param object const MPILoadBalanceTree&
             * @return MPILoadBalanceTree&
             */
            MPILoadBalanceTree& operator=(const MPILoadBalanceTree& object);

            /**
             * @brief Makes a load balanced partition of the space, filling _root.
             * 
             */
            void divideSpace();

            /**
             * @brief Performs a test dividing the space of the _world considering the current _numPartitions. It does not affect the _root member object.
             * 
             */
            void divideSpaceTest();

            /**
             * @brief Gets the current state of the '_root' member.
             * 
             */
            const node<Rectangle<int>>& getTree() const;

            /**
             * @brief Gets the _world member object.
             * 
             * @return World* 
             */
            const World& getWorld() const;

            /**
             * @brief Gets the created partitions, leaving it in 'partitions'.
             * 
             * @param partitions std::vector<Rectangle<int>>&
             */
            void getPartitionsFromTree(std::vector<Rectangle<int>>& partitions) const;

            /**
             * @brief Get an Agents list which are in 'position'
             * 
             * @param position const Point2D<int>&
             * @param type const string&
             * @return AgentsVector 
             */
            AgentsVector getAgentsInPosition(const Point2D<int>& position, const std::string& type = "all") const;

    };

} // namespace Engine

#endif // __MPILoadBalanceTree_hxx__