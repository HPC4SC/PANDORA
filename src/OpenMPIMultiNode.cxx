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

#include <Exception.hxx>
#include <GeneralState.hxx>
#include <MpiFactory.hxx>

#include <math.h>
#include <string>
#include <string.h>

#include <unistd.h>

namespace Engine {

    /** INITIALIZATION PUBLIC METHODS **/

    OpenMPIMultiNode::OpenMPIMultiNode() : _initialTime(0.0f), _masterNodeID(0), _assignLoadToMasterNode(true), _serializer(*this), _printInConsole(false), _printInstrumentation(false)
    {
    }

    OpenMPIMultiNode::~OpenMPIMultiNode()
    {
    }

    void OpenMPIMultiNode::setMasterNode(const int& masterNode)
    {
        _masterNodeID = masterNode;
    }

    void OpenMPIMultiNode::assignLoadToMasterNode(const bool& loadToMasterNode)
    {
        _assignLoadToMasterNode = loadToMasterNode;
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

        omp_init_lock(&_ompLock);

        stablishInitialBoundaries();
        registerMPIStructs();

        _tree = new LoadBalanceTree();
        _tree->initializeTreeAndSetData(_world, _numTasks - int(not _assignLoadToMasterNode));
    }

    void OpenMPIMultiNode::initData() 
    {
        _serializer.init(*_world);

        MpiFactory::instance()->registerTypes();

        createRasters();

        if (getId() == _masterNodeID) 
        {
            createAgents();

            divideSpace();                                  printPartitionsBeforeMPI();
            sendInitialSpacesToNodes();                     printOwnNodeStructureAfterMPI();

            createNeighbouringAgents();                     printNeighbouringAgentsPerTypes();
            sendInitialAgentsToNodes();                     printNodeAgents();

            printNodeRasters();
        }
        else 
        {
            receiveInitialSpacesFromNode(_masterNodeID);    printOwnNodeStructureAfterMPI();
            receiveInitialAgentsFromNode(_masterNodeID);    printNodeAgents();
            
            printNodeRasters();
        }

if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("\n").str());

        stablishBoundaries();

        MPI_Barrier(MPI_COMM_WORLD);
    }

    /** INITIALIZATION PROTECTED METHODS **/

    void OpenMPIMultiNode::initLogFileNames()
    {
        _schedulerLogs = new OpenMPIMultiNodeLogs();
        _schedulerLogs->initLogFileNames(*this);
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

        _positionAndValueDatatype = new MPI_Datatype;
        MPI_Type_contiguous(3, MPI_INT, _positionAndValueDatatype);
        MPI_Type_commit(_positionAndValueDatatype);
    }

    void OpenMPIMultiNode::createRasters()
    {
        double initialTime = getWallTime();

        _world->createRasters();

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] createRasters()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::createAgents()
    {
        double initialTime = getWallTime();

        _world->createAgents();

        double endTime = getWallTime();

std::stringstream ss;
ss << "TotalTime OUT: " << endTime - initialTime << "\n";
std::cout << ss.str();

if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] createAgents()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::divideSpace()
    {
        double initialTime = getWallTime();

        _tree->divideSpace();

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] divideSpace()\tTOTAL TIME: " << endTime - initialTime).str());

        initialTime = getWallTime();

        createNodesInformationToSend();

        endTime = getWallTime();

if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] createNodesInformationToSend()\tTOTAL TIME: " << endTime - initialTime).str());

        if (not arePartitionsSuitable()) {
            throw Exception(CreateStringStream("[Process # " << getId() <<  "] Partitions not suitable. Maybe there are too many unnecessary MPI nodes for such a small space, or the overlap size is too wide.").str());
            exit(1);
        }
    }

    void OpenMPIMultiNode::sendInitialSpacesToNodes()
    {
        double initialTime = getWallTime();

        for (MPINodesMap::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            int nodeID = it->first;
            if (not _assignLoadToMasterNode and nodeID >= _masterNodeID) nodeID += 1;

            if (_assignLoadToMasterNode and nodeID == _masterNodeID) fillOwnStructures(it->second);
            else 
            {
                sendOwnAreaToNode(nodeID, it->second.ownedArea);
                sendNeighboursToNode(nodeID, it->second.neighbours);
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] sendInitialSpacesToNodes()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::receiveInitialSpacesFromNode(const int& masterNodeID)
    {
        double initialTime = getWallTime();

        MPINode mpiNode;
        receiveOwnAreaFromNode(masterNodeID, mpiNode);
        receiveNeighboursFromNode(masterNodeID, mpiNode);
        fillOwnStructures(mpiNode);

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] receiveInitialSpacesFromNode()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::generateOverlapAreas(MPINode& mpiNode) const
    {
        mpiNode.ownedAreaWithOuterOverlaps = mpiNode.ownedArea;
        mpiNode.ownedAreaWithoutInnerOverlap = mpiNode.ownedArea;

        if (_numTasks == 1) return;

        expandRectangleConsideringLimits(mpiNode.ownedAreaWithOuterOverlaps, _overlapSize);
        expandRectangleConsideringLimits(mpiNode.ownedAreaWithoutInnerOverlap, -_overlapSize, false);
    }

    Rectangle<int> OpenMPIMultiNode::squeezeRectangle(const Rectangle<int>& rectangle, const int& squeezeTop, const int& squeezeBottom, const int& squeezeLeft, const int& squeezeRight) const
    {
        Rectangle<int> resultingRectangle = rectangle;

        resultingRectangle.getOrigin().getY() += squeezeTop;
        resultingRectangle.getSize().getHeight() -= (squeezeTop + squeezeBottom);

        resultingRectangle.getOrigin().getX() += squeezeLeft;
        resultingRectangle.getSize().getWidth() -= (squeezeLeft + squeezeRight);

        return resultingRectangle;
    }

    void OpenMPIMultiNode::generateInnerSubOverlapAreas(MPINode& mpiNode) const
    {
        if (_numTasks == 1) return;

        mpiNode.innerSubOverlaps[eTopCenter] =      squeezeRectangle(mpiNode.ownedArea, 0, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, _overlapSize, _overlapSize);
        mpiNode.innerSubOverlaps[eBottomCenter] =   squeezeRectangle(mpiNode.ownedArea, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, 0, _overlapSize, _overlapSize);
        mpiNode.innerSubOverlaps[eLeftCenter] =     squeezeRectangle(mpiNode.ownedArea, _overlapSize, _overlapSize, 0, mpiNode.ownedArea.getSize().getWidth() - _overlapSize);
        mpiNode.innerSubOverlaps[eRightCenter] =    squeezeRectangle(mpiNode.ownedArea, _overlapSize, _overlapSize, mpiNode.ownedArea.getSize().getWidth() - _overlapSize, 0);

        mpiNode.innerSubOverlaps[eTopLeft] =        squeezeRectangle(mpiNode.ownedArea, 0, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, 0, mpiNode.ownedArea.getSize().getWidth() - _overlapSize);
        mpiNode.innerSubOverlaps[eTopRight] =       squeezeRectangle(mpiNode.ownedArea, 0, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, mpiNode.ownedArea.getSize().getWidth() - _overlapSize, 0);
        mpiNode.innerSubOverlaps[eBottomLeft] =     squeezeRectangle(mpiNode.ownedArea, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, 0, 0, mpiNode.ownedArea.getSize().getWidth() - _overlapSize);
        mpiNode.innerSubOverlaps[eBottomRight] =    squeezeRectangle(mpiNode.ownedArea, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, 0, mpiNode.ownedArea.getSize().getWidth() - _overlapSize, 0);
    }

    void OpenMPIMultiNode::generateInnerSubOverlapNeighbours(MPINode& mpiNode, const int& neighbourID, const MPINode& neighbourNode) const
    {
        Rectangle<int> neighbourArea = neighbourNode.ownedArea;

        for (std::map<int, Rectangle<int>>::const_iterator it = mpiNode.innerSubOverlaps.begin(); it != mpiNode.innerSubOverlaps.end(); ++it)
        {
            int subOverlapID = it->first;
            Rectangle<int> subOverlapArea = it->second;

            if (mpiNode.innerSubOverlapsNeighbours.find(subOverlapID) == mpiNode.innerSubOverlapsNeighbours.end())
                mpiNode.innerSubOverlapsNeighbours[subOverlapID] = std::list<int>();

            if (areTheyNeighbours(subOverlapArea, neighbourArea))
                mpiNode.innerSubOverlapsNeighbours.at(subOverlapID).push_back(neighbourID);
        }
    }

    void OpenMPIMultiNode::fillOwnStructures(const MPINode& mpiNodeInfo)
    {
        _nodeSpace.ownedArea = mpiNodeInfo.ownedArea;

        generateOverlapAreas(_nodeSpace);
        generateInnerSubOverlapAreas(_nodeSpace);

        for (std::map<int, MPINode*>::const_iterator it = mpiNodeInfo.neighbours.begin(); it != mpiNodeInfo.neighbours.end(); ++it)
        {
            int neighbourID = it->first;
            MPINode* mpiNode = it->second;

            _nodeSpace.neighbours[neighbourID] = new MPINode;
            _nodeSpace.neighbours[neighbourID]->ownedArea = mpiNode->ownedArea;
            generateOverlapAreas(*(_nodeSpace.neighbours[neighbourID]));

            generateInnerSubOverlapNeighbours(_nodeSpace, neighbourID, *(mpiNode));
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
        for (MPINodesMap::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            int nodeID = it->first;
            if (not _assignLoadToMasterNode and nodeID >= _masterNodeID) nodeID += 1;

            if (it->second.ownedAreaWithOuterOverlaps.contains(agent.getPosition())) agentNodes.push_back(nodeID);
        }
    }

    void OpenMPIMultiNode::createNeighbouringAgents()
    {
        double initialTime = getWallTime();

        if (_numTasks == 1) return;

        for (AgentsList::const_iterator itAgent = _world->beginAgents(); itAgent != _world->endAgents(); ++itAgent)
        {
            AgentPtr agent = *itAgent;
            std::string agentType = agent->getType();
            
            std::list<int> belongingNodes;
            getBelongingNodesOfAgent(*(agent.get()), belongingNodes);
            for (std::list<int>::const_iterator itNodes = belongingNodes.begin(); itNodes != belongingNodes.end(); ++itNodes)
            {
                int nodeID = *itNodes;
                if (_neighbouringAgents.find(nodeID) == _neighbouringAgents.end())
                    _neighbouringAgents[nodeID] = std::map<std::string, AgentsList>();

                if (_neighbouringAgents.at(nodeID).find(agentType) == _neighbouringAgents.at(nodeID).end())
                    _neighbouringAgents.at(nodeID)[agentType] = AgentsList();
                
                _neighbouringAgents.at(nodeID).at(agentType).push_back(agent);
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] createNeighbouringAgents()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::sendAgentsToNodeByType(const AgentsList& agentsToSend, const int& currentNode, const std::string& agentsTypeName) const
    {
        int agentsTypeID = MpiFactory::instance()->getIDFromTypeName(agentsTypeName);
        MPI_Send(&agentsTypeID, 1, MPI_INTEGER, currentNode, eAgentTypeID, MPI_COMM_WORLD);

        int numberOfAgentsToSend = agentsToSend.size();
        MPI_Send(&numberOfAgentsToSend, 1, MPI_INTEGER, currentNode, eNumAgents, MPI_COMM_WORLD);

        int sizeOfAgentPackage = MpiFactory::instance()->getSizeOfPackage(agentsTypeName);
        void* agentsPackageArray = malloc(numberOfAgentsToSend * sizeOfAgentPackage);

        int i = 0;
        MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentsTypeName);
        for (AgentsList::const_iterator itAgent = agentsToSend.begin(); itAgent != agentsToSend.end(); ++itAgent)
        {
            Agent* agent = itAgent->get();

            void* agentPackage = agent->fillPackage();
            memcpy((char*) agentsPackageArray + i * sizeOfAgentPackage, agentPackage, sizeOfAgentPackage);
            agent->freePackage(agentPackage);
            ++i;
        }
        MPI_Send(agentsPackageArray, numberOfAgentsToSend, *agentTypeMPI, currentNode, eAgent, MPI_COMM_WORLD);

        free(agentsPackageArray);
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

    void OpenMPIMultiNode::removeAgentsFromID(const std::list<std::string>& agentIDsToRemove)
    {
        for (std::list<std::string>::const_iterator it = agentIDsToRemove.begin(); it != agentIDsToRemove.end(); ++it)
        {
            std::string agentID = *it;
            AgentsList::const_iterator agentIt = getAgentInWorldFromID(agentID);
            _world->eraseAgent(agentIt);
        }
    }

    void OpenMPIMultiNode::keepAgentsInNode(const AgentsList& agentsToKeep)
    {
        std::list<std::string> agentIDsToRemove;

        for (AgentsList::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            Agent* agent = it->get();
            if (not isAgentInList(*agent, agentsToKeep)) agentIDsToRemove.push_back(agent->getId());
        }

        removeAgentsFromID(agentIDsToRemove);
    }

    void OpenMPIMultiNode::sendInitialAgentsToNodes()
    {
        double initialTime = getWallTime();

        if (_numTasks == 1) return;

        AgentsList agentsToKeepInMasterNode;

        for (NeighbouringAgentsMap::const_iterator itNode = _neighbouringAgents.begin(); itNode != _neighbouringAgents.end(); ++itNode)
        {
            int nodeID = itNode->first;
            std::map<std::string, AgentsList> agentsByType = itNode->second;

            bool keepAgentsInCurrentNodeID = nodeID == _masterNodeID and _assignLoadToMasterNode;
            bool needToSendAgentsToNodeID = not keepAgentsInCurrentNodeID;

            if (needToSendAgentsToNodeID)
            {
                int numberOfTypesOfAgentsToSend = agentsByType.size();
                MPI_Send(&numberOfTypesOfAgentsToSend, 1, MPI_INTEGER, nodeID, eNumTypes, MPI_COMM_WORLD);
            }

            for (std::map<std::string, AgentsList>::const_iterator itType = agentsByType.begin(); itType != agentsByType.end(); ++itType)
            {
                std::string agentTypeName = itType->first;
                AgentsList agentsToSend = itType->second;

                if (needToSendAgentsToNodeID)
                    sendAgentsToNodeByType(agentsToSend, nodeID, agentTypeName);
                else
                    agentsToKeepInMasterNode.insert(agentsToKeepInMasterNode.end(), agentsToSend.begin(), agentsToSend.end());
            }
        }

        keepAgentsInNode(agentsToKeepInMasterNode);

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] sendInitialAgentsToNodes()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::receiveInitialAgentsFromNode(const int& masterNodeID)
    {
        double initialTime = getWallTime();

        int numberOfTypesOfAgents;
        MPI_Recv(&numberOfTypesOfAgents, 1, MPI_INTEGER, masterNodeID, eNumTypes, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < numberOfTypesOfAgents; ++i)
        {
            int agentsTypeID;
            MPI_Recv(&agentsTypeID, 1, MPI_INTEGER, masterNodeID, eAgentTypeID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::string agentsTypeName = MpiFactory::instance()->getNameFromTypeID(agentsTypeID);
            MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentsTypeName);

            int numberOfAgentsToReceive;
            MPI_Recv(&numberOfAgentsToReceive, 1, MPI_INTEGER, masterNodeID, eNumAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int sizeOfAgentPackage = MpiFactory::instance()->getSizeOfPackage(agentsTypeName);
            void* agentsPackageArray = malloc(numberOfAgentsToReceive * sizeOfAgentPackage);

            MPI_Recv(agentsPackageArray, numberOfAgentsToReceive, *agentTypeMPI, masterNodeID, eAgent, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = 0; j < numberOfAgentsToReceive; ++j)
            {
                void* package = (char*) agentsPackageArray + j * sizeOfAgentPackage;
                Agent* agent = MpiFactory::instance()->createAndFillAgent(agentsTypeName, package);

                _world->addAgent(agent, false);
            }
            free(agentsPackageArray);
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] receiveInitialAgentsFromNode()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::stablishBoundaries()
    {
        _boundaries = _nodeSpace.ownedArea;
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
        Rectangle<int> rectangleBWithOuterOverlap = rectangleB;
        expandRectangleConsideringLimits(rectangleAWithOuterOverlap, _overlapSize);
        expandRectangleConsideringLimits(rectangleBWithOuterOverlap, _overlapSize);

        return doOverlap(rectangleAWithOuterOverlap, rectangleBWithOuterOverlap);
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
        for (MPINodesMap::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            if (it->second.ownedArea.getSize().getWidth() <= 2 * _overlapSize or
                it->second.ownedArea.getSize().getHeight() <= 2 * _overlapSize)
                return false;
        }
        return true;
    }

    /** INITIALIZATION DEBUGGING METHODS **/

    void OpenMPIMultiNode::printPartitionsBeforeMPI() const
    {
        _schedulerLogs->printPartitionsBeforeMPIInDebugFile(*this);
    }

    void OpenMPIMultiNode::printOwnNodeStructureAfterMPI() const
    {
        _schedulerLogs->printOwnNodeStructureAfterMPIInDebugFile(*this);
    }

    void OpenMPIMultiNode::printNeighbouringAgentsPerTypes() const
    {
        _schedulerLogs->printNeighbouringAgentsPerTypesInDebugFile(*this);
    }
    
    void OpenMPIMultiNode::printNodeAgents() const
    {
        _schedulerLogs->printNodeAgentsInDebugFile(*this);
    }

    void OpenMPIMultiNode::printNodeRasters() const
    {
        _schedulerLogs->printNodeRastersInDebugFile(*this);
    }

    /** RUN PROTECTED METHODS (CALLED BY INHERIT METHODS) **/

    void OpenMPIMultiNode::initializeAgentsAndRastersState()
    {
        _executedAgentsInStep.clear();
        _sentRastersInStep.clear();

        for (int rasterIndex = 0; rasterIndex < _world->getNumberOfRasters(); ++rasterIndex)
        {
            if (_world->rasterExists(rasterIndex) and _world->isRasterDynamic(rasterIndex))
                _sentRastersInStep[rasterIndex] = MapOfPositionsAndValues();
        }
    }

    void OpenMPIMultiNode::randomlyExecuteAgents(AgentsVector& agentsToExecute)
    {
        GeneralState::statistics().shuffleWithinIterators(agentsToExecute.begin(), agentsToExecute.end());

        #pragma omp parallel for schedule(dynamic) if(_updateKnowledgeInParallel)
        for (int i = 0; i < agentsToExecute.size(); ++i)
        {
            Agent* agent = agentsToExecute[i].get();
            agent->updateKnowledge();
            agent->selectActions();
        }

        #pragma omp parallel for schedule(dynamic) if(_executeActionsInParallel)
        for (int i = 0; i < agentsToExecute.size(); ++i)
        {
            Agent* agent = agentsToExecute[i].get();
            agent->executeActions();
            agent->updateState();
        }
    }

    bool OpenMPIMultiNode::hasBeenExecuted(const std::string& agentID) const
    {
        return not (_executedAgentsInStep.find(agentID) == _executedAgentsInStep.end());
    }

    void OpenMPIMultiNode::executeAgentsInSubOverlap(AgentsVector& agentsVector, const int& subOverlapID)
    {
        double initialTime = getWallTime();

        Rectangle<int> areaToExecute;
        if (subOverlapID == 0)  areaToExecute = _nodeSpace.ownedAreaWithoutInnerOverlap;
        else                    areaToExecute = _nodeSpace.innerSubOverlaps[subOverlapID];

        AgentsVector agentsToExecute;
        for (AgentsList::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            AgentPtr agentPtr = *it;
            std::string agentID = agentPtr->getId();
            if (agentPtr->exists() and not hasBeenExecuted(agentID) and areaToExecute.contains(agentPtr->getPosition()))
            {
                _executedAgentsInStep.insert(agentID);
                agentsToExecute.push_back(agentPtr);
            }
        }
        randomlyExecuteAgents(agentsToExecute);

        agentsVector.insert(agentsVector.end(), agentsToExecute.begin(), agentsToExecute.end());

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("\n[Process # " << getId() <<  "] executeAgentsInSubOverlap() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::initializeAgentsToSendMap(std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode) const
    {
        for (std::map<int, MPINode*>::const_iterator itNode = _nodeSpace.neighbours.begin(); itNode != _nodeSpace.neighbours.end(); ++itNode)
        {
            int neighbourID = itNode->first;
            if (neighbourID == _masterNodeID and not _assignLoadToMasterNode) continue;

            agentsByTypeAndNode[neighbourID] = std::map<std::string, AgentsList>();
        }
    }

    void OpenMPIMultiNode::sendDataRequestToNode(void* data, const int& numberOfElements, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag)
    {
        MPI_Request* mpiRequest = new MPI_Request;

        MPI_Isend(data, numberOfElements, mpiDatatype, destinationNode, tag, MPI_COMM_WORLD, mpiRequest);
        _sendRequests.push_back(mpiRequest);
    }

    void OpenMPIMultiNode::sendGhostAgentsInMap(const std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode, const int& subOverlapID)
    {
        double initialTime = getWallTime();

        for (std::map<int, std::map<std::string, AgentsList>>::const_iterator itNeighbourNode = agentsByTypeAndNode.begin(); itNeighbourNode != agentsByTypeAndNode.end(); ++itNeighbourNode)
        {
            int neighbourNodeID = itNeighbourNode->first;
            std::map<std::string, AgentsList> agentsByType = itNeighbourNode->second;

            int numberOfAgentTypesToSend = agentsByType.size();
            sendDataRequestToNode(&numberOfAgentTypesToSend, 1, MPI_INTEGER, neighbourNodeID, eNumGhostAgentsType);

            for (std::map<std::string, AgentsList>::const_iterator itType = agentsByType.begin(); itType != agentsByType.end(); ++itType)
            {
                std::string agentsTypeName = itType->first;
                AgentsList agentsToSend = itType->second;

                int agentsTypeID = MpiFactory::instance()->getIDFromTypeName(agentsTypeName);
                sendDataRequestToNode(&agentsTypeID, 1, MPI_INTEGER, neighbourNodeID, eGhostAgentsType);

                int numberOfAgentsToSend = agentsToSend.size();
                sendDataRequestToNode(&numberOfAgentsToSend, 1, MPI_INTEGER, neighbourNodeID, eNumGhostAgents);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " sending numberOfAgents: " << numberOfAgentsToSend << " of type: " << agentsTypeName << "\tto node: " << neighbourNodeID << "\n").str();

                int sizeOfAgentPackage = MpiFactory::instance()->getSizeOfPackage(agentsTypeName);
                void* agentsPackageArray = malloc(numberOfAgentsToSend * sizeOfAgentPackage);

                int i = 0;
                MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentsTypeName);
                for (AgentsList::const_iterator itAgent = agentsToSend.begin(); itAgent != agentsToSend.end(); ++itAgent)
                {
                    Agent* agent = itAgent->get();

 if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " sending agent: " << agent << "\tto node: " << neighbourNodeID << "\n").str();

                    void* agentPackage = agent->fillPackage();
                    memcpy((char*) agentsPackageArray + i * sizeOfAgentPackage, agentPackage, sizeOfAgentPackage);
                    agent->freePackage(agentPackage);
                    ++i;
                }
                sendDataRequestToNode(agentsPackageArray, numberOfAgentsToSend, *agentTypeMPI, neighbourNodeID, eGhostAgents);
                
                //free(agentsPackageArray);
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] sendGhostAgentsInMap() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::receiveGhostAgentsFromNeighbouringNodes(const int& subOverlapID)
    {
        double initialTime = getWallTime();

        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int sendingNodeID = it->first;

            int numberOfAgentTypesToReceive;
            MPI_Recv(&numberOfAgentTypesToReceive, 1, MPI_INTEGER, sendingNodeID, eNumGhostAgentsType, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int i = 0; i < numberOfAgentTypesToReceive; ++i)
            {
                int agentTypeID;
                MPI_Recv(&agentTypeID, 1, MPI_INTEGER, sendingNodeID, eGhostAgentsType, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::string agentsTypeName = MpiFactory::instance()->getNameFromTypeID(agentTypeID);
                MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentsTypeName);

                int numberOfAgentsToReceive;
                MPI_Recv(&numberOfAgentsToReceive, 1, MPI_INTEGER, sendingNodeID, eNumGhostAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " receiving numberOfAgentsToReceive: " << numberOfAgentsToReceive << " of type: " << agentsTypeName << "\tfrom node: " << sendingNodeID << "\n").str();

                int sizeOfAgentPackage = MpiFactory::instance()->getSizeOfPackage(agentsTypeName);
                void* agentsPackageArray = malloc(numberOfAgentsToReceive * sizeOfAgentPackage);

                MPI_Recv(agentsPackageArray, numberOfAgentsToReceive, *agentTypeMPI, sendingNodeID, eGhostAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int j = 0; j < numberOfAgentsToReceive; ++j)
                {
                    void* package = (char*) agentsPackageArray + j * sizeOfAgentPackage;
                    Agent* agent = MpiFactory::instance()->createAndFillAgent(agentsTypeName, package);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " receiving agent: " << agent << "\tfrom node: " << sendingNodeID << "\n").str();

                    AgentsList::const_iterator agentIt = getAgentIteratorFromID(agent->getId());
                    if (agentIt != _world->endAgents())
                        _world->eraseAgent(agentIt);

                    if (_nodeSpace.ownedAreaWithOuterOverlaps.contains(agent->getPosition()))
                    {
                        _world->addAgent(agent, true);
                        _executedAgentsInStep.insert(agent->getId());
                    }
                }
                free(agentsPackageArray);
            }
        }

        _world->sortAgentsListAlphabetically();     // Important when receiving agents asynchronously! Subsequent shuffles must obtain the same resulting list, so we need to sort the list of agents.

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] receiveGhostAgentsFromNeighbouringNodes() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    std::list<int> OpenMPIMultiNode::getRealNeighboursForAgent(const std::list<int>& potentialNeighbours, const Point2D<int>& agentPosition) const
    {
        std::list<int> resultingList = std::list<int>();

        for (std::list<int>::const_iterator potentialNeighboursIt = potentialNeighbours.begin(); potentialNeighboursIt != potentialNeighbours.end(); ++potentialNeighboursIt)
        {
            int potentialNeighbourID = *potentialNeighboursIt;
            Rectangle<int> potentialNeighbourArea = _nodeSpace.neighbours.at(potentialNeighbourID)->ownedAreaWithOuterOverlaps;

            if (potentialNeighbourArea.contains(agentPosition))
                resultingList.push_back(potentialNeighbourID);
        }
        return resultingList;
    }

    void OpenMPIMultiNode::getNeighboursToSendAgentInsideInnerSubOverlap(std::list<int>& neighbouringNodeIDs, const Agent& agent) const
    {
        for (std::map<int, Rectangle<int>>::const_iterator subOverlapsIt = _nodeSpace.innerSubOverlaps.begin(); subOverlapsIt != _nodeSpace.innerSubOverlaps.end(); ++subOverlapsIt)
        {
            int currentSubOverlapID = subOverlapsIt->first;
            Rectangle<int> subOverlapArea = subOverlapsIt->second;

            if (subOverlapArea.contains(agent.getPosition()))
            {
                std::list<int> potentialNeighbours = _nodeSpace.innerSubOverlapsNeighbours.at(currentSubOverlapID);
                neighbouringNodeIDs = getRealNeighboursForAgent(potentialNeighbours, agent.getPosition());
                break;
            }
        }
    }

    void OpenMPIMultiNode::synchronizeAgentsIfNecessary(const AgentsVector& agentsVector, const int& subOverlapID)
    {
        if (_numTasks == 1) return;

        std::map<int, std::map<std::string, AgentsList>> agentsByTypeAndNode;
        initializeAgentsToSendMap(agentsByTypeAndNode);

        for (AgentsVector::const_iterator itAgent = agentsVector.begin(); itAgent != agentsVector.end(); ++itAgent)
        {
            AgentPtr agentPtr = *itAgent;
            Agent* agent = agentPtr.get();
            std::string agentType = agent->getType();

            Point2D<int> agentPosition = agent->getPosition();

            bool isInInnerOverlapArea = _nodeSpace.ownedArea.contains(agentPosition) and not _nodeSpace.ownedAreaWithoutInnerOverlap.contains(agentPosition);

            std::list<int> neighbouringNodeIDs;
            if (isInInnerOverlapArea)   // Case #2 (no action needed for Case #1)
                getNeighboursToSendAgentInsideInnerSubOverlap(neighbouringNodeIDs, *agent);

            for (std::list<int>::const_iterator itNeighbourNodeID = neighbouringNodeIDs.begin(); itNeighbourNodeID != neighbouringNodeIDs.end(); ++itNeighbourNodeID)
            {
                int neighbourNodeID = *itNeighbourNodeID;

                if (agentsByTypeAndNode.at(neighbourNodeID).find(agentType) == agentsByTypeAndNode.at(neighbourNodeID).end())
                    agentsByTypeAndNode.at(neighbourNodeID)[agentType] = AgentsList();

                agentsByTypeAndNode.at(neighbourNodeID).at(agentType).push_back(agentPtr);
            }
        }

        sendGhostAgentsInMap(agentsByTypeAndNode, subOverlapID);
        receiveGhostAgentsFromNeighbouringNodes(subOverlapID);
    }

    void OpenMPIMultiNode::addSubOverlapNeighboursFromPosition(std::list<int>& subOverlapNeighboursIDs, const int& subOverlapID, const Point2D<int>& agentPosition) const
    {
        if (subOverlapID >= 1 and subOverlapID <= eNumberOfSubOverlaps)
        {
            std::list<int> potentialNeighbours = _nodeSpace.innerSubOverlapsNeighbours.at(subOverlapID);
            std::list<int> realNeighbours = getRealNeighboursForAgent(potentialNeighbours, agentPosition);

            subOverlapNeighboursIDs.insert(subOverlapNeighboursIDs.end(), realNeighbours.begin(), realNeighbours.end());
            subOverlapNeighboursIDs.unique();
        }
    }

    void OpenMPIMultiNode::getNeighboursThatNeedToRemoveAgent(std::list<int>& subOverlapNeighboursIDs, const int& originalSubOverlapAreaID, const Agent& agent) const
    {
        addSubOverlapNeighboursFromPosition(subOverlapNeighboursIDs, originalSubOverlapAreaID, agent.getDiscretePosition());
    }

    void OpenMPIMultiNode::getNeighboursThatNeedToAddAgent(std::list<int>& subOverlapNeighboursIDs, const int& originalSubOverlapAreaID, const Agent& agent) const
    {
        addSubOverlapNeighboursFromPosition(subOverlapNeighboursIDs, originalSubOverlapAreaID, agent.getPosition());
    }

    void OpenMPIMultiNode::sendGhostAgentsToNeighbours(const AgentsVector& agentsVector, const int& originalSubOverlapAreaID)
    {
        std::map<int, std::map<std::string, AgentsList>> agentsByTypeAndNode;
        initializeAgentsToSendMap(agentsByTypeAndNode);

        for (AgentsVector::const_iterator itAgent = agentsVector.begin(); itAgent != agentsVector.end(); ++itAgent)
        {
            AgentPtr agentPtr = *itAgent;
            Agent* agent = agentPtr.get();
            std::string agentType = agent->getType();

            Point2D<int> agentPosition = agent->getPosition();

            bool isInInnerMostArea = _nodeSpace.ownedAreaWithoutInnerOverlap.contains(agentPosition);

            std::list<int> neighbouringNodeIDs; 
            if (isInInnerMostArea)      // Case #3
                getNeighboursThatNeedToRemoveAgent(neighbouringNodeIDs, originalSubOverlapAreaID, *agent);
            else {                      // Case #4, #5 and #6
                getNeighboursToSendAgentInsideInnerSubOverlap(neighbouringNodeIDs, *agent);

                getNeighboursThatNeedToRemoveAgent(neighbouringNodeIDs, originalSubOverlapAreaID, *agent);
                getNeighboursThatNeedToAddAgent(neighbouringNodeIDs, originalSubOverlapAreaID, *agent);
            }

            for (std::list<int>::const_iterator itNeighbourNodeID = neighbouringNodeIDs.begin(); itNeighbourNodeID != neighbouringNodeIDs.end(); ++itNeighbourNodeID)
            {
                int neighbourNodeID = *itNeighbourNodeID;

                if (agentsByTypeAndNode.at(neighbourNodeID).find(agentType) == agentsByTypeAndNode.at(neighbourNodeID).end())
                    agentsByTypeAndNode.at(neighbourNodeID)[agentType] = AgentsList();

                agentsByTypeAndNode.at(neighbourNodeID).at(agentType).push_back(agentPtr);
            }
        }

        sendGhostAgentsInMap(agentsByTypeAndNode, originalSubOverlapAreaID);
    }

    void OpenMPIMultiNode::initializeRasterValuesToSendMap(std::map<int, MapOfValuesByRaster>& rasterValuesByNode) const
    {
        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int neighbourID = it->first;

            if (neighbourID == _masterNodeID and not _assignLoadToMasterNode) continue;
            rasterValuesByNode[neighbourID] = std::map<int, MapOfPositionsAndValues>();

            for (int i = 0; i < _world->getNumberOfRasters(); ++i)
            {
                if (_world->rasterExists(i) and _world->isRasterDynamic(i))
                    rasterValuesByNode[neighbourID][i] = MapOfPositionsAndValues();
            }
        }
    }

    bool OpenMPIMultiNode::isPointInAreaOfInfluence(const Point2D<int>& position, const Rectangle<int>& area)
    {
        Rectangle<int> expandedArea = area;
        expandRectangleConsideringLimits(expandedArea, _overlapSize);

        if (expandedArea.contains(position)) return true;
        return false;
    }

    bool OpenMPIMultiNode::hasPositionChangedInRaster(const Point2D<int>& positionToCheck, const DynamicRaster& raster) const
    {
        int rasterIndex = raster.getID();
        if (_sentRastersInStep.find(rasterIndex) != _sentRastersInStep.end())
        {
            int continuousValue = raster.getValue(positionToCheck);
            for (MapOfPositionsAndValues::const_iterator it = _sentRastersInStep.at(rasterIndex).begin(); it != _sentRastersInStep.at(rasterIndex).end(); ++it)
            {
                Point2D<int> position = it->first;
                int lastKnownValue = it->second;

                if (position == positionToCheck)
                {
                    if (continuousValue != lastKnownValue) return true;
                    else return false;
                }
            }

            // If position not found, need to check against it discrete value (initial value at the beginning of the current step):
            int discreteValue = raster.getDiscreteValue(positionToCheck);
            if (continuousValue != discreteValue) return true;
        }
        return false;
    }

    std::list<int> OpenMPIMultiNode::getNeighbourIDsInInnerSubOverlap(const Point2D<int>& point) const
    {
        std::list<int> resultingList;

        for (std::map<int, Rectangle<int>>::const_iterator subOverlapIt = _nodeSpace.innerSubOverlaps.begin(); subOverlapIt != _nodeSpace.innerSubOverlaps.end(); ++subOverlapIt)
        {
            int subOverlapID = subOverlapIt->first;
            Rectangle<int> subOverlapArea = subOverlapIt->second;

            if (subOverlapArea.contains(point))
            {
                for (std::list<int>::const_iterator neighboursIt = _nodeSpace.innerSubOverlapsNeighbours.at(subOverlapID).begin(); neighboursIt != _nodeSpace.innerSubOverlapsNeighbours.at(subOverlapID).end(); ++neighboursIt)
                {
                    int neighbourID = *neighboursIt;
                    if (_nodeSpace.neighbours.at(neighbourID)->ownedAreaWithOuterOverlaps.contains(point))
                        resultingList.push_back(neighbourID);
                }
                break;
            }
        }

        return resultingList;
    }

    std::list<int> OpenMPIMultiNode::getNeighbourIDsInOuterSubOverlap(const Point2D<int>& point) const
    {
        std::list<int> resultingList;

        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int neighbourID = it->first;
            Rectangle<int> neighbourAreaWithOuterOverlap = it->second->ownedAreaWithOuterOverlaps;

            if (neighbourAreaWithOuterOverlap.contains(point))
                resultingList.push_back(neighbourID);
        }

        return resultingList;
    }

    void OpenMPIMultiNode::addPositionAndValueToMap(MapOfPositionsAndValues& map, const Point2D<int>& position, const int& value)
    {
        if (map.find(position) == map.end())
            map[position] = value;
        else
            map.at(position) = value;
    }

    void OpenMPIMultiNode::sendRasterValuesInMap(const std::map<int, MapOfValuesByRaster>& rasterValuesByNode, const int& subOverlapID)
    {
        double initialTime = getWallTime();

        for (std::map<int, MapOfValuesByRaster>::const_iterator rasterValuesByNodeIt = rasterValuesByNode.begin(); rasterValuesByNodeIt != rasterValuesByNode.end(); ++rasterValuesByNodeIt)
        {
            int neighbourNodeID = rasterValuesByNodeIt->first;
            MapOfValuesByRaster valuesByRaster = rasterValuesByNodeIt->second;

            int numberOfRasters = valuesByRaster.size();
            sendDataRequestToNode(&numberOfRasters, 1, MPI_INTEGER, neighbourNodeID, eNumRasters);

            for (MapOfValuesByRaster::const_iterator valuesByRasterIt = valuesByRaster.begin(); valuesByRasterIt != valuesByRaster.end(); ++valuesByRasterIt)
            {
                int rasterIndex = valuesByRasterIt->first;
                MapOfPositionsAndValues positionsAndValues = valuesByRasterIt->second;

                sendDataRequestToNode(&rasterIndex, 1, MPI_INTEGER, neighbourNodeID, eRasterIndex);

                int numberOfPositions = positionsAndValues.size();
                sendDataRequestToNode(&numberOfPositions, 1, MPI_INTEGER, neighbourNodeID, eNumRasterPositions);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " sending numberOfPositions: " << numberOfPositions << " of raster: " << rasterIndex << "\tto node: " << neighbourNodeID << "\n").str();

                PositionAndValue positionAndValueArray[numberOfPositions];

                int i = 0;
                for (MapOfPositionsAndValues::const_iterator positionsAndValuesIt = positionsAndValues.begin(); positionsAndValuesIt != positionsAndValues.end(); ++positionsAndValuesIt)
                {
                    positionAndValueArray[i].x = positionsAndValuesIt->first.getX();
                    positionAndValueArray[i].y = positionsAndValuesIt->first.getY();
                    positionAndValueArray[i].value = positionsAndValuesIt->second;

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " sending position: " << positionsAndValuesIt->first << " and value: " << positionsAndValuesIt->second << "\tto node: " << neighbourNodeID << "\n").str();

                    i++;
                }
                sendDataRequestToNode(positionAndValueArray, numberOfPositions, *_positionAndValueDatatype, neighbourNodeID, ePosAndValue);
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] sendRasterValuesInMap() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::sendRastersToNeighbours(const int& subOverlapID)
    {
        double initialTime = getWallTime();

        if (_numTasks == 1) return;

        std::map<int, MapOfValuesByRaster> rastersValuesByNode;
        initializeRasterValuesToSendMap(rastersValuesByNode);

        for (int rasterIndex = 0; rasterIndex < _world->getNumberOfRasters(); ++rasterIndex)
        {
            if (_world->rasterExists(rasterIndex) and _world->isRasterDynamic(rasterIndex))
            {
                DynamicRaster raster = _world->getDynamicRaster(rasterIndex);

                for (int i = 0; i < raster.getSize().getHeight(); ++i)
                {
                    for (int j = 0; j < raster.getSize().getWidth(); ++j)
                    {
                        Point2D<int> position = Point2D<int>(i, j);

                        if (subOverlapID <= 0 or isPointInAreaOfInfluence(position, _nodeSpace.innerSubOverlaps.at(subOverlapID)))
                        {
                            bool isInInnerOverlapArea = _nodeSpace.ownedArea.contains(position) and not _nodeSpace.ownedAreaWithoutInnerOverlap.contains(position);
                            bool isInOuterOverlapArea = _nodeSpace.ownedAreaWithOuterOverlaps.contains(position) and not _nodeSpace.ownedArea.contains(position);

                            if ((isInInnerOverlapArea or isInOuterOverlapArea) and hasPositionChangedInRaster(position, raster))
                            {
                                std::list<int> neighbourIDs;

                                if (isInInnerOverlapArea) neighbourIDs = getNeighbourIDsInInnerSubOverlap(position);
                                else if (isInOuterOverlapArea) neighbourIDs = getNeighbourIDsInOuterSubOverlap(position);

                                for (std::list<int>::const_iterator neighboursIt = neighbourIDs.begin(); neighboursIt != neighbourIDs.end(); ++neighboursIt)
                                {
                                    int neighbourID = *neighboursIt;

                                    int continuousValue = raster.getValue(position);

                                    addPositionAndValueToMap(rastersValuesByNode.at(neighbourID).at(rasterIndex), position, continuousValue);
                                    addPositionAndValueToMap(_sentRastersInStep.at(rasterIndex), position, continuousValue);
                                }
                            }
                        }
                    }
                }
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] sendRastersToNeighbours() (PREPARATION OF THE DATA) OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());

        sendRasterValuesInMap(rastersValuesByNode, subOverlapID);
    }

    void OpenMPIMultiNode::receiveRasters(const int& subOverlapID)
    {
        double initialTime = getWallTime();

        if (_numTasks == 1) return;

        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int sendingNodeID = it->first;
            int numberOfRasters;
            MPI_Recv(&numberOfRasters, 1, MPI_INTEGER, sendingNodeID, eNumRasters, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int i = 0; i < numberOfRasters; ++i)
            {
                int rasterIndex;
                MPI_Recv(&rasterIndex, 1, MPI_INTEGER, sendingNodeID, eRasterIndex, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                if (not _world->rasterExists(rasterIndex)) 
                    throw Exception(CreateStringStream("[Process # " << getId() <<  "] OpenMPIMultiNode::receiveRasters() - raster with index: " << rasterIndex << " not found.\n").str());

                int numberOfPositions;
                MPI_Recv(&numberOfPositions, 1, MPI_INTEGER, sendingNodeID, eNumRasterPositions, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                PositionAndValue positionAndValueArray[numberOfPositions];
                MPI_Recv(positionAndValueArray, numberOfPositions, *_positionAndValueDatatype, sendingNodeID, ePosAndValue, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int j = 0; j < numberOfPositions; ++j)
                {
                    PositionAndValue positionAndValue = positionAndValueArray[j];

                    Point2D<int> position = Point2D<int>(positionAndValue.x, positionAndValue.y);
                    int continuousValue = positionAndValue.value;

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " receiving raster index: " << rasterIndex << " position: " << position << " and value: " << continuousValue << "\tfrom node: " << sendingNodeID << "\n").str();

                    _world->getDynamicRaster(rasterIndex).setValue(position, continuousValue);

                    addPositionAndValueToMap(_sentRastersInStep.at(rasterIndex), position, continuousValue);
                }
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] receiveRasters() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void OpenMPIMultiNode::clearRequests()
    {
        for (std::list<MPI_Request*>::const_iterator it = _sendRequests.begin(); it != _sendRequests.end(); ++it)
        {
            MPI_Request* mpiRequest = *it;
            MPI_Wait(mpiRequest, MPI_STATUS_IGNORE);
        }
    }

    AgentsList::const_iterator OpenMPIMultiNode::getAgentIteratorFromID(const std::string& agentID)
    {
        for (AgentsList::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            Agent* agent = it->get();
            if (agent->getId() == agentID) return it;
        }
        return _world->endAgents();
    }

    /** RUN PUBLIC METHODS (INHERIT) **/

    void OpenMPIMultiNode::updateEnvironmentState()
    {
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] STEP: " << _world->getCurrentStep() << " ==================================================================================\n").str());

        initializeAgentsAndRastersState();

        sendRastersToNeighbours();
        receiveRasters();

        MPI_Barrier(MPI_COMM_WORLD);
    }

    void OpenMPIMultiNode::executeAgents()
    {
//usleep(getId() * 500000);

_schedulerLogs->printAgentsMatrixInDebugFile(*this);

_schedulerLogs->writeInDebugFile(CreateStringStream("AGENTS AT THE BEGINNING OF STEP " << _world->getCurrentStep() << ":").str(), *this);
_schedulerLogs->printNodeAgentsInDebugFile(*this, true);

_schedulerLogs->writeInDebugFile(CreateStringStream("RASTERS AT THE BEGINNING OF STEP " << _world->getCurrentStep() << ":").str(), *this);
_schedulerLogs->printNodeRastersInDebugFile(*this);

//if (_world->getCurrentStep() == 8) exit(0);
        AgentsVector executedAgentsInArea;
        executeAgentsInSubOverlap(executedAgentsInArea, 0);
        synchronizeAgentsIfNecessary(executedAgentsInArea, 0);

        sendRastersToNeighbours(0);
        receiveRasters(0);

        double initialTime = getWallTime();
        MPI_Barrier(MPI_COMM_WORLD);
        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] MPI_Barrier() AFTER OVERLAP: " << 0 << "\tTOTAL TIME: " << endTime - initialTime).str());

_schedulerLogs->writeInDebugFile(CreateStringStream("AGENTS AT STEP " << _world->getCurrentStep() << "; INNER_MOST EXECUTED:").str(), *this);
_schedulerLogs->printNodeAgentsInDebugFile(*this, true);

_schedulerLogs->writeInDebugFile(CreateStringStream("RASTERS AT STEP " << _world->getCurrentStep() << "; INNER_MOST EXECUTED:").str(), *this);
_schedulerLogs->printNodeRastersInDebugFile(*this);

if (_printInConsole) std::cout << CreateStringStream("\n").str();
//usleep(500000);

        for (std::map<int, Rectangle<int>>::const_iterator it = _nodeSpace.innerSubOverlaps.begin(); it != _nodeSpace.innerSubOverlaps.end(); ++it)
        {
            int originalSubOverlapAreaID = it->first;
            Rectangle<int> originalOverlapArea = it->second;

//usleep(getId() * 500000);

            executedAgentsInArea.clear();
if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "] " << getWallTime() << " executing agents in suboverlap #" << originalSubOverlapAreaID << "\n").str();
            executeAgentsInSubOverlap(executedAgentsInArea, originalSubOverlapAreaID);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "] " << getWallTime() << " agents executed -> synchronizing\n").str();
            sendGhostAgentsToNeighbours(executedAgentsInArea, originalSubOverlapAreaID);
            receiveGhostAgentsFromNeighbouringNodes(originalSubOverlapAreaID);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "] " << getWallTime() << " agents synchronized\n").str();
if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\n").str();

            sendRastersToNeighbours(originalSubOverlapAreaID);
            receiveRasters(originalSubOverlapAreaID);

            //clearRequests(); // Is this really necessary?

            initialTime = getWallTime();
            MPI_Barrier(MPI_COMM_WORLD);
            double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("[Process # " << getId() <<  "] MPI_Barrier() AFTER OVERLAP: " << originalSubOverlapAreaID << "\tTOTAL TIME: " << endTime - initialTime).str());


_schedulerLogs->writeInDebugFile(CreateStringStream("AGENTS AT STEP " << _world->getCurrentStep() << "; AFTER OVERLAP: " << originalSubOverlapAreaID).str(), *this);
_schedulerLogs->printNodeAgentsInDebugFile(*this, true);

_schedulerLogs->writeInDebugFile(CreateStringStream("RASTERS AT STEP " << _world->getCurrentStep() << "; AFTER OVERLAP: " << originalSubOverlapAreaID).str(), *this);
_schedulerLogs->printNodeRastersInDebugFile(*this);

if (_printInConsole) std::cout << CreateStringStream("\n").str();
//usleep(500000);
        }

if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, CreateStringStream("\n").str());
    }
    
    void OpenMPIMultiNode::finish() 
    {
        MpiFactory::instance()->cleanTypes();

        MPI_Type_free(_coordinatesDatatype);
        MPI_Type_free(_positionAndValueDatatype);

std::string totalSimulationTime = CreateStringStream("TotalSimulationTime: " << MPI_Wtime() - _initialTime << " seconds.\n").str();
std::cout << totalSimulationTime;
if (_printInstrumentation) _schedulerLogs->printInstrumentation(*this, totalSimulationTime);

        MPI_Finalize();
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

    void OpenMPIMultiNode::removeAgents()
    {
        std::list<std::string> agentIDsToBeRemoved;
        for (AgentsList::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            Agent* agent = it->get();
            if (not agent->exists()) agentIDsToBeRemoved.push_back(agent->getId());
        }

        removeAgentsFromID(agentIDsToBeRemoved);
    }

    void OpenMPIMultiNode::removeAgent(Agent* agent)
    {
        AgentsList::const_iterator agentIt = getAgentIteratorFromID(agent->getId());
        if (agentIt == _world->endAgents())
            throw Exception(CreateStringStream("[Process # " << getId() <<  "] OpenMPIMultiNode::removeAgent(id) - agent: " << agentIt->get()->getId() << " not found.\n").str());
        agent->setExists(false);
    }

    Agent* OpenMPIMultiNode::getAgent(const std::string& id)
    {
        AgentsList::const_iterator agentIt = getAgentIteratorFromID(id);
        if (agentIt == _world->endAgents() or not agentIt->get()->exists())
            throw Exception(CreateStringStream("[Process # " << getId() <<  "] OpenMPIMultiNode::getAgent(id) - agent: " << agentIt->get()->getId() << " not found.").str());

        return agentIt->get();
    }

    AgentsVector OpenMPIMultiNode::getAgent(const Point2D<int>& position, const std::string& type)
    {
        return _tree->getAgentsInPosition(position, type);
    }

    int OpenMPIMultiNode::countNeighbours(Agent* target, const double& radius, const std::string& type)
    {
        return for_each(_world->beginAgents(), _world->endAgents(), aggregatorCount<std::shared_ptr<Agent>>(radius, *target, type))._count;
    }

    AgentsVector OpenMPIMultiNode::getNeighbours(Agent* target, const double& radius, const std::string& type)
    {
        AgentsVector agentsVector = for_each(_world->beginAgents(), _world->endAgents(), aggregatorGet<std::shared_ptr<Agent>>(radius, *target, type))._neighbors;
        GeneralState::statistics().shuffleWithinIterators(agentsVector.begin(), agentsVector.end());
        return agentsVector;
    }

    size_t OpenMPIMultiNode::getNumberOfTypedAgents(const std::string& type) const
    {
        size_t numberOfAgents = 0;
        for (AgentsList::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            AgentPtr agentPtr = *it;
            if (agentPtr->isType(type)) ++numberOfAgents;
        }
        size_t distributedNumberOfAgents;
        MPI_Allreduce(&numberOfAgents, &distributedNumberOfAgents, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
        return distributedNumberOfAgents;
    }

    void OpenMPIMultiNode::setValue(DynamicRaster& raster, const Point2D<int>& position, int value)
    {
        raster.setValue(position, value);
    }

    int OpenMPIMultiNode::getValue(const DynamicRaster& raster, const Point2D<int>& position) const
    {
        return raster.getValue(position);
    }

    void OpenMPIMultiNode::setMaxValue(DynamicRaster& raster, const Point2D<int>& position, int value)
    {
        raster.setMaxValue(position, value);
    }

    int OpenMPIMultiNode::getMaxValue(const DynamicRaster& raster, const Point2D<int>& position) const
    {
        return raster.getMaxValue(position);
    }

    void OpenMPIMultiNode::addStringAttribute(const std::string& type, const std::string& key, const std::string& value)
    {
        _serializer.addStringAttribute(type, key, value);
    }

    void OpenMPIMultiNode::addIntAttribute(const std::string& type, const std::string& key, int value)
    {
        _serializer.addIntAttribute(type, key, value);
    }

    void OpenMPIMultiNode::addFloatAttribute(const std::string & type, const std::string& key, float value)
    {
        _serializer.addFloatAttribute(type, key, value);
    }

    void OpenMPIMultiNode::serializeAgents(const int& step)
    {
        _serializer.serializeAgents(step, _world->beginAgents(), _world->endAgents());
    }

    void OpenMPIMultiNode::serializeRasters(const int& step)
    {
        _serializer.serializeRasters(step);
    }

    const Rectangle<int>& OpenMPIMultiNode::getOwnedArea() const
    {
        return _nodeSpace.ownedArea;
    }

    const int& OpenMPIMultiNode::getOverlap() const
    {
        return _overlapSize;
    }

} // namespace Engine