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

#include <GeneralState.hxx>

#include <math.h>

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

        //MPI_Comm_size(MPI_COMM_WORLD, &_numTasks);
        MPI_Comm_rank(MPI_COMM_WORLD, &_id);

        _updateKnowledgeInParallel = false;
        _executeActionsInParallel = false;
        omp_init_lock(&_ompLock);

        stablishInitialBoundaries();
        initializeTree();
    }

    void OpenMPIMultiNode::divideSpace()
    {
        divideSpaceRecursively(_root, getAllAgentsWeight(), (int) std::ceil(std::log2(_numTasks)));
        createNodesInformationToSend();
    }

    void OpenMPIMultiNode::sendSpaces()
    {



    }

    Point2D<int> OpenMPIMultiNode::getRandomPosition() const
    {
        return Engine::Point2D<int>(GeneralState::statistics().getUniformDistValue(_ownedArea.left(), _ownedArea.right()),
                                    GeneralState::statistics().getUniformDistValue(_ownedArea.top(), _ownedArea.bottom()));
    }

    AgentsVector OpenMPIMultiNode::getAgent(const Point2D<int>& position, const std::string& type)
    {
        return getAgentsInPosition(position, type);
        // + overlap agents?
    }

    // PROTECTED METHODS //

    void OpenMPIMultiNode::stablishInitialBoundaries()
    {
        // _boundaries._origin = Point2D<int>( 0, 0 );
        // _boundaries._size = _world->getConfig( ).getSize( );
        _boundaries = Rectangle<int>(_world->getConfig().getSize());

        _ownedArea = _boundaries;
    }

    void OpenMPIMultiNode::initializeTree() 
    {
        _root = new node<Rectangle<int>>;
        _root->value = Rectangle<int>(_world->getConfig().getSize());
        _root->left = NULL;
        _root->right = NULL;
    }

    int OpenMPIMultiNode::numberOfNodesAtDepthRecursive(node<Rectangle<int>>* node, const int& desiredDepth, int currentDepth) const
    {
        if (node == NULL) return 0;
        if (desiredDepth == currentDepth) return 1;
        return numberOfNodesAtDepthRecursive(node->left, desiredDepth, currentDepth + 1) + numberOfNodesAtDepthRecursive(node->right, desiredDepth, currentDepth + 1);
    }

    int OpenMPIMultiNode::numberOfNodesAtDepth(node<Rectangle<int>>* node, const int& desiredDepth) const
    {
        return numberOfNodesAtDepthRecursive(node, desiredDepth, 0);
    }

    bool OpenMPIMultiNode::isPowerOf2(const int& x) const
    {
        return x > 0 and not (x & (x - 1));
    }

    int OpenMPIMultiNode::numberOfLeafs(node<Rectangle<int>>* node) const
    {
        if (node == NULL) return 0;
        if (node->left == NULL and node->right == NULL) return 1;
        return numberOfLeafs(node->left) + numberOfLeafs(node->right);
    }

    bool OpenMPIMultiNode::stopProcreating(const int& currentHeight) const
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

    OpenMPIMultiNode::node<Rectangle<int>>* OpenMPIMultiNode::insertNode(const Rectangle<int>& rectangle, node<Rectangle<int>>* treeNode)
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

    void OpenMPIMultiNode::destroyTree(node<Rectangle<int>>* leaf)
    {
        if (leaf != NULL) 
        {
            destroyTree(leaf->left);
            destroyTree(leaf->right);
            delete leaf;
        }
    }

    double OpenMPIMultiNode::getAgentWeight(const Agent& agent) const
    {
        // Make a map<agentsId, agentsWeight> for more complex implementations
        return 1;  // Simplest implementation
    }

    double OpenMPIMultiNode::getAgentsWeight(const AgentsVector& agentsVector) const
    {
        double totalWeight = 0;
        for (int i = 0; i < agentsVector.size(); ++i) {
            totalWeight += getAgentWeight(*agentsVector[i]);
        }
        return totalWeight;
    }

    double OpenMPIMultiNode::getAllAgentsWeight() const
    {
        double totalWeight = 0;
        for (AgentsList::iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
            totalWeight += getAgentWeight(*it->get());
        return totalWeight;
    }

    AgentsVector OpenMPIMultiNode::getAgentsInPosition(const Point2D<int>& position, const std::string& type) const
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

    double OpenMPIMultiNode::getAgentsWeightFromCell(const int& row, const int& column) const
    {
        Point2D<int> position(row, column);
        AgentsVector agentsVector = getAgentsInPosition(position);
        return getAgentsWeight(agentsVector);
    }

    void OpenMPIMultiNode::exploreHorizontallyAndKeepDividing(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentHeight)
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

    void OpenMPIMultiNode::exploreVerticallyAndKeepDividing(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentHeight)
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

    void OpenMPIMultiNode::divideSpaceRecursively(node<Rectangle<int>>* treeNode, const double& totalWeight, const int& currentHeight)
    {
        if (stopProcreating(currentHeight)) return;

        bool landscape = treeNode->value.getSize().getWidth() > treeNode->value.getSize().getHeight();
        if (landscape)
            exploreHorizontallyAndKeepDividing(treeNode, totalWeight, currentHeight);
        else
            exploreVerticallyAndKeepDividing(treeNode, totalWeight, currentHeight);
    }

    void OpenMPIMultiNode::getPartitionsFromTree(node<Rectangle<int>>* node, std::vector<Rectangle<int>>& partitions) const
    {
        if (node == NULL) return;
        if (node->left == NULL and node->right == NULL) 
        {
            partitions.push_back(node->value);
            return;
        }
        getPartitionsFromTree(node->left, partitions);
        getPartitionsFromTree(node->right, partitions);
    }

    void OpenMPIMultiNode::addMPINodeInMapItIsNot(const std::vector<Rectangle<int>>& partitions, const int& neighbourIndex)
    {
        if (_mpiNodesMap.find(neighbourIndex) == _mpiNodesMap.end())
        {
            MPINode mpiNode;
            mpiNode.ownedArea = partitions[neighbourIndex];
            _mpiNodesMap[neighbourIndex] = mpiNode;
        }
    }

    void OpenMPIMultiNode::expandRectangle(Rectangle<int>& rectangle, const int& expansion) const
    {
        int leftMovement = std::min(rectangle.left() - _root->value.left(), expansion);
        int topMovement = std::min(rectangle.top() - _root->value.top(), expansion);

        rectangle.getOrigin().getX() -= leftMovement;
        rectangle.getOrigin().getY() -= topMovement;

        rectangle.getSize().getWidth() += std::min(_root->value.right() - rectangle.right(), expansion + leftMovement);
        rectangle.getSize().getHeight() += std::min(_root->value.bottom() - rectangle.bottom(), expansion + topMovement);
    }

    bool OpenMPIMultiNode::doOverlap(const Rectangle<int>& rectangleA, const Rectangle<int>& rectangleB) const
    {
        if (rectangleA.getOrigin().getX() >= rectangleB.right() + 1 or
            rectangleB.getOrigin().getX() >= rectangleA.right() + 1 or
            rectangleA.getOrigin().getY() >= rectangleB.bottom() + 1 or
            rectangleB.getOrigin().getY() >= rectangleA.bottom() + 1)
            return false;
        return true;
    }

    bool OpenMPIMultiNode::areTheyNeighbours(const Rectangle<int>& rectangleA, const Rectangle<int>& rectangleB) const
    {
        Rectangle<int> rectangleAWithOuterOverlap = rectangleA;
        expandRectangle(rectangleAWithOuterOverlap, _overlapSize);

        return doOverlap(rectangleAWithOuterOverlap, rectangleB);
    }

    void OpenMPIMultiNode::printNodes() const
    {
        std::cout << "Overlap size: " << _overlapSize << std::endl;
        for (std::map<int, MPINode>::const_iterator it = _mpiNodesMap.begin(); it != _mpiNodesMap.end(); ++it) 
        {
            std::cout << "ID: " << it->first << "\tleft: " << it->second.ownedArea.left() << " top: " << it->second.ownedArea.top() << " right: " << it->second.ownedArea.right()+1 << " bottom: " << it->second.ownedArea.bottom()+1 << "\tneighbours: ";
            for (std::list<int>::const_iterator it2 = it->second.neighbours.begin(); it2 != it->second.neighbours.end(); ++it2)
                std::cout << " " << *it2;
            std::cout << std::endl;
        }
    }

    void OpenMPIMultiNode::createNodesInformationToSend()
    {
        std::vector<Rectangle<int>> partitions;
        getPartitionsFromTree(_root, partitions);

        for (int i = 0; i < partitions.size(); ++i)
        {
            addMPINodeInMapItIsNot(partitions, i);

            for (int j = i+1; j < partitions.size(); ++j)
            {
                addMPINodeInMapItIsNot(partitions, j);

                if (areTheyNeighbours(partitions[i], partitions[j]))
                {
                    _mpiNodesMap[i].neighbours.push_back(j);
                    _mpiNodesMap[j].neighbours.push_back(i);
                }
            }
        }

        printNodes();
    }
    
} // namespace Engine