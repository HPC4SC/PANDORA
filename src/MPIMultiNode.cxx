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

#include <MPIMultiNode.hxx>

#include <Exception.hxx>
#include <GeneralState.hxx>
#include <MpiFactory.hxx>

#include <math.h>
#include <string>
#include <string.h>

#include <unistd.h>
#include <stdlib.h>

namespace Engine {

    /** INITIALIZATION PUBLIC METHODS **/

    MPIMultiNode::MPIMultiNode() : _initialTime(0.0f), _masterNodeID(0), _justAwaken(false), _justFinished(false), _serializer(*this)
    {
    }

    MPIMultiNode::~MPIMultiNode()
    {
    }

    void MPIMultiNode::init(int argc, char *argv[]) 
    {
        int alreadyInitialized;
        MPI_Initialized(&alreadyInitialized);
        if (not alreadyInitialized) MPI_Init(&argc, &argv);

        _initialTime = getWallTime();

        MPI_Comm_size(MPI_COMM_WORLD, &_numTasks);
        MPI_Comm_rank(MPI_COMM_WORLD, &_id);

        _numTasksMax = _numTasks;

        initLogFileNames();
        initLoadBalanceTree();
        initAutoAdjustment();

        omp_init_lock(&_ompLock);

        stablishInitialBoundaries();
        registerMPIStructs();

        _serializer.init(*_world);

        MpiFactory::instance()->registerTypes();
    }

    void MPIMultiNode::initData() 
    {
        createInitialRasters();
        createInitialAgents();

        bool distributeFromTheBeginning = _world->getConfig().getInitialPartitioning();
        if (not distributeFromTheBeginning)
            _numTasks = 1;

        _loadBalanceTree->setNumberOfPartitions(_numTasks);
        enableOnlyProcesses(_numTasks);

        MPI_Barrier(MPI_COMM_WORLD);

        if (getId() == _masterNodeID) 
        {
            divideSpace();                                  printPartitionsBeforeMPI();
            sendInitialSpacesToNodes(_masterNodeID);        printOwnNodeStructureAfterMPI();
        }
        else 
        {
            if (not distributeFromTheBeginning)
            {
                waitSleeping(_masterNodeID);
std::cout << CreateStringStream("[processID: " << getId() << "] outside waitSleeping()\n").str();
            }
            else
                receiveInitialSpacesFromNode(_masterNodeID);    printOwnNodeStructureAfterMPI();
        }

        if (not hasBeenTaggedAsJustFinished() and not hasBeenTaggedAsJustAwaken())
        {
            filterOutInitialAgents();                           printNodeAgents();
            filterOutInitialRasters();                          printNodeRasters();

            stablishBoundaries();

if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("\n").str());
        }
    }

    /** INITIALIZATION PROTECTED METHODS **/

    void MPIMultiNode::terminateAllMPIProcesses() const
    {
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        MPI_Finalize();
    }

    void MPIMultiNode::initLogFileNames()
    {
        _schedulerLogs = new MPIMultiNodeLogs();
        _schedulerLogs->initLogFileNames(*this);
    }

    void MPIMultiNode::initLoadBalanceTree()
    {
        _loadBalanceTree = new MPILoadBalanceTree();
        _loadBalanceTree->setWorld(_world);
        _loadBalanceTree->initializeTree();
    }

    void MPIMultiNode::initAutoAdjustment()
    {
        _autoAdjustment = new MPIAutoAdjustment();
        _autoAdjustment->initAutoAdjustment(*this);
    }

    void MPIMultiNode::stablishInitialBoundaries()
    {
        _boundaries = Rectangle<int>(_world->getConfig().getSize());

        _nodeSpace.ownedArea = _boundaries;
    }

    void MPIMultiNode::registerMPIStructs()
    {
        _coordinatesDatatype = new MPI_Datatype;
        MPI_Type_contiguous(4, MPI_INT, _coordinatesDatatype);
        MPI_Type_commit(_coordinatesDatatype);

        _positionAndValueDatatype = new MPI_Datatype;
        MPI_Type_contiguous(3, MPI_INT, _positionAndValueDatatype);
        MPI_Type_commit(_positionAndValueDatatype);
    }

    void MPIMultiNode::createInitialRasters()
    {
        double initialTime = getWallTime();

        _world->createInitialRasters();

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::createInitialRasters()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::createInitialAgents()
    {
        double initialTime = getWallTime();

        _world->createInitialAgents();

        double endTime = getWallTime();

if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::createInitialAgents()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::enableOnlyProcesses(const int& numberOfProcessesToEnable)
    {
        _numberOfActiveProcesses = numberOfProcessesToEnable;
        _numTasks = numberOfProcessesToEnable;

        int* activeRanksArray = (int*) malloc(_numberOfActiveProcesses * sizeof(int));

        for (int i = 0; i < _numberOfActiveProcesses; ++i)
                activeRanksArray[i] = i;

        MPI_Group worldGroup, activeGroup;
        MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

        MPI_Group_incl(worldGroup, _numberOfActiveProcesses, activeRanksArray, &activeGroup);
        MPI_Comm_create_group(MPI_COMM_WORLD, activeGroup, eCreateGroupActive, &_activeProcessesComm);
        MPI_Group_free(&worldGroup);
        MPI_Group_free(&activeGroup);

        printActiveAndInactiveProcesses();
    }

    void MPIMultiNode::cleanNodeDataStructures()
    {
        std::vector<Agent*> agentsToRemove;
        for (AgentsMap::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
            agentsToRemove.push_back(it->second.get());
        removeAgentsInVector(agentsToRemove);

        // The following is optional (when this node is awaken, it receives all and every raster cell from scratch before this code).
        // for (int rasterID = 0; rasterID < _schedulerInstance->_world->getNumberOfRasters(); ++rasterID)
        // {
        //     if (_schedulerInstance->_world->rasterExists(rasterID) and _schedulerInstance->_world->isRasterDynamic(rasterID))
        //     {
        //         for (int i = _nodeSpace.ownedAreaWithOuterOverlap.top(); i < _nodeSpace.ownedAreaWithOuterOverlap.bottom() + 1; ++i)
        //         {
        //             for (int j = _nodeSpace.ownedAreaWithOuterOverlap.left(); j < _nodeSpace.ownedAreaWithOuterOverlap.right() + 1; ++j)
        //             {
        //                 Point2D<int> point = Point2D<int>(j, i);
        //                 _world->setValue(rasterID, point, -1);
        //             }
        //         }
        //     }
        // }

        MPINode mpiNode;
        _nodeSpace = mpiNode;

        _executedAgentsInStep.clear();
        _changedRastersCells.clear();

        _justAwaken = _justFinished = false;

        _agentsToBeRemoved.clear();
    }

    void MPIMultiNode::waitSleeping(const int& masterNodeID)
    {
        cleanNodeDataStructures();

        bool wakeUp = false;
        while (not wakeUp)
        {
            int flag = 0;
            while (flag == 0)
            {
                int milisecondsToSleep = 10;
                usleep(milisecondsToSleep * 1000);
                MPI_Iprobe(masterNodeID, eTypeOfEventAfterWakeUp, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
            }

            int typeOfEventAfterWakeUp;
            MPI_Recv(&typeOfEventAfterWakeUp, 1, MPI_INT, masterNodeID, eTypeOfEventAfterWakeUp, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (typeOfEventAfterWakeUp == eMessage_Die)
            {
                finish();
                _justFinished = true;
                return;
            }
            else if (typeOfEventAfterWakeUp == eMessage_AwakeToRepartition)
            {
                wakeUp = true;
                _justAwaken = true;

                int newNumberOfProcessesAndCurrentStep[2];
                MPI_Recv(newNumberOfProcessesAndCurrentStep, 2, MPI_INT, masterNodeID, eNumberOfProcessesAndStep, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int newNumberOfProcesses = newNumberOfProcessesAndCurrentStep[0];
                int currentStep = newNumberOfProcessesAndCurrentStep[1];

std::cout << CreateStringStream("[process id: " << getId() << "] being awaken.... newNumberOfProcesses: " << newNumberOfProcesses << "\t currentStep: " << currentStep << "\n").str();

                _numberOfActiveProcesses = newNumberOfProcesses;
                _numTasks = newNumberOfProcesses;

                _world->setCurrentStep(currentStep);
            }
        }
    }

    void MPIMultiNode::divideSpace()
    {
        double initialTime = getWallTime();

        _loadBalanceTree->divideSpace();

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::divideSpace()\tTOTAL TIME: " << endTime - initialTime).str());

        initialTime = getWallTime();

        createNodesInformationToSend();

        endTime = getWallTime();

if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::createNodesInformationToSend()\tTOTAL TIME: " << endTime - initialTime).str());


        _schedulerLogs->writeInDebugFile(CreateStringStream("HEY THERE:").str());
        _schedulerLogs->printAgentsMatrixInDebugFile(true);





        if (not arePartitionsSuitable()) {
            std::cout << CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::divideSpace() - Partitions not suitable. Maybe there are too many unnecessary MPI nodes for such a small space, or the overlap size is too wide.\n").str();
            terminateAllMPIProcesses();
        }
    }

    void MPIMultiNode::sendInitialSpacesToNodes(const int& masterNodeID)
    {
        double initialTime = getWallTime();

        for (MPINodesMap::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            int nodeID = it->first;

            if (nodeID == masterNodeID) fillOwnStructures(it->second);
            else 
            {
                sendOwnAreaToNode(nodeID, it->second.ownedArea);
                sendNeighboursToNode(nodeID, it->second.neighbours);
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::sendInitialSpacesToNodes()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::receiveInitialSpacesFromNode(const int& masterNodeID)
    {
        double initialTime = getWallTime();

        MPINode mpiNode;
        receiveOwnAreaFromNode(masterNodeID, mpiNode);
        receiveNeighboursFromNode(masterNodeID, mpiNode);
        fillOwnStructures(mpiNode);

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::receiveInitialSpacesFromNode()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::filterOutInitialAgents()
    {
        double initialTime = getWallTime();

        std::vector<Agent*> agentsToRemove;

        for (AgentsMap::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            AgentPtr agentPtr = it->second;
            Point2D<int> agentPosition = agentPtr.get()->getPosition();

            if (not _nodeSpace.ownedAreaWithOuterOverlap.contains(agentPosition))
                agentsToRemove.push_back(agentPtr.get());
        }

        removeAgentsInVector(agentsToRemove);

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::filterOutInitialAgents()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::filterOutInitialRasters()
    {
        double initialTime = getWallTime();


        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::filterOutInitialRasters()\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::generateOverlapAreas(MPINode& mpiNode) const
    {
        mpiNode.ownedAreaWithOuterOverlap = mpiNode.ownedArea;
        mpiNode.ownedAreaWithoutInnerOverlap = mpiNode.ownedArea;

        if (_numTasks == 1) return;

        expandRectangleConsideringLimits(mpiNode.ownedAreaWithOuterOverlap, _overlapSize);
        expandRectangleConsideringLimits(mpiNode.ownedAreaWithoutInnerOverlap, -_overlapSize, false);
    }

    Rectangle<int> MPIMultiNode::squeezeRectangle(const Rectangle<int>& rectangle, const int& squeezeTop, const int& squeezeBottom, const int& squeezeLeft, const int& squeezeRight) const
    {
        Rectangle<int> resultingRectangle = rectangle;

        resultingRectangle.getOrigin().getY() += squeezeTop;
        resultingRectangle.getSize().getHeight() -= (squeezeTop + squeezeBottom);

        resultingRectangle.getOrigin().getX() += squeezeLeft;
        resultingRectangle.getSize().getWidth() -= (squeezeLeft + squeezeRight);

        return resultingRectangle;
    }

    void MPIMultiNode::generateInnerSubOverlapAreas(MPINode& mpiNode) const
    {
        if (_numTasks == 1) return;

        if (_subpartitioningMode == 4)
        {
            mpiNode.innerSubOverlaps[eTopLeft_Mode4] =        squeezeRectangle(mpiNode.ownedArea, 0, std::ceil( (double) mpiNode.ownedArea.getSize().getHeight() / 2), 0, std::ceil( (double) mpiNode.ownedArea.getSize().getWidth() / 2));
            mpiNode.innerSubOverlaps[eTopRight_Mode4] =       squeezeRectangle(mpiNode.ownedArea, 0, std::ceil( (double) mpiNode.ownedArea.getSize().getHeight() / 2), std::floor( (double) mpiNode.ownedArea.getSize().getWidth() / 2), 0);
            mpiNode.innerSubOverlaps[eBottomLeft_Mode4] =     squeezeRectangle(mpiNode.ownedArea, std::floor( (double) mpiNode.ownedArea.getSize().getHeight() / 2), 0, 0, std::ceil( (double) mpiNode.ownedArea.getSize().getWidth() / 2));
            mpiNode.innerSubOverlaps[eBottomRight_Mode4] =    squeezeRectangle(mpiNode.ownedArea, std::floor( (double) mpiNode.ownedArea.getSize().getHeight() / 2), 0, std::floor( (double) mpiNode.ownedArea.getSize().getWidth() / 2), 0);
        }
        else if (_subpartitioningMode == 9)
        {
            mpiNode.innerSubOverlaps[eTopCenter_Mode9] =      squeezeRectangle(mpiNode.ownedArea, 0, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, _overlapSize, _overlapSize);
            mpiNode.innerSubOverlaps[eBottomCenter_Mode9] =   squeezeRectangle(mpiNode.ownedArea, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, 0, _overlapSize, _overlapSize);
            mpiNode.innerSubOverlaps[eLeftCenter_Mode9] =     squeezeRectangle(mpiNode.ownedArea, _overlapSize, _overlapSize, 0, mpiNode.ownedArea.getSize().getWidth() - _overlapSize);
            mpiNode.innerSubOverlaps[eRightCenter_Mode9] =    squeezeRectangle(mpiNode.ownedArea, _overlapSize, _overlapSize, mpiNode.ownedArea.getSize().getWidth() - _overlapSize, 0);

            mpiNode.innerSubOverlaps[eTopLeft_Mode9] =        squeezeRectangle(mpiNode.ownedArea, 0, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, 0, mpiNode.ownedArea.getSize().getWidth() - _overlapSize);
            mpiNode.innerSubOverlaps[eTopRight_Mode9] =       squeezeRectangle(mpiNode.ownedArea, 0, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, mpiNode.ownedArea.getSize().getWidth() - _overlapSize, 0);
            mpiNode.innerSubOverlaps[eBottomLeft_Mode9] =     squeezeRectangle(mpiNode.ownedArea, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, 0, 0, mpiNode.ownedArea.getSize().getWidth() - _overlapSize);
            mpiNode.innerSubOverlaps[eBottomRight_Mode9] =    squeezeRectangle(mpiNode.ownedArea, mpiNode.ownedArea.getSize().getHeight() - _overlapSize, 0, mpiNode.ownedArea.getSize().getWidth() - _overlapSize, 0);
        }
        else
        {
            std::cout << CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::generateInnerSubOverlapAreas() - Subpartitioning mode not implemented. Please, use mode 4 or mode 9.\n").str();
            terminateAllMPIProcesses();
        }
        
    }

    void MPIMultiNode::generateInnerSubOverlapNeighbours(MPINode& mpiNode, const int& neighbourID, const MPINode& neighbourNode) const
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

    void MPIMultiNode::fillOwnStructures(const MPINode& mpiNodeInfo, const bool& fillNeighbours)
    {
        _nodeSpace.ownedArea = mpiNodeInfo.ownedArea;

        generateOverlapAreas(_nodeSpace);
        generateInnerSubOverlapAreas(_nodeSpace);

        if (fillNeighbours)
        {
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
    }

    void MPIMultiNode::sendOwnAreaToNode(const int& nodeID, const Rectangle<int>& mpiNodeInfo) const
    {
        Coordinates coordinates;
        coordinates.top = mpiNodeInfo.top();
        coordinates.left = mpiNodeInfo.left();
        coordinates.bottom = mpiNodeInfo.bottom();
        coordinates.right = mpiNodeInfo.right();
        MPI_Send(&coordinates, 1, *_coordinatesDatatype, nodeID, eCoordinates, MPI_COMM_WORLD);
    }
    
    void MPIMultiNode::sendNeighboursToNode(const int& nodeID, const std::map<int, MPINode*>& neighbours) const
    {
        int numberOfNeighbours = neighbours.size();
        MPI_Send(&numberOfNeighbours, 1, MPI_INT, nodeID, eNumNeighbours, MPI_COMM_WORLD);

        for (std::map<int, MPINode*>::const_iterator it = neighbours.begin(); it != neighbours.end(); ++it)
        {
            int neighbourID = it->first;
            MPI_Send(&neighbourID, 1, MPI_INT, nodeID, eNeighbourID, MPI_COMM_WORLD);

            Coordinates coordinates;
            coordinates.top = it->second->ownedArea.top();
            coordinates.left = it->second->ownedArea.left();
            coordinates.bottom = it->second->ownedArea.bottom();
            coordinates.right = it->second->ownedArea.right();
            MPI_Send(&coordinates, 1, *_coordinatesDatatype, nodeID, eCoordinatesNeighbour, MPI_COMM_WORLD);
        }
    }

    void MPIMultiNode::receiveOwnAreaFromNode(const int& masterNodeID, MPINode& mpiNodeInfo) const
    {
        Coordinates coordinates;
        MPI_Recv(&coordinates, 1, *_coordinatesDatatype, masterNodeID, eCoordinates, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        mpiNodeInfo.ownedArea = Rectangle<int>(coordinates.left, coordinates.top, coordinates.right, coordinates.bottom);
    }

    void MPIMultiNode::receiveNeighboursFromNode(const int& masterNodeID, MPINode& mpiNodeInfo) const
    {
        int numberOfNeighbours;
        MPI_Recv(&numberOfNeighbours, 1, MPI_INT, masterNodeID, eNumNeighbours, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < numberOfNeighbours; ++i)
        {
            int neighbourID;
            MPI_Recv(&neighbourID, 1, MPI_INT, masterNodeID, eNeighbourID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            Coordinates coordinates;
            MPI_Recv(&coordinates, 1, *_coordinatesDatatype, masterNodeID, eCoordinatesNeighbour, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            mpiNodeInfo.neighbours[neighbourID] = new MPINode;
            mpiNodeInfo.neighbours[neighbourID]->ownedArea = Rectangle<int>(coordinates.left, coordinates.top, coordinates.right, coordinates.bottom);
        }
    }

    bool MPIMultiNode::isAgentInList(const Agent& agent, const AgentsList& agentsList) const
    {
        for (AgentsList::const_iterator it = agentsList.begin(); it != agentsList.end(); ++it)
        {
            Agent* agentFromList = it->get();
            if (agent.isEqual(*agentFromList)) return true;
        }
        return false;
    }
    
    void MPIMultiNode::removeAgentsInVector(const std::vector<Agent*>& agentsToRemove)
    {
        for (int i = 0; i < agentsToRemove.size(); ++i)
        {
            Agent* agent = agentsToRemove[i];
            _world->eraseAgent(agent);
        }
    }

    void MPIMultiNode::stablishBoundaries()
    {
        _boundaries = _nodeSpace.ownedArea;
    }

    void MPIMultiNode::addMPINodeToSendInMapItIsNot(const std::vector<Rectangle<int>>& partitions, const int& neighbourIndex)
    {
        if (_mpiNodesMapToSend.find(neighbourIndex) == _mpiNodesMapToSend.end())
        {
            MPINode mpiNode;
            mpiNode.ownedArea = partitions[neighbourIndex];
            generateOverlapAreas(mpiNode);

            _mpiNodesMapToSend[neighbourIndex] = mpiNode;
        }
    }

    void MPIMultiNode::expandRectangleConsideringLimits(Rectangle<int>& rectangle, const int& expansion, const bool& contractInTheLimits) const
    {
        int topDifferenceAgainstLimits = rectangle.top() - _loadBalanceTree->getTree().value.top();
        int leftDifferenceAgainstLimits = rectangle.left() - _loadBalanceTree->getTree().value.left();

        int topMovement = std::min(topDifferenceAgainstLimits, expansion);
        if (not contractInTheLimits and topDifferenceAgainstLimits == 0) topMovement = 0;

        int leftMovement = std::min(leftDifferenceAgainstLimits, expansion);
        if (not contractInTheLimits and leftDifferenceAgainstLimits == 0) leftMovement = 0;

        rectangle.getOrigin().getY() -= topMovement;
        rectangle.getOrigin().getX() -= leftMovement;

        if (rectangle.bottom() > _loadBalanceTree->getTree().value.bottom()) rectangle.getSize().getHeight() -= rectangle.bottom() - _loadBalanceTree->getTree().value.bottom();
        if (rectangle.right() > _loadBalanceTree->getTree().value.right()) rectangle.getSize().getWidth() -= rectangle.right() - _loadBalanceTree->getTree().value.right();

        int bottomDifferenceAgainstLimits = _loadBalanceTree->getTree().value.bottom() - rectangle.bottom();
        int rightDifferenceAgainstLimits = _loadBalanceTree->getTree().value.right() - rectangle.right();

        int heightMovement = std::min(bottomDifferenceAgainstLimits, expansion + topMovement);
        if (not contractInTheLimits and bottomDifferenceAgainstLimits == 0) heightMovement = 0;

        int widthMovement = std::min(rightDifferenceAgainstLimits, expansion + leftMovement);
        if (not contractInTheLimits and rightDifferenceAgainstLimits == 0) widthMovement = 0;

        rectangle.getSize().getHeight() += heightMovement;
        rectangle.getSize().getWidth() += widthMovement;
    }

    bool MPIMultiNode::doOverlap(const Rectangle<int>& rectangleA, const Rectangle<int>& rectangleB) const
    {
        if (rectangleA.getOrigin().getX() >= rectangleB.right() + 1 or
            rectangleB.getOrigin().getX() >= rectangleA.right() + 1 or
            rectangleA.getOrigin().getY() >= rectangleB.bottom() + 1 or
            rectangleB.getOrigin().getY() >= rectangleA.bottom() + 1)
            return false;
        return true;
    }

    bool MPIMultiNode::areTheyNeighbours(const Rectangle<int>& rectangleA, const Rectangle<int>& rectangleB) const
    {
        Rectangle<int> rectangleAWithOuterOverlap = rectangleA;
        Rectangle<int> rectangleBWithOuterOverlap = rectangleB;
        expandRectangleConsideringLimits(rectangleAWithOuterOverlap, _overlapSize);
        expandRectangleConsideringLimits(rectangleBWithOuterOverlap, _overlapSize);

        return doOverlap(rectangleAWithOuterOverlap, rectangleBWithOuterOverlap);
    }

    void MPIMultiNode::createNodesInformationToSend()
    {
        std::vector<Rectangle<int>> partitions;
        _loadBalanceTree->getPartitionsFromTree(partitions);

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

    bool MPIMultiNode::arePartitionsSuitable()
    {
        if (_numTasks == 1) return true;

        for (MPINodesMap::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            if (it->second.ownedArea.getSize().getWidth() < 4 * _overlapSize or
                it->second.ownedArea.getSize().getHeight() < 4 * _overlapSize)
                return false;
        }
        return true;
    }

    /** INITIALIZATION DEBUGGING METHODS **/

    void MPIMultiNode::printPartitionsBeforeMPI() const
    {
        _schedulerLogs->printPartitionsBeforeMPIInDebugFile();
    }

    void MPIMultiNode::printOwnNodeStructureAfterMPI() const
    {
        _schedulerLogs->printOwnNodeStructureAfterMPIInDebugFile();
    }
    
    void MPIMultiNode::printNodeAgents() const
    {
        _schedulerLogs->printNodeAgentsInDebugFile();
    }

    void MPIMultiNode::printNodeRasters() const
    {
        _schedulerLogs->printNodeRastersInDebugFile();
    }

    void MPIMultiNode::printActiveAndInactiveProcesses() const
    {
        int rank = -1;
        if (_activeProcessesComm != MPI_COMM_NULL)
        {
            MPI_Comm_rank(_activeProcessesComm, &rank);
            std::cout << CreateStringStream("[Process # " << getId() <<  "] active group rank: " << rank << " id: " << _id << "\n").str();
        }
        else if (MPI_COMM_WORLD != MPI_COMM_NULL)
        {
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            std::cout << CreateStringStream("[Process # " << getId() <<  "] inactive group rank: " << rank << " id: " << _id << "\n").str();
        }

        std::cout << CreateStringStream("\n").str();
    }

    /** RUN PROTECTED METHODS (CALLED BY INHERIT METHODS) **/

    void MPIMultiNode::prepareAgentsAndRastersStateForCurrentStep()
    {
        _executedAgentsInStep.clear();
    }

    void MPIMultiNode::updateTotalAccordingToExecutingPhase(const int& executingPhaseType, const double& initialTime)
    {
        double endTime = getWallTime();
        double currentAgentTime = endTime - initialTime;

        if (executingPhaseType == eUpdateKnowledge)
            _world->setUpdateKnowledgeTotalTime(_world->getUpdateKnowledgeTotalTime() + currentAgentTime);
        else if (executingPhaseType == eSelectActions)
            _world->setSelectActionsTotalTime(_world->getSelectActionsTotalTime() + currentAgentTime);
        else if (executingPhaseType == eExecuteActions)
            _world->setExecuteActionsTotalTime(_world->getExecuteActionsTotalTime() + currentAgentTime);
        else if (executingPhaseType == eUpdateState)
            _world->setUpdateStateTotalTime(_world->getUpdateStateTotalTime() + currentAgentTime);
    }

    void MPIMultiNode::agent_updateKnowledge(Agent* agent)
    {
        double initialTime;

        bool monitorIt = _world->getConfig().getRebalancingFrequency() > 0 and _world->getCurrentStep() % _world->getConfig().getRebalancingFrequency() == 0;
        if (monitorIt) initialTime = getWallTime();
        
        agent->updateKnowledge();

        if (monitorIt) updateTotalAccordingToExecutingPhase(eUpdateKnowledge, initialTime);
    }

    void MPIMultiNode::agent_selectActions(Agent* agent)
    {
        double initialTime;

        bool monitorIt = _world->getConfig().getRebalancingFrequency() > 0 and _world->getCurrentStep() % _world->getConfig().getRebalancingFrequency() == 0;
        if (monitorIt) initialTime = getWallTime();

        agent->selectActions();

        if (monitorIt) updateTotalAccordingToExecutingPhase(eSelectActions, initialTime);
    }

    void MPIMultiNode::agent_executeActions(Agent* agent)
    {
        double initialTime;

        bool monitorIt = _world->getConfig().getRebalancingFrequency() > 0 and _world->getCurrentStep() % _world->getConfig().getRebalancingFrequency() == 0;
        if (monitorIt) initialTime = getWallTime();

        agent->executeActions();

        if (monitorIt) updateTotalAccordingToExecutingPhase(eExecuteActions, initialTime);
    }

    void MPIMultiNode::agent_updateState(Agent* agent)
    {
        double initialTime;

        bool monitorIt = _world->getConfig().getRebalancingFrequency() > 0 and _world->getCurrentStep() % _world->getConfig().getRebalancingFrequency() == 0;
        if (monitorIt) initialTime = getWallTime();

        agent->updateState();

        if (monitorIt) updateTotalAccordingToExecutingPhase(eUpdateState, initialTime);
    }

    void MPIMultiNode::randomlyExecuteAgents(AgentsVector& agentsToExecute)
    {
        GeneralState::statistics().shuffleWithinIterators(agentsToExecute.begin(), agentsToExecute.end());

        #pragma omp parallel for schedule(dynamic) if(_updateKnowledgeInParallel)
        for (int i = 0; i < agentsToExecute.size(); ++i)
        {
            Agent* agent = agentsToExecute[i].get();
            agent_updateKnowledge(agent);
            agent_selectActions(agent);
        }

        #pragma omp parallel for schedule(dynamic) if(_executeActionsInParallel)
        for (int i = 0; i < agentsToExecute.size(); ++i)
        {
            Agent* agent = agentsToExecute[i].get();
            agent_executeActions(agent);
            agent_updateState(agent);
        }
    }

    bool MPIMultiNode::hasBeenExecuted(const std::string& agentID) const
    {
        return not (_executedAgentsInStep.find(agentID) == _executedAgentsInStep.end());
    }

    void MPIMultiNode::executeAgentsInSubOverlap(AgentsVector& agentsVector, const int& subOverlapID)
    {
        double initialTime = getWallTime();

        Rectangle<int> areaToExecute;
        if (subOverlapID == 0)  areaToExecute = _nodeSpace.ownedAreaWithoutInnerOverlap;
        else                    areaToExecute = _nodeSpace.innerSubOverlaps[subOverlapID];

        AgentsVector agentsToExecute;
        for (AgentsMap::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            AgentPtr agentPtr = it->second;
            std::string agentID = agentPtr->getId();
            if (agentPtr.get()->exists() and not hasBeenExecuted(agentID) and areaToExecute.contains(agentPtr.get()->getPosition()))
            {
                _executedAgentsInStep.insert(agentID);
                agentsToExecute.push_back(agentPtr);
            }
        }
        randomlyExecuteAgents(agentsToExecute);

        agentsVector.insert(agentsVector.end(), agentsToExecute.begin(), agentsToExecute.end());

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("\n[Process # " << getId() <<  "] MPIMultiNode::executeAgentsInSubOverlap() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::initializeAgentsToSendMap(std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode) const
    {
        for (std::map<int, MPINode*>::const_iterator itNode = _nodeSpace.neighbours.begin(); itNode != _nodeSpace.neighbours.end(); ++itNode)
        {
            int neighbourID = itNode->first;

            agentsByTypeAndNode[neighbourID] = std::map<std::string, AgentsList>();
        }
    }

    void MPIMultiNode::sendDataRequestToNode(void* data, const int& numberOfElements, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag, const MPI_Comm& mpiComm)
    {
        MPI_Request* mpiRequest = new MPI_Request;

        MPI_Isend(data, numberOfElements, mpiDatatype, destinationNode, tag, mpiComm, mpiRequest);
        _sendRequests.push_back(mpiRequest);
    }

    void MPIMultiNode::sendGhostAgentsInMap(const std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode, const int& subOverlapID)
    {
        double initialTime = getWallTime();

        sendAgentsInMap(agentsByTypeAndNode);

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::sendGhostAgentsInMap() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::receiveGhostAgentsFromNeighbouringNodes(const int& subOverlapID)
    {
        double initialTime = getWallTime();

        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int sendingNodeID = it->first;

            int numberOfAgentTypesToReceive;
            MPI_Recv(&numberOfAgentTypesToReceive, 1, MPI_INT, sendingNodeID, eNumGhostAgentsType, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int i = 0; i < numberOfAgentTypesToReceive; ++i)
            {
                int agentTypeID;
                MPI_Recv(&agentTypeID, 1, MPI_INT, sendingNodeID, eGhostAgentsType, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::string agentsTypeName = MpiFactory::instance()->getNameFromTypeID(agentTypeID);
                MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentsTypeName);

                int numberOfAgentsToReceive;
                MPI_Recv(&numberOfAgentsToReceive, 1, MPI_INT, sendingNodeID, eNumGhostAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " receiving numberOfAgentsToReceive: " << numberOfAgentsToReceive << " of type: " << agentsTypeName << "\tfrom node: " << sendingNodeID << "\n").str();

                int sizeOfAgentPackage = MpiFactory::instance()->getSizeOfPackage(agentsTypeName);
                
                void* agentsPackageArray = malloc(numberOfAgentsToReceive * sizeOfAgentPackage);
                MPI_Recv(agentsPackageArray, numberOfAgentsToReceive, *agentTypeMPI, sendingNodeID, eGhostAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int j = 0; j < numberOfAgentsToReceive; ++j)
                {
                    void* package = (char*) agentsPackageArray + j * sizeOfAgentPackage;
                    Agent* agent = MpiFactory::instance()->createAndFillAgent(agentsTypeName, package);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " receiving agent: " << agent << "\tfrom node: " << sendingNodeID << "\n").str();

                    AgentsMap agentsByID = _world->getAgentsMap();
                    if (agentsByID.find(agent->getId()) != agentsByID.end())
                        _world->eraseAgent(agentsByID.at(agent->getId()).get());

                    if (_nodeSpace.ownedAreaWithOuterOverlap.contains(agent->getPosition()))
                    {
                        _world->addAgent(agent);
                        _executedAgentsInStep.insert(agent->getId());
                    }
                }
                free(agentsPackageArray);
            }
        }

        // This line is not needed because in principle agents are already sorted when inserted in the map.
        //_world->sortAgentsListAlphabetically();     // Important when receiving agents asynchronously! Subsequent shuffles must obtain the same resulting list, so we need to sort the list of agents.

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::receiveGhostAgentsFromNeighbouringNodes() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::getRealNeighboursForAgent(std::list<int>& subOverlapNeighboursIDs, const std::list<int>& potentialNeighbours, const Point2D<int>& agentPosition) const
    {
        for (std::list<int>::const_iterator potentialNeighboursIt = potentialNeighbours.begin(); potentialNeighboursIt != potentialNeighbours.end(); ++potentialNeighboursIt)
        {
            int potentialNeighbourID = *potentialNeighboursIt;
            Rectangle<int> potentialNeighbourArea = _nodeSpace.neighbours.at(potentialNeighbourID)->ownedAreaWithOuterOverlap;

            if (potentialNeighbourArea.contains(agentPosition))
                subOverlapNeighboursIDs.push_back(potentialNeighbourID);
        }
    }

    void MPIMultiNode::getNeighboursToSendAgentInsideInnerSubOverlap(std::list<int>& neighbouringNodeIDs, const Agent& agent) const
    {
        for (std::map<int, Rectangle<int>>::const_iterator subOverlapsIt = _nodeSpace.innerSubOverlaps.begin(); subOverlapsIt != _nodeSpace.innerSubOverlaps.end(); ++subOverlapsIt)
        {
            int currentSubOverlapID = subOverlapsIt->first;
            Rectangle<int> subOverlapArea = subOverlapsIt->second;

            if (subOverlapArea.contains(agent.getPosition()))
            {
                std::list<int> potentialNeighbours = _nodeSpace.innerSubOverlapsNeighbours.at(currentSubOverlapID);
                getRealNeighboursForAgent(neighbouringNodeIDs, potentialNeighbours, agent.getPosition());
                break;
            }
        }
    }

    void MPIMultiNode::synchronizeAgentsIfNecessary(const AgentsVector& agentsVector, const int& subOverlapID)
    {
        if (_numTasks == 1) return;

        double initialTime = getWallTime();

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

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::synchronizeAgentsIfNecessary() (PREPARATION OF THE DATA) OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());

        sendGhostAgentsInMap(agentsByTypeAndNode, subOverlapID);
        receiveGhostAgentsFromNeighbouringNodes(subOverlapID);
    }

    void MPIMultiNode::addSubOverlapNeighboursFromPosition(std::list<int>& subOverlapNeighboursIDs, const int& subOverlapID, const Point2D<int>& agentPosition) const
    {
        if (subOverlapID >= eTopLeft_Mode4 and subOverlapID <= eBottomRight_Mode4 or subOverlapID >= eTopCenter_Mode9 and subOverlapID <= eBottomRight_Mode9)
        {
            std::list<int> potentialNeighbours = _nodeSpace.innerSubOverlapsNeighbours.at(subOverlapID);

            std::list<int> realNeighbours;
            getRealNeighboursForAgent(realNeighbours, potentialNeighbours, agentPosition);

            subOverlapNeighboursIDs.insert(subOverlapNeighboursIDs.end(), realNeighbours.begin(), realNeighbours.end());
            subOverlapNeighboursIDs.sort();
            subOverlapNeighboursIDs.unique();
        }
        else
        {
            std::cout << CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::addSubOverlapNeighboursFromPosition() - subOverlapID not valid: " << subOverlapID << "\n").str();
            terminateAllMPIProcesses();
        }
        
    }

    void MPIMultiNode::getNeighboursThatNeedToRemoveAgent(std::list<int>& subOverlapNeighboursIDs, const int& originalSubOverlapAreaID, const Agent& agent) const
    {
        addSubOverlapNeighboursFromPosition(subOverlapNeighboursIDs, originalSubOverlapAreaID, agent.getDiscretePosition());
    }

    void MPIMultiNode::getNeighboursThatNeedToAddAgent(std::list<int>& subOverlapNeighboursIDs, const int& originalSubOverlapAreaID, const Agent& agent) const
    {
        addSubOverlapNeighboursFromPosition(subOverlapNeighboursIDs, originalSubOverlapAreaID, agent.getPosition());
    }

    void MPIMultiNode::getNeighboursForAgentAccordingToSubpartitioningMode(std::list<int>& neighbouringNodeIDs, const int& originalSubOverlapAreaID, const Agent& agent, const int& subpartitioningMode) const
    {
        Point2D<int> agentDiscretePosition = agent.getDiscretePosition();
        Point2D<int> agentPosition = agent.getPosition();

        bool isInInnerMostArea = _nodeSpace.ownedAreaWithoutInnerOverlap.contains(agentPosition);
        bool wereInInnerMostArea = _nodeSpace.ownedAreaWithoutInnerOverlap.contains(agentDiscretePosition);

        bool needToSend;
        if (subpartitioningMode == eMode4) needToSend = not wereInInnerMostArea or not isInInnerMostArea;
        else if (subpartitioningMode == eMode9) needToSend = true;

        if (needToSend)
        {
            if (isInInnerMostArea)          // Case #9.3 and #4.5, respectively
                getNeighboursThatNeedToRemoveAgent(neighbouringNodeIDs, originalSubOverlapAreaID, agent);
            else if (wereInInnerMostArea)   // Case #4.2 (for mode9 wereInInnerMostArea will always be false)
                getNeighboursThatNeedToAddAgent(neighbouringNodeIDs, originalSubOverlapAreaID, agent);
            else                            // Case #9.4, #9.5 and #9.6 and #4.3, #4.7, #4.6, respectively.
            {
                getNeighboursThatNeedToRemoveAgent(neighbouringNodeIDs, originalSubOverlapAreaID, agent);
                getNeighboursThatNeedToAddAgent(neighbouringNodeIDs, originalSubOverlapAreaID, agent);
            }
            
        }
    }

    void MPIMultiNode::sendGhostAgentsToNeighbours(const AgentsVector& agentsVector, const int& originalSubOverlapAreaID, const int& subpartitioningMode)
    {
        double initialTime = getWallTime();

        std::map<int, std::map<std::string, AgentsList>> agentsByTypeAndNode;
        initializeAgentsToSendMap(agentsByTypeAndNode);

        for (AgentsVector::const_iterator itAgent = agentsVector.begin(); itAgent != agentsVector.end(); ++itAgent)
        {
            AgentPtr agentPtr = *itAgent;
            Agent* agent = agentPtr.get();
            std::string agentType = agent->getType();

            std::list<int> neighbouringNodeIDs; 
            getNeighboursForAgentAccordingToSubpartitioningMode(neighbouringNodeIDs, originalSubOverlapAreaID, *agent, subpartitioningMode);

            for (std::list<int>::const_iterator itNeighbourNodeID = neighbouringNodeIDs.begin(); itNeighbourNodeID != neighbouringNodeIDs.end(); ++itNeighbourNodeID)
            {
                int neighbourNodeID = *itNeighbourNodeID;

                if (agentsByTypeAndNode.at(neighbourNodeID).find(agentType) == agentsByTypeAndNode.at(neighbourNodeID).end())
                    agentsByTypeAndNode.at(neighbourNodeID)[agentType] = AgentsList();

                agentsByTypeAndNode.at(neighbourNodeID).at(agentType).push_back(agentPtr);
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::sendGhostAgentsToNeighbours() (PREPARATION OF THE DATA) OVERLAP: " << originalSubOverlapAreaID << "\tTOTAL TIME: " << endTime - initialTime).str());

        sendGhostAgentsInMap(agentsByTypeAndNode, originalSubOverlapAreaID);
    }

    void MPIMultiNode::initializeRasterValuesToSendMap(std::map<int, MapOfValuesByRaster>& rasterValuesByNode) const
    {
        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int neighbourID = it->first;

            rasterValuesByNode[neighbourID] = std::map<int, MapOfPositionsAndValues>();

            for (int i = 0; i < _world->getNumberOfRasters(); ++i)
            {
                if (_world->rasterExists(i) and _world->isRasterDynamic(i))
                    rasterValuesByNode[neighbourID][i] = MapOfPositionsAndValues();
            }
        }
    }

    void MPIMultiNode::getNeighbourIDsInInnerSubOverlap(const Point2D<int>& point, std::list<int>& neighbourIDs) const
    {
        for (std::map<int, Rectangle<int>>::const_iterator subOverlapIt = _nodeSpace.innerSubOverlaps.begin(); subOverlapIt != _nodeSpace.innerSubOverlaps.end(); ++subOverlapIt)
        {
            int subOverlapID = subOverlapIt->first;
            Rectangle<int> subOverlapArea = subOverlapIt->second;

            if (subOverlapArea.contains(point))
            {
                for (std::list<int>::const_iterator neighboursIt = _nodeSpace.innerSubOverlapsNeighbours.at(subOverlapID).begin(); neighboursIt != _nodeSpace.innerSubOverlapsNeighbours.at(subOverlapID).end(); ++neighboursIt)
                {
                    int neighbourID = *neighboursIt;
                    if (_nodeSpace.neighbours.at(neighbourID)->ownedAreaWithOuterOverlap.contains(point))
                        neighbourIDs.push_back(neighbourID);
                }
                break;
            }
        }
    }

    void MPIMultiNode::getNeighbourIDsInOuterSubOverlap(const Point2D<int>& point, std::list<int>& neighbourIDs) const
    {
        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int neighbourID = it->first;
            Rectangle<int> neighbourAreaWithOuterOverlap = it->second->ownedAreaWithOuterOverlap;

            if (neighbourAreaWithOuterOverlap.contains(point))
                neighbourIDs.push_back(neighbourID);
        }
    }

    void MPIMultiNode::addPositionAndValueToMap(MapOfPositionsAndValues& map, const Point2D<int>& position, const int& value)
    {
        if (map.find(position) == map.end())
            map[position] = value;
        else
            map.at(position) = value;
    }

    void MPIMultiNode::sendRasterValuesInMap(const std::map<int, MapOfValuesByRaster>& rasterValuesByNode, const int& subOverlapID)
    {
        double initialTime = getWallTime();

        for (std::map<int, MapOfValuesByRaster>::const_iterator rasterValuesByNodeIt = rasterValuesByNode.begin(); rasterValuesByNodeIt != rasterValuesByNode.end(); ++rasterValuesByNodeIt)
        {
            int neighbourNodeID = rasterValuesByNodeIt->first;
            MapOfValuesByRaster valuesByRaster = rasterValuesByNodeIt->second;

            int numberOfRasters = valuesByRaster.size();
            sendDataRequestToNode(&numberOfRasters, 1, MPI_INT, neighbourNodeID, eNumRasters, MPI_COMM_WORLD);

            for (MapOfValuesByRaster::const_iterator valuesByRasterIt = valuesByRaster.begin(); valuesByRasterIt != valuesByRaster.end(); ++valuesByRasterIt)
            {
                int rasterIndex = valuesByRasterIt->first;
                MapOfPositionsAndValues positionsAndValues = valuesByRasterIt->second;

                sendDataRequestToNode(&rasterIndex, 1, MPI_INT, neighbourNodeID, eRasterIndex, MPI_COMM_WORLD);

                int numberOfPositions = positionsAndValues.size();
                sendDataRequestToNode(&numberOfPositions, 1, MPI_INT, neighbourNodeID, eNumRasterPositions, MPI_COMM_WORLD);

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
                sendDataRequestToNode(positionAndValueArray, numberOfPositions, *_positionAndValueDatatype, neighbourNodeID, ePosAndValue, MPI_COMM_WORLD);
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::sendRasterValuesInMap() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::sendRastersToNeighbours(const int& subOverlapID)
    {
        if (_numTasks == 1) return;

        double initialTime = getWallTime();

        std::map<int, MapOfValuesByRaster> rastersValuesByNode;
        initializeRasterValuesToSendMap(rastersValuesByNode);

        for (int i = 0; i < _changedRastersCells.size(); ++i)
        {
            int rasterIndex = _changedRastersCells[i].first;
            Point2D<int> position = _changedRastersCells[i].second;

            int changedValue = _world->getValue(rasterIndex, position);

            bool isInInnerOverlapArea = _nodeSpace.ownedArea.contains(position) and not _nodeSpace.ownedAreaWithoutInnerOverlap.contains(position);
            bool isInOuterOverlapArea = _nodeSpace.ownedAreaWithOuterOverlap.contains(position) and not _nodeSpace.ownedArea.contains(position);

            if (isInInnerOverlapArea or isInOuterOverlapArea)
            {
                std::list<int> neighbourIDs;

                if (isInInnerOverlapArea) getNeighbourIDsInInnerSubOverlap(position, neighbourIDs);
                else if (isInOuterOverlapArea) getNeighbourIDsInOuterSubOverlap(position, neighbourIDs);

                for (std::list<int>::const_iterator neighboursIt = neighbourIDs.begin(); neighboursIt != neighbourIDs.end(); ++neighboursIt)
                {
                    int neighbourID = *neighboursIt;

                    addPositionAndValueToMap(rastersValuesByNode.at(neighbourID).at(rasterIndex), position, changedValue);
                }
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::sendRastersToNeighbours() (PREPARATION OF THE DATA) OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());

        sendRasterValuesInMap(rastersValuesByNode, subOverlapID);

        _changedRastersCells.clear();
    }

    void MPIMultiNode::receiveRasters(const int& subOverlapID)
    {
        if (_numTasks == 1) return;

        double initialTime = getWallTime();

        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int sendingNodeID = it->first;
            int numberOfRasters;
            MPI_Recv(&numberOfRasters, 1, MPI_INT, sendingNodeID, eNumRasters, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int i = 0; i < numberOfRasters; ++i)
            {
                int rasterIndex;
                MPI_Recv(&rasterIndex, 1, MPI_INT, sendingNodeID, eRasterIndex, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                if (not _world->rasterExists(rasterIndex))
                { 
                    std::cout << CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::receiveRasters() - raster with index: " << rasterIndex << " not found.\n").str();
                    terminateAllMPIProcesses();
                }

                int numberOfPositions;
                MPI_Recv(&numberOfPositions, 1, MPI_INT, sendingNodeID, eNumRasterPositions, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                PositionAndValue positionAndValueArray[numberOfPositions];
                MPI_Recv(positionAndValueArray, numberOfPositions, *_positionAndValueDatatype, sendingNodeID, ePosAndValue, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int j = 0; j < numberOfPositions; ++j)
                {
                    PositionAndValue positionAndValue = positionAndValueArray[j];

                    Point2D<int> position = Point2D<int>(positionAndValue.x, positionAndValue.y);
                    int continuousValue = positionAndValue.value;

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " receiving raster index: " << rasterIndex << " position: " << position << " and value: " << continuousValue << "\tfrom node: " << sendingNodeID << "\n").str();

                    _world->getDynamicRaster(rasterIndex).setValue(position, continuousValue);
                }
            }
        }

        double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::receiveRasters() OVERLAP: " << subOverlapID << "\tTOTAL TIME: " << endTime - initialTime).str());
    }

    void MPIMultiNode::clearRequests()
    {
        for (std::list<MPI_Request*>::const_iterator it = _sendRequests.begin(); it != _sendRequests.end(); ++it)
        {
            MPI_Request* mpiRequest = *it;
            MPI_Wait(mpiRequest, MPI_STATUS_IGNORE);
        }
    }

    void MPIMultiNode::finishSleepingProcesses()
    {
        if (getId() == _masterNodeID) 
        {
            int totalNumberOfProcesses;
            MPI_Comm_size(MPI_COMM_WORLD, &totalNumberOfProcesses);
            for (int processID = _numberOfActiveProcesses; processID < totalNumberOfProcesses; ++processID)
            {
                sendDataRequestToNode(&totalNumberOfProcesses, 1, MPI_INT, processID, eProcessWakeUp, MPI_COMM_WORLD);

                int endingEventType = eMessage_Die;
                sendDataRequestToNode(&endingEventType, 1, MPI_INT, processID, eTypeOfEventAfterWakeUp, MPI_COMM_WORLD);
            }
        }
    }

    int MPIMultiNode::getTotalNumberOfOverlappingCells(const MPILoadBalanceTree& tree) const
    {
        int totalNumberOfOverlappingCells = 0;

        int worldWidth = tree.getWorld().getConfig().getSize().getWidth();
        int worldHeight = tree.getWorld().getConfig().getSize().getHeight();

        int overlapSize = tree.getWorld().getConfig().getOverlapSize();

        std::vector<Rectangle<int>> partitions;
        tree.getPartitionsFromTree(partitions);
        for (int i = 0; i < partitions.size(); ++i)
        {
            Rectangle<int> rectangle = partitions[i];

            const int rectangleWidth = rectangle.getSize().getWidth();
            const int rectangleHeight = rectangle.getSize().getHeight();

            if (rectangle.left() == 0 and rectangle.top() == 0 or
                rectangle.right() + 1 == worldWidth and rectangle.top() == 0 or
                rectangle.right() + 1 == worldWidth and rectangle.bottom() + 1 == worldHeight or
                rectangle.left() == 0 and rectangle.bottom() + 1 == worldHeight)
                    totalNumberOfOverlappingCells += overlapSize * rectangleWidth + overlapSize * rectangleHeight - std::pow(overlapSize, 2);
            else if (   rectangle.left() == 0 or
                        rectangle.right() + 1 == worldWidth)
                    totalNumberOfOverlappingCells += 2*(overlapSize * rectangleWidth) + overlapSize * rectangleHeight - 2*std::pow(overlapSize, 2);
            else if (   rectangle.top() == 0 or
                        rectangle.bottom() + 1 == worldHeight)
                    totalNumberOfOverlappingCells += overlapSize * rectangleWidth + 2*(overlapSize * rectangleHeight) - 2*std::pow(overlapSize, 2);
            else
                    totalNumberOfOverlappingCells += 2*(overlapSize * rectangleWidth) + 2*(overlapSize * rectangleHeight) - 4*std::pow(overlapSize, 2);
        }
        return totalNumberOfOverlappingCells;
    }

    /** MPIAutoAdjustment CALLED ROUTINES **/

    int MPIMultiNode::performDivideTest(const int& numberOfProcesses)
    {
        MPILoadBalanceTree* testTree = new MPILoadBalanceTree();

        testTree->setWorld(_world);
        testTree->initializeTree();
        testTree->setNumberOfPartitions(numberOfProcesses);
        testTree->divideSpace();
        return getTotalNumberOfOverlappingCells(*testTree);
    }

    void MPIMultiNode::resetPartitioning(const int& newNumberOfProcesses)
    {
        // Deleting map pointers up to 1 depth:
        for (MPINodesMap::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            MPINode mpiNode = it->second;
            for (std::map<int, MPINode*>::const_iterator itNeighbours = mpiNode.neighbours.begin(); itNeighbours != mpiNode.neighbours.end(); ++itNeighbours)
                delete itNeighbours->second;
        }
        _mpiNodesMapToSend.clear();

        _loadBalanceTree->resetTree();
        _loadBalanceTree->setNumberOfPartitions(newNumberOfProcesses);
    }

    void MPIMultiNode::sendAgentsInMap(const std::map<int, std::map<std::string, AgentsList>>& agentsByTypeAndNode)
    {
        for (std::map<int, std::map<std::string, AgentsList>>::const_iterator itNeighbourNode = agentsByTypeAndNode.begin(); itNeighbourNode != agentsByTypeAndNode.end(); ++itNeighbourNode)
        {
            int neighbourNodeID = itNeighbourNode->first;
            std::map<std::string, AgentsList> agentsByType = itNeighbourNode->second;

            int numberOfAgentTypesToSend = agentsByType.size();
            sendDataRequestToNode(&numberOfAgentTypesToSend, 1, MPI_INT, neighbourNodeID, eNumGhostAgentsType, MPI_COMM_WORLD);

            for (std::map<std::string, AgentsList>::const_iterator itType = agentsByType.begin(); itType != agentsByType.end(); ++itType)
            {
                std::string agentsTypeName = itType->first;
                AgentsList agentsToSend = itType->second;

                int agentsTypeID = MpiFactory::instance()->getIDFromTypeName(agentsTypeName);
                sendDataRequestToNode(&agentsTypeID, 1, MPI_INT, neighbourNodeID, eGhostAgentsType, MPI_COMM_WORLD);

                int numberOfAgentsToSend = agentsToSend.size();
                sendDataRequestToNode(&numberOfAgentsToSend, 1, MPI_INT, neighbourNodeID, eNumGhostAgents, MPI_COMM_WORLD);

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
                sendDataRequestToNode(agentsPackageArray, numberOfAgentsToSend, *agentTypeMPI, neighbourNodeID, eGhostAgents, MPI_COMM_WORLD);
                
                //free(agentsPackageArray);
            }
        }
    }

    /** RUN PUBLIC METHODS (INHERIT) **/

    bool MPIMultiNode::hasBeenTaggedAsJustFinished() const
    {
        return _justFinished;
    }

    bool MPIMultiNode::hasBeenTaggedAsJustAwaken() const
    {
        return _justAwaken;
    }

    void MPIMultiNode::updateEnvironmentState()
    {
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::updateEnvironmentState() STEP: " << _world->getCurrentStep() << " ==================================================================================\n").str());

        prepareAgentsAndRastersStateForCurrentStep();
        sendRastersToNeighbours();
        receiveRasters();

        MPI_Barrier(_activeProcessesComm);
    }

    void MPIMultiNode::checkForRebalancingSpace()
    {
        if (_world->getConfig().getAutoMode())
        {
            if (_world->getCurrentStep() > 0 and _world->getCurrentStep() % _world->getConfig().getRebalancingFrequency() == 0) 
                _autoAdjustment->checkForRebalancingSpace();
        }
    }

    void MPIMultiNode::executeAgents()
    {
        AgentsVector executedAgentsInArea;
        
        double initialTime;

_schedulerLogs->printAgentsMatrixInDebugFile(true);

        if (_numTasks == 1 or (_numTasks > 1 and _subpartitioningMode == 9))
        {
_schedulerLogs->writeInDebugFile(CreateStringStream("AGENTS AT THE BEGINNING OF STEP " << _world->getCurrentStep() << ":").str());
_schedulerLogs->printNodeAgentsInDebugFile(true);

_schedulerLogs->writeInDebugFile(CreateStringStream("RASTERS AT THE BEGINNING OF STEP " << _world->getCurrentStep() << ":").str());
_schedulerLogs->printNodeRastersInDebugFile();

            executeAgentsInSubOverlap(executedAgentsInArea, 0);
            synchronizeAgentsIfNecessary(executedAgentsInArea, 0);

            sendRastersToNeighbours(0);
            receiveRasters(0);

            initialTime = getWallTime();
            MPI_Barrier(_activeProcessesComm);
            double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::executeAgents() MPI_Barrier() AFTER OVERLAP: " << 0 << "\tTOTAL TIME: " << endTime - initialTime).str());

_schedulerLogs->writeInDebugFile(CreateStringStream("AGENTS AT STEP " << _world->getCurrentStep() << "; INNER_MOST EXECUTED:").str());
_schedulerLogs->printNodeAgentsInDebugFile(true);

_schedulerLogs->writeInDebugFile(CreateStringStream("RASTERS AT STEP " << _world->getCurrentStep() << "; INNER_MOST EXECUTED:").str());
_schedulerLogs->printNodeRastersInDebugFile();

if (_printInConsole) std::cout << CreateStringStream("\n").str();
        }

        for (std::map<int, Rectangle<int>>::const_iterator it = _nodeSpace.innerSubOverlaps.begin(); it != _nodeSpace.innerSubOverlaps.end(); ++it)
        {
            int originalSubOverlapAreaID = it->first;
            Rectangle<int> originalOverlapArea = it->second;

            executedAgentsInArea.clear();
if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "] " << getWallTime() << " executing agents in suboverlap #" << originalSubOverlapAreaID << "\n").str();
            executeAgentsInSubOverlap(executedAgentsInArea, originalSubOverlapAreaID);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "] " << getWallTime() << " agents executed -> synchronizing\n").str();
            sendGhostAgentsToNeighbours(executedAgentsInArea, originalSubOverlapAreaID, _subpartitioningMode);
            receiveGhostAgentsFromNeighbouringNodes(originalSubOverlapAreaID);

if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "] " << getWallTime() << " agents synchronized\n").str();
if (_printInConsole) std::cout << CreateStringStream("[Process # " << getId() <<  "]\n").str();

            sendRastersToNeighbours(originalSubOverlapAreaID);
            receiveRasters(originalSubOverlapAreaID);

            //clearRequests(); // Is this really necessary?

            initialTime = getWallTime();
            MPI_Barrier(_activeProcessesComm);
            double endTime = getWallTime();
if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::executeAgents() MPI_Barrier() AFTER OVERLAP: " << originalSubOverlapAreaID << "\tTOTAL TIME: " << endTime - initialTime).str());


_schedulerLogs->writeInDebugFile(CreateStringStream("AGENTS AT STEP " << _world->getCurrentStep() << "; AFTER OVERLAP: " << originalSubOverlapAreaID).str());
_schedulerLogs->printNodeAgentsInDebugFile(true);

_schedulerLogs->writeInDebugFile(CreateStringStream("RASTERS AT STEP " << _world->getCurrentStep() << "; AFTER OVERLAP: " << originalSubOverlapAreaID).str());
_schedulerLogs->printNodeRastersInDebugFile();

if (_printInConsole) std::cout << CreateStringStream("\n").str();
        }

if (_printInstrumentation) _schedulerLogs->printInstrumentation(CreateStringStream("\n").str());
    }

    void MPIMultiNode::finish() 
    {
        finishSleepingProcesses();

        MpiFactory::instance()->cleanTypes();

        MPI_Type_free(_coordinatesDatatype);
        MPI_Type_free(_positionAndValueDatatype);

        if (_numberOfActiveProcesses > getId()) MPI_Comm_free(&_activeProcessesComm);

std::string totalSimulationTime = CreateStringStream("[Process #" << getId() << "] MPIMultiNode::finish() TotalSimulationTime: " << MPI_Wtime() - _initialTime << " seconds.\n").str();
std::cout << totalSimulationTime;
if (_printInstrumentation) _schedulerLogs->printInstrumentation(totalSimulationTime);

        MPI_Finalize();
    }

    Point2D<int> MPIMultiNode::getRandomPosition() const
    {
        return Engine::Point2D<int>(GeneralState::statistics().getUniformDistValue(_nodeSpace.ownedArea.left(), _nodeSpace.ownedArea.right()),
                                    GeneralState::statistics().getUniformDistValue(_nodeSpace.ownedArea.top(), _nodeSpace.ownedArea.bottom()));
    }

    double MPIMultiNode::getWallTime() const 
    {
        return MPI_Wtime() - _initialTime;
    }

    void MPIMultiNode::removeAgents()
    {
        removeAgentsInVector(_agentsToBeRemoved);
        _agentsToBeRemoved.clear();
    }

    void MPIMultiNode::addAgentToBeRemoved(Agent* agent)
    {
        AgentsMap agentsByID = _world->getAgentsMap();
        if (agentsByID.find(agent->getId()) == agentsByID.end())
        {
            std::cout << CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::removeAgent(id) - agent: " << agent->getId() << " not found.\n").str();
            terminateAllMPIProcesses();
        }

        agent->setExists(false);
        _agentsToBeRemoved.push_back(agent);
    }

    Agent* MPIMultiNode::getAgent(const std::string& id)
    {
        AgentsMap agentsByID = _world->getAgentsMap();
        if (agentsByID.find(id) != agentsByID.end())
            return agentsByID.at(id).get();
        else
        {
            std::cout << CreateStringStream("[Process # " << getId() <<  "] MPIMultiNode::getAgent(id) - agent: " << id << " not found.\n").str();
            terminateAllMPIProcesses();
        }
    }

    AgentsVector MPIMultiNode::getAgent(const Point2D<int>& position, const std::string& type)
    {
        return _loadBalanceTree->getAgentsInPosition(position, type);
    }

    int MPIMultiNode::countNeighbours(Agent* target, const double& radius, const std::string& type)
    {
        return for_each(_world->beginAgents(), _world->endAgents(), aggregatorCount<std::pair<std::string, AgentPtr>>(radius, *target, type))._count;
    }

    AgentsVector MPIMultiNode::getNeighbours(Agent* target, const double& radius, const std::string& type)
    {
        AgentsVector agentsVector = for_each(_world->beginAgents(), _world->endAgents(), aggregatorGet<std::pair<std::string, AgentPtr>>(radius, *target, type))._neighbors;
        GeneralState::statistics().shuffleWithinIterators(agentsVector.begin(), agentsVector.end());
        return agentsVector;
    }

    size_t MPIMultiNode::getNumberOfTypedAgents(const std::string& type) const
    {
        size_t numberOfAgents = 0;
        for (AgentsMap::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            AgentPtr agentPtr = it->second;
            if (agentPtr->isType(type)) ++numberOfAgents;
        }
        size_t distributedNumberOfAgents;
        MPI_Allreduce(&numberOfAgents, &distributedNumberOfAgents, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
        return distributedNumberOfAgents;
    }

    void MPIMultiNode::setValue(DynamicRaster& raster, const Point2D<int>& position, int value)
    {
        _changedRastersCells.push_back(std::make_pair(raster.getID(), position));

        raster.setValue(position, value);
    }

    int MPIMultiNode::getValue(const DynamicRaster& raster, const Point2D<int>& position) const
    {
        return raster.getValue(position);
    }

    void MPIMultiNode::setMaxValue(DynamicRaster& raster, const Point2D<int>& position, int value)
    {
        raster.setMaxValue(position, value);
    }

    int MPIMultiNode::getMaxValue(const DynamicRaster& raster, const Point2D<int>& position) const
    {
        return raster.getMaxValue(position);
    }

    void MPIMultiNode::addStringAttribute(const std::string& type, const std::string& key, const std::string& value)
    {
        _serializer.addStringAttribute(type, key, value);
    }

    void MPIMultiNode::addIntAttribute(const std::string& type, const std::string& key, int value)
    {
        _serializer.addIntAttribute(type, key, value);
    }

    void MPIMultiNode::addFloatAttribute(const std::string & type, const std::string& key, float value)
    {
        _serializer.addFloatAttribute(type, key, value);
    }

    void MPIMultiNode::serializeAgents(const int& step)
    {
        _serializer.serializeAgents(step, _world->beginAgents(), _world->endAgents());
    }

    void MPIMultiNode::serializeRasters(const int& step)
    {
        _serializer.serializeRasters(step);
    }

    const Rectangle<int>& MPIMultiNode::getOwnedArea() const
    {
        return _nodeSpace.ownedArea;
    }

    const int& MPIMultiNode::getOverlap() const
    {
        return _overlapSize;
    }

} // namespace Engine