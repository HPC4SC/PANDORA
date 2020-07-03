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

#include <LoadBalanceTree.hxx>
#include <Agent.hxx>

#include <iostream>

#include <math.h>

namespace Engine {

    /** PUBLIC METHODS **/

    LoadBalanceTree::LoadBalanceTree() : _root(NULL), _world(0), _numTasks(1)
    {
    }

    LoadBalanceTree::~LoadBalanceTree()
    {
        destroyTree(_root);
    }

    void LoadBalanceTree::initializeTreeAndSetData(World* world, const int& numTasks)
    {
        _world = world;
        _numTasks = numTasks;
        initializeTree();
    }

    void LoadBalanceTree::divideSpace()
    {
        divideSpaceRecursively(_root, getAllAgentsWeight(), (int) std::ceil(std::log2(_numTasks)));
    }

    LoadBalanceTree::node<Rectangle<int>>* LoadBalanceTree::getTree()
    {
        return _root;
    }

    void LoadBalanceTree::getPartitionsFromTree(std::vector<Rectangle<int>>& partitions) const
    {
        getPartitionsFromTreeRecursively(_root, partitions);
    }

    AgentsVector LoadBalanceTree::getAgentsInPosition(const Point2D<int>& position, const std::string& type) const
    {
        AgentsVector result;
        for (AgentsList::iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            AgentPtr agent = *it;
            if (agent->getPosition().isEqual(position) and (type.compare("all") == 0 or agent->isType(type)))
                result.push_back(agent);
        }
        return result;
    }

    /** PROTECTED METHODS **/

    void LoadBalanceTree::initializeTree() 
    {
        _root = new node<Rectangle<int>>;
        _root->value = Rectangle<int>(_world->getConfig().getSize());
        _root->left = NULL;
        _root->right = NULL;
    }

    int LoadBalanceTree::numberOfNodesAtDepthRecursive(node<Rectangle<int>>* node, const int& desiredDepth, int currentDepth) const
    {
        if (node == NULL) return 0;
        if (desiredDepth == currentDepth) return 1;
        return numberOfNodesAtDepthRecursive(node->left, desiredDepth, currentDepth + 1) + numberOfNodesAtDepthRecursive(node->right, desiredDepth, currentDepth + 1);
    }

    int LoadBalanceTree::numberOfNodesAtDepth(node<Rectangle<int>>* node, const int& desiredDepth) const
    {
        return numberOfNodesAtDepthRecursive(node, desiredDepth, 0);
    }

    bool LoadBalanceTree::isPowerOf2(const int& x) const
    {
        return x > 0 and not (x & (x - 1));
    }

    int LoadBalanceTree::numberOfLeafs(node<Rectangle<int>>* node) const
    {
        if (node == NULL) return 0;
        if (node->left == NULL and node->right == NULL) return 1;
        return numberOfLeafs(node->left) + numberOfLeafs(node->right);
    }

    bool LoadBalanceTree::stopProcreating(const int& currentHeight) const
    {
        bool condition1 = currentHeight == 0;
        bool condition2 = numberOfLeafs(_root) == _numTasks;

        bool condition3 = false;
        if (not isPowerOf2(_numTasks))
        {
            int numberOfNodesNeededAtLowestLevel = 2 * (_numTasks - std::pow(2, std::floor(std::log2(_numTasks))));
            int lowestLevel = std::ceil(std::log2(_numTasks));
            bool isLowestLevelAlreadyFull = numberOfNodesNeededAtLowestLevel == numberOfNodesAtDepth(_root, lowestLevel);
            condition3 = currentHeight == 1 and isLowestLevelAlreadyFull;
        }

        return condition1 or condition2 or condition3;
    }

    LoadBalanceTree::node<Rectangle<int>>* LoadBalanceTree::insertNode(const Rectangle<int>& rectangle, node<Rectangle<int>>* treeNode)
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

    void LoadBalanceTree::destroyTree(node<Rectangle<int>>* leaf)
    {
        if (leaf != NULL) 
        {
            destroyTree(leaf->left);
            destroyTree(leaf->right);
            delete leaf;
        }
    }

    double LoadBalanceTree::getAgentWeight(const Agent& agent) const
    {
        // Make a map<agentsId, agentsWeight> for more complex implementations
        return 1;  // Simplest implementation
    }

    double LoadBalanceTree::getAgentsWeight(const AgentsVector& agentsVector) const
    {
        double totalWeight = 0;
        for (int i = 0; i < agentsVector.size(); ++i) {
            totalWeight += getAgentWeight(*agentsVector[i]);
        }
        return totalWeight;
    }

    double LoadBalanceTree::getAllAgentsWeight() const
    {
        double totalWeight = 0;
        for (AgentsList::iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
            totalWeight += getAgentWeight(*it->get());
        return totalWeight;
    }

    double LoadBalanceTree::getAgentsWeightFromCell(const int& row, const int& column) const
    {
        Point2D<int> position(row, column);
        AgentsVector agentsVector = getAgentsInPosition(position);
        return getAgentsWeight(agentsVector);
    }

    void LoadBalanceTree::exploreHorizontallyAndKeepDividing(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentHeight)
    {
        double leftChildTotalWeight = 0;
        bool stopExploration = false;

        for (int i = treeNode->value.left(); i <= treeNode->value.right() and not stopExploration; ++i)
        {
            for (int j = treeNode->value.top(); j <= treeNode->value.bottom(); ++j) 
                leftChildTotalWeight += getAgentsWeightFromCell(j, i);

            if (leftChildTotalWeight >= totalWeight / 2)
            {
                stopExploration = true;

                Rectangle<int> leftRectangle(treeNode->value.left(), treeNode->value.top(), i, treeNode->value.bottom());
                Rectangle<int> rightRectangle(i + 1, treeNode->value.top(), treeNode->value.right(), treeNode->value.bottom());
                node<Rectangle<int>>* leftChildNode = insertNode(leftRectangle, treeNode);
                node<Rectangle<int>>* rightChildNode = insertNode(rightRectangle, treeNode);

                divideSpaceRecursively(leftChildNode, leftChildTotalWeight, currentHeight - 1);
                divideSpaceRecursively(rightChildNode, totalWeight - leftChildTotalWeight, currentHeight - 1);
            }
        }
    }

    void LoadBalanceTree::exploreVerticallyAndKeepDividing(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentHeight)
    {
        double leftChildTotalWeight = 0;
        bool stopExploration = false;

        for (int i = treeNode->value.top(); i < treeNode->value.bottom() and not stopExploration; ++i) 
        {
            for (int j = treeNode->value.left(); j < treeNode->value.right(); ++j)
                leftChildTotalWeight += getAgentsWeightFromCell(i, j);

            if (leftChildTotalWeight >= totalWeight / 2)
            {
                stopExploration = true;

                Rectangle<int> topRectangle(treeNode->value.left(), treeNode->value.top(), treeNode->value.right(), i);
                Rectangle<int> bottomRectangle(treeNode->value.left(), i + 1, treeNode->value.right(), treeNode->value.bottom());
                node<Rectangle<int>>* leftChildNode = insertNode(topRectangle, treeNode);
                node<Rectangle<int>>* rightChildNode = insertNode(bottomRectangle, treeNode);

                divideSpaceRecursively(leftChildNode, leftChildTotalWeight, currentHeight - 1);
                divideSpaceRecursively(rightChildNode, totalWeight - leftChildTotalWeight, currentHeight - 1);
            }
        }
    }

    void LoadBalanceTree::divideSpaceRecursively(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentHeight)
    {
        if (stopProcreating(currentHeight)) return;

        bool landscape = treeNode->value.getSize().getWidth() > treeNode->value.getSize().getHeight();
        if (landscape)
            exploreHorizontallyAndKeepDividing(treeNode, totalWeight, currentHeight);
        else
            exploreVerticallyAndKeepDividing(treeNode, totalWeight, currentHeight);
    }

    void LoadBalanceTree::getPartitionsFromTreeRecursively(node<Rectangle<int>>* node, std::vector<Rectangle<int>>& partitions) const
    {
        if (node == NULL) return;
        if (node->left == NULL and node->right == NULL) 
        {
            partitions.push_back(node->value);
            return;
        }
        getPartitionsFromTreeRecursively(node->left, partitions);
        getPartitionsFromTreeRecursively(node->right, partitions);
    }

} // namespace Engine