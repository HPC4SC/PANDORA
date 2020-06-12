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

#include <OpenMPIMultiNode.hxx>

namespace Engine {

    // PUBLIC METHODS //

    OpenMPIMultiNode::OpenMPIMultiNode() 
    {
        _root = NULL;
    }

    OpenMPIMultiNode::~OpenMPIMultiNode()
    {
        destroyTree(_root);
    }

    void OpenMPIMultiNode::init(int argc, char *argv[]) 
    {
        int alreadyInitialized;
        MPI_Initialized(&alreadyInitialized);
        if (not alreadyInitialized) MPI_Init(&argc, &argv);

        MPI_Comm_size(MPI_COMM_WORLD, &_numberOfMPITasks);
        MPI_Comm_rank(MPI_COMM_WORLD, &_id);

        _updateKnowledgeInParallel = false;
        _executeActionsInParallel = false;
        omp_init_lock(&_ompLock);

        // _boundaries._origin = Point2D<int>( 0, 0 );
        // _boundaries._size = _world->getConfig( ).getSize( );
        _boundaries = Rectangle<int>(_world->getConfig().getSize());

        initTree();
    }

    void OpenMPIMultiNode::divideSpace()
    {
        _totalAgentsWeight = (double) _world->getNumberOfAgents();   // This can be changed so a 'weight' for an agent can be anything. Here all agents weight the same: 1
        divideSpaceRecursive(_root, _totalAgentsWeight, (int) std::log2(_numberOfMPITasks) + 1);
    }

    void OpenMPIMultiNode::sendSpaces()
    {

    }

    // PROTECTED METHODS //

    void OpenMPIMultiNode::initTree() 
    {
        _root = new node<Rectangle<int>>;
        _root->value = Rectangle<int>(_world->getConfig().getSize());
        _root->left = NULL;
        _root->right = NULL;
    }

    int OpenMPIMultiNode::numberOfLeafs(node<Rectangle<int>>* node)
    {
        if (node == NULL) return 0;
        if (node->left == NULL and node->right == NULL) return 1;
        return numberOfLeafs(node->left) + numberOfLeafs(node->right);
    }

    node<Rectangle<int>>* OpenMPIMultiNode::insertNode(const Rectangle<int>& rectangle, node<Rectangle<int>>* treeNode)
    {
        if (treeNode->left == NULL)
        {
            treeNode->left = new node<Rectangle<int>>;
            treeNode->left->value = rectangle;
            treeNode->left->left = NULL;
            treeNode->left->right = NULL;

            return treeNode->left;
        }
        else if (treeNode->right == NULL)
        {
            treeNode->right = new node<Rectangle<int>>;
            treeNode->right->value = rectangle;
            treeNode->right->left = NULL;
            treeNode->right->right = NULL;

            return treeNode->right;
        }
    }

    void OpenMPIMultiNode::destroyTree(node* leaf)
    {
        if (leaf != NULL) 
        {
            destroyTree(leaf->left);
            destroyTree(leaf->right);
            delete leaf;
        }
    }

    double OpenMPIMultiNode::getAgentWeight(const Agent& agent)
    {
        // Make a map<agentsId, agentsWeight> for more complex implementations
        return 1;  // Simplest implementation
    }

    double OpenMPIMultiNode::getAgentsWeight(const AgentsVector& agentsVector)
    {
        double totalWeight = 0;
        for (i = 0; i < agentsVector.size(); ++i) totalWeight += getAgentWeight(agentsVector[i].get());
        return totalWeight;
    }

    AgentsVector OpenMPIMultiNode::getAgentsInPosition(const Point2D<int>& position, const std::string& type)
    {
        AgentsVector result;
        for (AgentsList::iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            AgentPtr agent = *it;
            if (agent->getPosition().isEqual(position))
            {
                if (type.compare("all") == 0 or agent->isType(type)) result.push_back(agent);
            }
        }
        return result;
    }

    double OpenMPIMultiNode::getAgentsWeightFromCell(const int& row, const int& column)
    {
        Point2D<int> position(row, column);
        AgentsVector agentsVector = getAgentsInPosition(position);
        return getAgentsWeight(agentsVector);
    }

    void OpenMPIMultiNode::divideSpaceRecursive(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentDepth)
    {
        if (currentDepth == 0 or numberOfLeafs(_root) == _numberOfMPITasks) return 0;

        double leftChildTotalWeight = 0;
        bool stopExploration = false;

        if (currentDepth % 2 == 0) {   // Horizontal exploration
            for (int i = treeNode->value.left(); i <= treeNode->value.right() and not stopExploration; ++i)
            {
                for (int j = treeNode->value.top(); j <= treeNode->value.bottom(); ++j) 
                    leftChildTotalWeight += getAgentsWeightFromCell(j, i);

                if (leftChildTotalWeight >= totalWeight / 2)
                {
                    stopExploration = true;

                    Rectangle<int> leftRectangle(treeNode->value.left(), treeNode->value.top(), i, treeNode->value.bottom());
                    Rectangle<int> rightRectangle(i + 1, treeNode->value.top(), treeNode->value.right(), treeNode->value.bottom());
                    node<Rectangle<int>>* leftNode = insertNode(leftRectangle, treeNode);
                    node<Rectangle<int>>* rightNode = insertNode(rightRectangle, treeNode);

                    divideSpaceRecursive(leftNode, leftChildTotalWeight, depth - 1);
                    divideSpaceRecursive(rightNode, totalWeight - leftChildTotalWeight, depth - 1);
                }

                leftChildTotalWeight = 0;
            }
        }
        else {                  // Vertical exploration
            for (int i = _boundaries.top(); i < _boundaries.bottom(); ++i) {
                for (int j = _boundaries.left(); j < _boundaries.right(); ++j) {
                    // to be implemented


















                }
            }
        }
    }

} // namespace Engine