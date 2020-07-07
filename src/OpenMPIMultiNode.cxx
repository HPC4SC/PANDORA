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
#include <MpiFactory.hxx>

#include <math.h>

namespace Engine {

    /** PUBLIC METHODS **/

    OpenMPIMultiNode::OpenMPIMultiNode() : _initialTime(0.0f), _masterNodeID(0), _assignLoadToMasterNode(true)
    {
    }

    OpenMPIMultiNode::~OpenMPIMultiNode()
    {
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
        registerMPIStructs();

        _tree = new LoadBalanceTree();
        _tree->initializeTreeAndSetData(_world, _numTasks - int(not _assignLoadToMasterNode));
    }

    void OpenMPIMultiNode::initData() 
    {
        MpiFactory::instance()->registerTypes();

        if (getId() == _masterNodeID) 
        {
            _world->createRasters();
            _world->createAgents();

            divideSpace();                          printPartitionsBeforeMPI();
            sendSpacesToNodes();                    printOwnNodeStructureAfterMPI();

            createNeighbouringAgents();             printNeighbouringAgentsPerTypes();
            sendAgentsToNodes();                    printNodeAgents();

            sendRastersToNodes();                   printNodeRasters();
        }
        else 
        {
            receiveSpacesFromNode(_masterNodeID);   printOwnNodeStructureAfterMPI();
            receiveAgentsFromNode(_masterNodeID);   printNodeAgents();
            receiveRastersFromNode(_masterNodeID);  printNodeRasters();
        }

        MPI_Barrier(MPI_COMM_WORLD);

        //_serializer.init(*_world);
    }

    void OpenMPIMultiNode::setMasterNode(const int& masterNode)
    {
        _masterNodeID = masterNode;
    }

    void OpenMPIMultiNode::assignLoadToMasterNode(const bool& loadToMasterNode)
    {
        _assignLoadToMasterNode = loadToMasterNode;
    }

    Point2D<int> OpenMPIMultiNode::getRandomPosition() const
    {
        return Engine::Point2D<int>(GeneralState::statistics().getUniformDistValue(_nodeSpace.ownedArea.left(), _nodeSpace.ownedArea.right()),
                                    GeneralState::statistics().getUniformDistValue(_nodeSpace.ownedArea.top(), _nodeSpace.ownedArea.bottom()));
    }

    double OpenMPIMultiNode::getWallTime() const 
    {
        return MPI_Wtime() - _initialTime;
    }

    AgentsVector OpenMPIMultiNode::getAgent(const Point2D<int>& position, const std::string& type)
    {
        return _tree->getAgentsInPosition(position, type);
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
        _schedulerLogs = new OpenMPIMultiNodeLogs();
    }

    void OpenMPIMultiNode::stablishInitialBoundaries()
    {
        _boundaries = Rectangle<int>(_world->getConfig().getSize());

        _nodeSpace.ownedArea = _boundaries;
    }

    void OpenMPIMultiNode::registerMPIStructs()
    {
        _coordinatesDatatype = new MPI_Datatype;
        MPI_Type_contiguous(4, MPI_INT, _coordinatesDatatype);
        MPI_Type_commit(_coordinatesDatatype);
    }

    void OpenMPIMultiNode::divideSpace()
    {
        _tree->divideSpace();
        createNodesInformationToSend();

        if (not arePartitionsSuitable()) {
            std::cout << "Partitions not suitable. Maybe there are too many unnecessary MPI nodes for a small space, or the overlap size is too wide." << std::endl;
            exit(1);
        }
    }

    void OpenMPIMultiNode::sendSpacesToNodes()
    {
        int nodeID;
        for (std::map<int, MPINode>::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            nodeID = it->first;
            if (not _assignLoadToMasterNode and nodeID >= _masterNodeID) nodeID += 1;

            if (_assignLoadToMasterNode and nodeID == _masterNodeID) fillOwnStructures(it->second);
            else 
            {
                sendOwnAreaToNode(nodeID, it->second.ownedArea);
                sendNeighboursToNode(nodeID, it->second.neighbours);
            }
        }
    }

    void OpenMPIMultiNode::receiveSpacesFromNode(const int& masterNodeID)
    {
        MPINode mpiNode;
        receiveOwnAreaFromNode(masterNodeID, mpiNode);
        receiveNeighboursFromNode(masterNodeID, mpiNode);
        fillOwnStructures(mpiNode);
    }

    void OpenMPIMultiNode::generateOverlapAreas(MPINode& mpiNode)
    {
        mpiNode.ownedAreaWithOuterOverlaps = mpiNode.ownedArea;
        mpiNode.ownedAreaWithoutInnerOverlap = mpiNode.ownedArea;

        expandRectangleConsideringLimits(mpiNode.ownedAreaWithOuterOverlaps, _overlapSize);
        expandRectangleConsideringLimits(mpiNode.ownedAreaWithoutInnerOverlap, -_overlapSize, false);
    }

    void OpenMPIMultiNode::fillOwnStructures(const MPINode& mpiNodeInfo)
    {
        _nodeSpace.ownedArea = mpiNodeInfo.ownedArea;
        generateOverlapAreas(_nodeSpace);

        for (std::map<int, MPINode*>::const_iterator it = mpiNodeInfo.neighbours.begin(); it != mpiNodeInfo.neighbours.end(); ++it)
        {
            _nodeSpace.neighbours[it->first] = new MPINode;
            _nodeSpace.neighbours[it->first]->ownedArea = it->second->ownedArea;
            generateOverlapAreas(*(_nodeSpace.neighbours[it->first]));
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
    
    void OpenMPIMultiNode::sendNeighboursToNode(const int& nodeID, const std::map<int, MPINode*>& neighbours) const
    {
        int numberOfNeighbours = neighbours.size();
        MPI_Send(&numberOfNeighbours, 1, MPI_INTEGER, nodeID, eNumNeighbours, MPI_COMM_WORLD);

        for (std::map<int, MPINode*>::const_iterator it = neighbours.begin(); it != neighbours.end(); ++it)
        {
            int neighbourID = it->first;
            MPI_Send(&neighbourID, 1, MPI_INTEGER, nodeID, eNeighbourID, MPI_COMM_WORLD);

            Coordinates coordinates;
            coordinates.top = it->second->ownedArea.top();
            coordinates.left = it->second->ownedArea.left();
            coordinates.bottom = it->second->ownedArea.bottom();
            coordinates.right = it->second->ownedArea.right();
            MPI_Send(&coordinates, 1, *_coordinatesDatatype, nodeID, eCoordinatesNeighbour, MPI_COMM_WORLD);
        }
    }

    void OpenMPIMultiNode::receiveOwnAreaFromNode(const int& masterNodeID, MPINode& mpiNodeInfo) const
    {
        Coordinates coordinates;
        MPI_Recv(&coordinates, 1, *_coordinatesDatatype, masterNodeID, eCoordinates, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        mpiNodeInfo.ownedArea = Rectangle<int>(coordinates.left, coordinates.top, coordinates.right, coordinates.bottom);
    }

    void OpenMPIMultiNode::receiveNeighboursFromNode(const int& masterNodeID, MPINode& mpiNodeInfo) const
    {
        int numberOfNeighbours;
        MPI_Recv(&numberOfNeighbours, 1, MPI_INTEGER, masterNodeID, eNumNeighbours, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < numberOfNeighbours; ++i)
        {
            int neighbourID;
            MPI_Recv(&neighbourID, 1, MPI_INTEGER, masterNodeID, eNeighbourID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            Coordinates coordinates;
            MPI_Recv(&coordinates, 1, *_coordinatesDatatype, masterNodeID, eCoordinatesNeighbour, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            mpiNodeInfo.neighbours[neighbourID] = new MPINode;
            mpiNodeInfo.neighbours[neighbourID]->ownedArea = Rectangle<int>(coordinates.left, coordinates.top, coordinates.right, coordinates.bottom);
        }
    }

    void OpenMPIMultiNode::getBelongingNodesOfAgent(const Agent& agent, std::list<int>& agentNodes) const
    {
        int nodeID;

        for (std::map<int, MPINode>::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            nodeID = it->first;
            if (not _assignLoadToMasterNode and nodeID >= _masterNodeID) nodeID += 1;

            if (it->second.ownedAreaWithOuterOverlaps.contains(agent.getPosition())) agentNodes.push_back(nodeID);
        }
    }

    void OpenMPIMultiNode::createNeighbouringAgents()
    {
        int nodeID;
        std::string typeString;

        for (MpiFactory::TypesMap::const_iterator itType = MpiFactory::instance()->beginTypes(); itType != MpiFactory::instance()->endTypes(); ++itType)
        {
            typeString = itType->first;
            if (_neighbouringAgents.find(typeString) == _neighbouringAgents.end()) _neighbouringAgents[typeString] = std::map<int, AgentsList>();

            for (AgentsList::const_iterator itAgent = _world->beginAgents(); itAgent != _world->endAgents(); ++itAgent)
            {
                AgentPtr agent = *itAgent;
                if (agent->isType(typeString))
                {
                    std::list<int> belongingNodes;
                    getBelongingNodesOfAgent(*(agent.get()), belongingNodes);
                    for (std::list<int>::const_iterator itNodes = belongingNodes.begin(); itNodes != belongingNodes.end(); ++itNodes)
                    {
                        nodeID = *itNodes;
                        if (_neighbouringAgents.at(typeString).find(nodeID) == _neighbouringAgents.at(typeString).end())
                            _neighbouringAgents.at(typeString)[nodeID] = AgentsList();
                        _neighbouringAgents.at(typeString).at(nodeID).push_back(agent);
                    }
                }
            }
        }
    }

    void OpenMPIMultiNode::sendAgentsToNodeByType(const AgentsList& agentsToSend, const int& currentNode, const std::string& agentType)
    {
        int agentsTypeID = MpiFactory::instance()->getIDFromTypeName(agentType);
        MPI_Send(&agentsTypeID, 1, MPI_INTEGER, currentNode, eTypeID, MPI_COMM_WORLD);

        int numberOfAgentsToSend = agentsToSend.size();
        MPI_Send(&numberOfAgentsToSend, 1, MPI_INTEGER, currentNode, eNumAgents, MPI_COMM_WORLD);

        MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentType);
        for (AgentsList::const_iterator itAgent = agentsToSend.begin(); itAgent != agentsToSend.end(); ++itAgent)
        {
            Agent* agent = itAgent->get();

            void* agentPackage = agent->fillPackage();
            MPI_Send(agentPackage, 1, *agentTypeMPI, currentNode, eAgent, MPI_COMM_WORLD);
            free(agentPackage);
        }
    }

    bool OpenMPIMultiNode::isAgentInList(const Agent& agent, const AgentsList& agentsList) const
    {
        for (AgentsList::const_iterator it = agentsList.begin(); it != agentsList.end(); ++it)
        {
            Agent* agentFromList = it->get();
            if (agent.isEqual(*agentFromList)) return true;
        }
        return false;
    }

    AgentsList::const_iterator OpenMPIMultiNode::getAgentInWorldFromID(const std::string& agentID) const
    {
        for (AgentsList::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it) 
        {
            if (it->get()->getId().compare(agentID) == 0) return it;
        }
        return _world->endAgents();
    }

    void OpenMPIMultiNode::keepAgentsInNode(const AgentsList& agentsToKeep)
    {
        std::list<std::string> agentIDsToRemove;

        for (AgentsList::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            Agent* currentAgent = it->get();
            if (not isAgentInList(*currentAgent, agentsToKeep)) agentIDsToRemove.push_back(currentAgent->getId());
        }

        for (std::list<std::string>::const_iterator it = agentIDsToRemove.begin(); it != agentIDsToRemove.end(); ++it)
        {
            std::string agentID = *it;
            AgentsList::const_iterator agentIt = getAgentInWorldFromID(agentID);
            _world->eraseAgent(agentIt);
        }
    }

    void OpenMPIMultiNode::sendAgentsToNodes()
    {
        std::string currentType;
        std::map<int, AgentsList> agentsInNodes;
        int currentNode;
        AgentsList agentsToSend;
        
        AgentsList agentsToKeepInMasterNode;

        for (NeighbouringAgentsMap::const_iterator itType = _neighbouringAgents.begin(); itType != _neighbouringAgents.end(); ++itType)
        {
            currentType = itType->first;
            agentsInNodes = itType->second;

            for (std::map<int, AgentsList>::const_iterator itNode = agentsInNodes.begin(); itNode != agentsInNodes.end(); ++itNode)
            {
                currentNode = itNode->first;
                agentsToSend = itNode->second;

                if (_assignLoadToMasterNode and currentNode == _masterNodeID)
                    agentsToKeepInMasterNode.insert(agentsToKeepInMasterNode.end(), agentsToSend.begin(), agentsToSend.end());
                else
                    sendAgentsToNodeByType(agentsToSend, currentNode, currentType);
            }
        }

        keepAgentsInNode(agentsToKeepInMasterNode);
    }

    void OpenMPIMultiNode::sendRastersToNodes()
    {
        
    }

    void OpenMPIMultiNode::receiveAgentsFromNode(const int& masterNodeID)
    {
        int agentsTypeID;
        MPI_Recv(&agentsTypeID, 1, MPI_INTEGER, masterNodeID, eTypeID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::string agentsTypeName = MpiFactory::instance()->getNameFromTypeID(agentsTypeID);
        MPI_Datatype* agentType = MpiFactory::instance()->getMPIType(agentsTypeName);

        int numberOfAgentsToReceive;
        MPI_Recv(&numberOfAgentsToReceive, 1, MPI_INTEGER, masterNodeID, eNumAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < numberOfAgentsToReceive; ++i)
        {
            void* package = MpiFactory::instance()->createDefaultPackage(agentsTypeName);
            MPI_Recv(package, 1, *agentType, masterNodeID, eAgent, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            Agent* agent = MpiFactory::instance()->createAndFillAgent(agentsTypeName, package);
            free(package);

            _world->addAgent(agent, false);
        }
    }
    
    void OpenMPIMultiNode::receiveRastersFromNode(const int& masterNodeID)
    {

    }

    void OpenMPIMultiNode::addMPINodeToSendInMapItIsNot(const std::vector<Rectangle<int>>& partitions, const int& neighbourIndex)
    {
        if (_mpiNodesMapToSend.find(neighbourIndex) == _mpiNodesMapToSend.end())
        {
            MPINode mpiNode;
            mpiNode.ownedArea = partitions[neighbourIndex];
            generateOverlapAreas(mpiNode);

            _mpiNodesMapToSend[neighbourIndex] = mpiNode;
        }
    }

    void OpenMPIMultiNode::expandRectangleConsideringLimits(Rectangle<int>& rectangle, const int& expansion, const bool& contractInTheLimits) const
    {
        int topDifferenceAgainstLimits = rectangle.top() - _tree->getTree()->value.top();
        int leftDifferenceAgainstLimits = rectangle.left() - _tree->getTree()->value.left();

        int topMovement = std::min(topDifferenceAgainstLimits, expansion);
        if (not contractInTheLimits and topDifferenceAgainstLimits == 0) topMovement = 0;

        int leftMovement = std::min(leftDifferenceAgainstLimits, expansion);
        if (not contractInTheLimits and leftDifferenceAgainstLimits == 0) leftMovement = 0;

        rectangle.getOrigin().getY() -= topMovement;
        rectangle.getOrigin().getX() -= leftMovement;

        if (rectangle.bottom() > _tree->getTree()->value.bottom()) rectangle.getSize().getHeight() -= rectangle.bottom() - _tree->getTree()->value.bottom();
        if (rectangle.right() > _tree->getTree()->value.right()) rectangle.getSize().getWidth() -= rectangle.right() - _tree->getTree()->value.right();

        int bottomDifferenceAgainstLimits = _tree->getTree()->value.bottom() - rectangle.bottom();
        int rightDifferenceAgainstLimits = _tree->getTree()->value.right() - rectangle.right();

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
        _tree->getPartitionsFromTree(partitions);

        for (int i = 0; i < partitions.size(); ++i)
        {
            addMPINodeToSendInMapItIsNot(partitions, i);

            for (int j = i+1; j < partitions.size(); ++j)
            {
                addMPINodeToSendInMapItIsNot(partitions, j);

                if (areTheyNeighbours(partitions[i], partitions[j]))
                {
                    _mpiNodesMapToSend[i].neighbours[j] = new MPINode;
                    _mpiNodesMapToSend[i].neighbours[j]->ownedArea = partitions[j];

                    _mpiNodesMapToSend[j].neighbours[i] = new MPINode;
                    _mpiNodesMapToSend[j].neighbours[i]->ownedArea = partitions[i];
                }
            }
        }
    }

    bool OpenMPIMultiNode::arePartitionsSuitable()
    {
        for (std::map<int, MPINode>::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            if (it->second.ownedArea.getSize().getWidth() <= 2 * _overlapSize or
                it->second.ownedArea.getSize().getHeight() <= 2 * _overlapSize)
                return false;
        }
        return true;
    }

    /** DEBUGGING METHODS **/

    void OpenMPIMultiNode::printPartitionsBeforeMPI() const
    {
        _schedulerLogs->printPartitionsBeforeMPI(*this);
    }

    void OpenMPIMultiNode::printOwnNodeStructureAfterMPI() const
    {
        _schedulerLogs->printOwnNodeStructureAfterMPI(*this);
    }

    void OpenMPIMultiNode::printNeighbouringAgentsPerTypes() const
    {
        _schedulerLogs->printNeighbouringAgentsPerTypes(*this);
    }
    
    void OpenMPIMultiNode::printNodeAgents() const
    {
        _schedulerLogs->printNodeAgents(*this);
    }

    void OpenMPIMultiNode::printNodeRasters() const
    {
        _schedulerLogs->printNodeRasters(*this);
    }

} // namespace Engine