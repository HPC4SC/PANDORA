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

            //Serializer _serializer; //! Serializer instance.

            template <typename T>
            struct node {
                T value;
                node* left;
                node* right;
            };

            node<Rectangle<int>>* _root;
            
            /**
             * @brief Initialize the tree starting at _root.
             * 
             */
            void initTree();

            /**
             * @brief Return the number of current leaf nodes in tree 'node'.
             * 
             * @param node node<Rectangle<int>>*
             * @return int 
             */
            int numberOfLeafs(node<Rectangle<int>>* node);

            /**
             * @brief It inserts a new node, with value 'rectangle', from 'treeNode'. It justs looks whether left or right is NULL (in this order) and insert the new node there. It is not recursive!
             * 
             * @param rectangle Rectangle<int>
             * @param treeNode node<Rectangle<int>>*
             * @return node* 
             */
            node<Rectangle<int>>* insertNode(const Rectangle<int>& rectangle, node<Rectangle<int>>* treeNode);

            /**
             * @brief It destroys the whole tree starting at 'leaf'.
             * 
             * @param leaf node*
             */
            void destroyTree(node<Rectangle<int>>* leaf);

            /**
             * @brief Gets the weight of the 'agent'.
             * 
             * @param agent Agent
             * @return double
             */
            double getAgentWeight(const Agent& agent);

            /**
             * @brief Gets the total weight of the agents in 'agentsVector'.
             * 
             * @param agentsVector AgentsVector
             * @return double
             */
            double getAgentsWeight(const AgentsVector& agentsVector);

            /**
             * @brief Get an Agents list which are in 'position'
             * 
             * @param position Point2D<int>
             * @param type string
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
            void divideSpaceRecursive(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentDepth);

        public:

            /**
             * @brief Construct a new OpenMPIMultiNode object
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
            
            Point2D<int> getRandomPosition() const 
            {


            }

            double getWallTime() const {}
            size_t getNumberOfTypedAgents( const std::string & type ) const {}
            void removeAgents() {}
            void removeAgent(Agent* agent) {}
            Agent* getAgent(const std::string& id) {}
            AgentsVector getAgent(const Point2D<int>& position, const std::string& type = "all") {}
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