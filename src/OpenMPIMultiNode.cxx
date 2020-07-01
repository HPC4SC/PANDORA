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
#include <Logger.hxx>

#include <math.h>

namespace Engine {

    /** PUBLIC METHODS **/

    OpenMPIMultiNode::OpenMPIMultiNode() : _root(NULL), _initialTime(0.0f)
    {
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

        _initialTime = getWallTime();

        MPI_Comm_size(MPI_COMM_WORLD, &_numTasks);
        MPI_Comm_rank(MPI_COMM_WORLD, &_id);

        initLogFileNames();

        _updateKnowledgeInParallel = false;
        _executeActionsInParallel = false;
        omp_init_lock(&_ompLock);

        stablishInitialBoundaries();
        initializeTree();
        registerMPIStructs();
    }

    void OpenMPIMultiNode::divideSpace()
    {
        divideSpaceRecursively(_root, getAllAgentsWeight(), (int) std::ceil(std::log2(_numTasks)));
        createNodesInformationToSend();

        if (not arePartitionsSuitable()) {
            std::cout << "Partitions not suitable. Maybe there are too many unnecessary MPI nodes for a small space, or the overlap size is too wide." << std::endl;
            exit(1);
        }
    }

    void OpenMPIMultiNode::sendSpacesAmongNodes()
    {
        for (std::map<int, MPINodeToSend>::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            if (it->first == 0) fillOwnStructures(it->second);
            else 
            {
                sendOwnAreaToNode(it->first, it->second.ownedArea);
                sendNeighboursToNode(it->first, it->second.neighbours);
            }
        }
        printOwnNodeStructureAfterMPI();
    }

    void OpenMPIMultiNode::receiveSpacesFromMaster()
    {
        MPINodeToSend mpiNodeToSend;
        receiveOwnAreaFromNode(0, mpiNodeToSend);
        receiveNeighboursFromNode(0, mpiNodeToSend);
        fillOwnStructures(mpiNodeToSend);

        printOwnNodeStructureAfterMPI();
    }

    double OpenMPIMultiNode::getWallTime() const 
    {
        return MPI_Wtime() - _initialTime;
    }

    Point2D<int> OpenMPIMultiNode::getRandomPosition() const
    {
        return Engine::Point2D<int>(GeneralState::statistics().getUniformDistValue(_nodesSpace.ownedArea.left(), _nodesSpace.ownedArea.right()),
                                    GeneralState::statistics().getUniformDistValue(_nodesSpace.ownedArea.top(), _nodesSpace.ownedArea.bottom()));
    }

    AgentsVector OpenMPIMultiNode::getAgent(const Point2D<int>& position, const std::string& type)
    {
        return getAgentsInPosition(position, type);
        // + overlap agents?
    }

    /** PROTECTED METHODS **/

    void OpenMPIMultiNode::initLogFileNames()
    {
        for (int i = 0; i < _numTasks; ++i)
        {
            std::stringstream ss;
            ss << "MPIProcess_" << i << "_log.txt";
            _logFileNames[i] = ss.str();
        }
    }

    void OpenMPIMultiNode::stablishInitialBoundaries()
    {
        _boundaries = Rectangle<int>(_world->getConfig().getSize());

        _nodesSpace.ownedArea = _boundaries;
    }

    void OpenMPIMultiNode::initializeTree() 
    {
        _root = new node<Rectangle<int>>;
        _root->value = Rectangle<int>(_world->getConfig().getSize());
        _root->left = NULL;
        _root->right = NULL;
    }

    void OpenMPIMultiNode::registerMPIStructs()
    {
        _coordinatesDatatype = new MPI_Datatype;
        MPI_Type_contiguous(4, MPI_INT, _coordinatesDatatype);
        MPI_Type_commit(_coordinatesDatatype);
    }

    void OpenMPIMultiNode::generateOverlapAreas(MPINode& mpiNode)
    {
        mpiNode.ownedAreaWithOuterOverlaps = mpiNode.ownedArea;
        mpiNode.ownedAreaWithoutInnerOverlap = mpiNode.ownedArea;

        expandRectangleConsideringLimits(mpiNode.ownedAreaWithOuterOverlaps, _overlapSize);
        expandRectangleConsideringLimits(mpiNode.ownedAreaWithoutInnerOverlap, -_overlapSize, false);
    }

    void OpenMPIMultiNode::fillOwnStructures(const MPINodeToSend& mpiNodeInfo)
    {
        _nodesSpace.ownedArea = mpiNodeInfo.ownedArea;
        generateOverlapAreas(_nodesSpace);

        for (std::map<int, Rectangle<int>>::const_iterator it = mpiNodeInfo.neighbours.begin(); it != mpiNodeInfo.neighbours.end(); ++it)
        {
            _nodesSpace.neighbours[it->first] = new MPINode;
            _nodesSpace.neighbours[it->first]->ownedArea = it->second;
            generateOverlapAreas(*(_nodesSpace.neighbours[it->first]));
        }

    }

    void OpenMPIMultiNode::sendOwnAreaToNode(const int& nodeID, const Rectangle<int>& mpiNodeInfo) const
    {
        Coordinates coordinates;
        coordinates.top = mpiNodeInfo.top();
        coordinates.left = mpiNodeInfo.left();
        coordinates.bottom = mpiNodeInfo.bottom();
        coordinates.right = mpiNodeInfo.right();
        MPI_Send(&coordinates, 1, *_coordinatesDatatype, nodeID, eCoordinates, MPI_COMM_WORLD);
    }
    
    void OpenMPIMultiNode::sendNeighboursToNode(const int& nodeID, const std::map<int, Rectangle<int>>& neighbours) const
    {
        int numberOfNeighbours = neighbours.size();
        MPI_Send(&numberOfNeighbours, 1, MPI_INTEGER, nodeID, eNumNeighbours, MPI_COMM_WORLD);

        for (std::map<int, Rectangle<int>>::const_iterator it = neighbours.begin(); it != neighbours.end(); ++it)
        {
            int neighbourID = it->first;
            MPI_Send(&neighbourID, 1, MPI_INTEGER, nodeID, eNeighbourID, MPI_COMM_WORLD);

            Coordinates coordinates;
            coordinates.top = it->second.top();
            coordinates.left = it->second.left();
            coordinates.bottom = it->second.bottom();
            coordinates.right = it->second.right();
            MPI_Send(&coordinates, 1, *_coordinatesDatatype, nodeID, eCoordinatesNeighbour, MPI_COMM_WORLD);
        }
    }

    void OpenMPIMultiNode::receiveOwnAreaFromNode(const int& masterNodeID, MPINodeToSend& mpiNodeInfo) const
    {
        Coordinates coordinates;
        MPI_Recv(&coordinates, 1, *_coordinatesDatatype, masterNodeID, eCoordinates, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        mpiNodeInfo.ownedArea = Rectangle<int>(coordinates.left, coordinates.top, coordinates.right, coordinates.bottom);
    }

    void OpenMPIMultiNode::receiveNeighboursFromNode(const int& masterNodeID, MPINodeToSend& mpiNodeInfo) const
    {
        int numberOfNeighbours;
        MPI_Recv(&numberOfNeighbours, 1, MPI_INTEGER, masterNodeID, eNumNeighbours, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < numberOfNeighbours; ++i)
        {
            int neighbourID;
            MPI_Recv(&neighbourID, 1, MPI_INTEGER, masterNodeID, eNeighbourID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            Coordinates coordinates;
            MPI_Recv(&coordinates, 1, *_coordinatesDatatype, masterNodeID, eCoordinatesNeighbour, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            mpiNodeInfo.neighbours[neighbourID] = Rectangle<int>(coordinates.left, coordinates.top, coordinates.right, coordinates.bottom);
        }
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

    void OpenMPIMultiNode::addMPINodeToSendInMapItIsNot(const std::vector<Rectangle<int>>& partitions, const int& neighbourIndex)
    {
        if (_mpiNodesMapToSend.find(neighbourIndex) == _mpiNodesMapToSend.end())
        {
            MPINodeToSend mpiNode;
            mpiNode.ownedArea = partitions[neighbourIndex];
            _mpiNodesMapToSend[neighbourIndex] = mpiNode;
        }
    }

    void OpenMPIMultiNode::expandRectangleConsideringLimits(Rectangle<int>& rectangle, const int& expansion, const bool& contractInTheLimits) const
    {
        int topDifferenceAgainstLimits = rectangle.top() - _root->value.top();
        int leftDifferenceAgainstLimits = rectangle.left() - _root->value.left();

        int topMovement = std::min(topDifferenceAgainstLimits, expansion);
        if (not contractInTheLimits and topDifferenceAgainstLimits == 0) topMovement = 0;

        int leftMovement = std::min(leftDifferenceAgainstLimits, expansion);
        if (not contractInTheLimits and leftDifferenceAgainstLimits == 0) leftMovement = 0;

        rectangle.getOrigin().getY() -= topMovement;
        rectangle.getOrigin().getX() -= leftMovement;

        if (rectangle.bottom() > _root->value.bottom()) rectangle.getSize().getHeight() -= rectangle.bottom() - _root->value.bottom();
        if (rectangle.right() > _root->value.right()) rectangle.getSize().getWidth() -= rectangle.right() - _root->value.right();

        int bottomDifferenceAgainstLimits = _root->value.bottom() - rectangle.bottom();
        int rightDifferenceAgainstLimits = _root->value.right() - rectangle.right();

        int heightMovement = std::min(bottomDifferenceAgainstLimits, expansion + topMovement);
        if (not contractInTheLimits and bottomDifferenceAgainstLimits == 0) heightMovement = 0;

        int widthMovement = std::min(rightDifferenceAgainstLimits, expansion + leftMovement);
        if (not contractInTheLimits and rightDifferenceAgainstLimits == 0) widthMovement = 0;

        rectangle.getSize().getHeight() += heightMovement;
        rectangle.getSize().getWidth() += widthMovement;
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
        expandRectangleConsideringLimits(rectangleAWithOuterOverlap, _overlapSize);

        return doOverlap(rectangleAWithOuterOverlap, rectangleB);
    }

    void OpenMPIMultiNode::createNodesInformationToSend()
    {
        std::vector<Rectangle<int>> partitions;
        getPartitionsFromTree(_root, partitions);

        for (int i = 0; i < partitions.size(); ++i)
        {
            addMPINodeToSendInMapItIsNot(partitions, i);

            for (int j = i+1; j < partitions.size(); ++j)
            {
                addMPINodeToSendInMapItIsNot(partitions, j);

                if (areTheyNeighbours(partitions[i], partitions[j]))
                {
                    _mpiNodesMapToSend[i].neighbours[j] = partitions[j];
                    _mpiNodesMapToSend[j].neighbours[i] = partitions[i];
                }
            }
        }

        printNodesBeforeMPI();
    }

    bool OpenMPIMultiNode::arePartitionsSuitable()
    {
        for (std::map<int, MPINodeToSend>::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            if (it->second.ownedArea.getSize().getWidth() <= 2 * _overlapSize or
                it->second.ownedArea.getSize().getHeight() <= 2 * _overlapSize)
                return false;
        }
        return true;
    }

    void OpenMPIMultiNode::printNodesBeforeMPI() const
    {
        std::stringstream ss;
        ss << "TS = " << getWallTime() << ":" << std::endl;
        ss << "Overlap size: " << _overlapSize << std::endl;
        for (std::map<int, MPINodeToSend>::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it) 
        {
            ss << "ID: " << it->first << "\tCoordinates: " << it->second.ownedArea << std::endl;
            for (std::map<int, Rectangle<int>>::const_iterator it2 = it->second.neighbours.begin(); it2 != it->second.neighbours.end(); ++it2)
            {
                ss << "\tNeighbour " << it2->first << ": " << it2->second.top() << " " << it2->second.left() << " " << it2->second.bottom()+1 << " " << it2->second.right()+1 << std::endl;
            }
            ss << std::endl;
        }

        log_DEBUG(_logFileNames.at(getId()), ss.str());
    }

    void OpenMPIMultiNode::printOwnNodeStructureAfterMPI() const
    {
        std::stringstream ss;
        ss << "TS = " << getWallTime() << ":" << std::endl;
        ss << "Process #" << getId() << ":" << std::endl;
        ss << "ownedAreaWithoutInnerOverlap: " << _nodesSpace.ownedAreaWithoutInnerOverlap << std::endl;
        ss << "ownedArea: " << _nodesSpace.ownedArea << std::endl;
        ss << "ownedAreaWithOuterOverlaps: " << _nodesSpace.ownedAreaWithOuterOverlaps << std::endl;
        for (std::map<int, MPINode*>::const_iterator it = _nodesSpace.neighbours.begin(); it != _nodesSpace.neighbours.end(); ++it)
        {
            ss << "NeighbourID: " << it->first << "\tCoordinates: " << it->second->ownedArea << std::endl;
        }
        ss << std::endl;

        log_DEBUG(_logFileNames.at(getId()), ss.str());
    }
    
} // namespace Engine