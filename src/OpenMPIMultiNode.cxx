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

#include <unistd.h>

namespace Engine {

    /** INITIALIZATION PUBLIC METHODS **/

    OpenMPIMultiNode::OpenMPIMultiNode() : _initialTime(0.0f), _masterNodeID(0), _assignLoadToMasterNode(true), _serializer(*this)
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

        _world->createRasters();

        if (getId() == _masterNodeID) 
        {
            _world->createAgents();

            divideSpace();                          printPartitionsBeforeMPI();
            sendSpacesToNodes();                    printOwnNodeStructureAfterMPI();

            createNeighbouringAgents();             printNeighbouringAgentsPerTypes();
            sendAgentsToNodes();                    printNodeAgents();

            //sendRastersToNodes();
            printNodeRasters();
        }
        else 
        {
            receiveSpacesFromNode(_masterNodeID);   printOwnNodeStructureAfterMPI();
            receiveAgentsFromNode(_masterNodeID);   printNodeAgents();
            //receiveRastersFromNode(_masterNodeID);
            printNodeRasters();
        }

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
    }

    void OpenMPIMultiNode::divideSpace()
    {
        _tree->divideSpace();
        createNodesInformationToSend();

        if (not arePartitionsSuitable()) {
            throw Exception(CreateStringStream("[Process # " << getId() <<  "] Partitions not suitable. Maybe there are too many unnecessary MPI nodes for such a small space, or the overlap size is too wide.").str());
            exit(1);
        }
    }

    void OpenMPIMultiNode::sendSpacesToNodes()
    {
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
    }

    void OpenMPIMultiNode::receiveSpacesFromNode(const int& masterNodeID)
    {
        MPINode mpiNode;
        receiveOwnAreaFromNode(masterNodeID, mpiNode);
        receiveNeighboursFromNode(masterNodeID, mpiNode);
        fillOwnStructures(mpiNode);
    }

    void OpenMPIMultiNode::generateOverlapAreas(MPINode& mpiNode) const
    {
        mpiNode.ownedAreaWithOuterOverlaps = mpiNode.ownedArea;
        mpiNode.ownedAreaWithoutInnerOverlap = mpiNode.ownedArea;

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
    }

    void OpenMPIMultiNode::sendAgentsToNodeByType(const AgentsList& agentsToSend, const int& currentNode, const std::string& agentType) const
    {
        int agentsTypeID = MpiFactory::instance()->getIDFromTypeName(agentType);
        MPI_Send(&agentsTypeID, 1, MPI_INTEGER, currentNode, eAgentTypeID, MPI_COMM_WORLD);

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

    void OpenMPIMultiNode::sendAgentsToNodes()
    {
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
                std::string agentType = itType->first;
                AgentsList agentsToSend = itType->second;

                if (needToSendAgentsToNodeID)
                    sendAgentsToNodeByType(agentsToSend, nodeID, agentType);
                else
                    agentsToKeepInMasterNode.insert(agentsToKeepInMasterNode.end(), agentsToSend.begin(), agentsToSend.end());
            }
        }

        keepAgentsInNode(agentsToKeepInMasterNode);
    }

    void OpenMPIMultiNode::receiveAgentsFromNode(const int& masterNodeID)
    { 
        int numberOfTypesOfAgents;
        MPI_Recv(&numberOfTypesOfAgents, 1, MPI_INTEGER, masterNodeID, eNumTypes, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < numberOfTypesOfAgents; ++i)
        {
            int agentsTypeID;
            MPI_Recv(&agentsTypeID, 1, MPI_INTEGER, masterNodeID, eAgentTypeID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::string agentsTypeName = MpiFactory::instance()->getNameFromTypeID(agentsTypeID);
            MPI_Datatype* agentType = MpiFactory::instance()->getMPIType(agentsTypeName);

            int numberOfAgentsToReceive;
            MPI_Recv(&numberOfAgentsToReceive, 1, MPI_INTEGER, masterNodeID, eNumAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int j = 0; j < numberOfAgentsToReceive; ++j)
            {
                void* package = MpiFactory::instance()->createDefaultPackage(agentsTypeName);
                MPI_Recv(package, 1, *agentType, masterNodeID, eAgent, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                Agent* agent = MpiFactory::instance()->createAndFillAgent(agentsTypeName, package);
                free(package);

                _world->addAgent(agent, false);
            }
        }
    }

    void OpenMPIMultiNode::sendRasterBaseInfoToNode(const DynamicRaster& raster, const int& nodeID) const
    {
        int rasterID = raster.getID();
        std::string rasterName = raster.getName();
        bool rasterSerialize = raster.getSerialize();
        
        MPI_Send(&rasterID, 1, MPI_INTEGER, nodeID, eStaticRasterID, MPI_COMM_WORLD);
        MPI_Send(rasterName.c_str(), rasterName.size(), MPI_CHAR, nodeID, eStaticRasterName, MPI_COMM_WORLD);
        MPI_Send(&rasterSerialize, 1, MPI_INTEGER, nodeID, eStaticRasterSerialize, MPI_COMM_WORLD);
    }

    void OpenMPIMultiNode::sendRasterFileNameToNode(const DynamicRaster& raster, const int& nodeID) const
    {
        std::string rasterFileName = raster.getFileName();
        MPI_Send(rasterFileName.c_str(), rasterFileName.size(), MPI_CHAR, nodeID, eStaticRasterFileName, MPI_COMM_WORLD);
    }

    void OpenMPIMultiNode::sendRasterDynamicInfoToNode(const DynamicRaster& raster, const int& nodeID) const
    {
        int minValue = raster.getMinValue();
        int maxValue = raster.getMaxValue();
        int defaultValue = raster.getDefaultValue();

        MPI_Send(&minValue, 1, MPI_INTEGER, nodeID, eDynamicRasterMinValue, MPI_COMM_WORLD);
        MPI_Send(&maxValue, 1, MPI_INTEGER, nodeID, eDynamicRasterMaxValue, MPI_COMM_WORLD);
        MPI_Send(&defaultValue, 1, MPI_INTEGER, nodeID, eDynamicRasterDefaultValue, MPI_COMM_WORLD);
    }

    void OpenMPIMultiNode::sendRastersToNodes() const
    {
        for (MPINodesMap::const_iterator it = _mpiNodesMapToSend.begin(); it != _mpiNodesMapToSend.end(); ++it)
        {
            int nodeID = it->first;
            if (nodeID != _masterNodeID)
            {
                if (not _assignLoadToMasterNode and nodeID >= _masterNodeID) nodeID += 1;

                int numberOfStaticRasters = _world->getNumberOfStaticRasters();
                MPI_Send(&numberOfStaticRasters, 1, MPI_INTEGER, nodeID, eNumberOfStaticRasters, MPI_COMM_WORLD);
                for (int i = 0; i < _world->getNumberOfRasters(); ++i)
                {
                    if (_world->rasterExists(i) and not _world->isRasterDynamic(i))
                    {
                        DynamicRaster* raster = static_cast<DynamicRaster*>(&(_world->getStaticRaster(i)));
                        sendRasterBaseInfoToNode(*raster, nodeID);
                        sendRasterFileNameToNode(*raster, nodeID);
                    }
                }

                int numberOfDynamicRasters = _world->getNumberOfDynamicRasters();
                MPI_Send(&numberOfDynamicRasters, 1, MPI_INTEGER, nodeID, eNumberOfDynamicRasters, MPI_COMM_WORLD);
                for (int i = 0; i < _world->getNumberOfRasters(); ++i)
                {
                    if (_world->rasterExists(i) and _world->isRasterDynamic(i))
                    {
                        DynamicRaster raster = _world->getDynamicRaster(i);
                        sendRasterBaseInfoToNode(raster, nodeID);
                        sendRasterDynamicInfoToNode(raster, nodeID);
                    }
                }
            }
        }
    }

    std::string OpenMPIMultiNode::receiveStringFromNode(const int& nodeID, const int& tag) const
    {
        MPI_Status mpiStatus;
        int numberOfCharacters;

        MPI_Probe(nodeID, tag, MPI_COMM_WORLD, &mpiStatus);
        MPI_Get_count(&mpiStatus, MPI_CHAR, &numberOfCharacters);

        char rasterName[numberOfCharacters];
        MPI_Recv(rasterName, numberOfCharacters, MPI_CHAR, nodeID, tag, MPI_COMM_WORLD, &mpiStatus);

        std::string rasterNameStr = std::string(rasterName);
        return rasterNameStr.substr(0, numberOfCharacters);
    }

    void OpenMPIMultiNode::receiveRasterBaseInfo(const int& masterNodeID, int& rasterID, std::string& rasterName, bool& rasterSerialize) const
    {
        MPI_Recv(&rasterID, 1, MPI_INTEGER, masterNodeID, eStaticRasterID, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        rasterName = receiveStringFromNode(masterNodeID, eStaticRasterName);
        MPI_Recv(&rasterSerialize, 1, MPI_INTEGER, masterNodeID, eStaticRasterSerialize, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    void OpenMPIMultiNode::receiveRasterDynamicInfo(const int& masterNodeID, int& minValue, int& maxValue, int& defaultValue) const
    {
        MPI_Recv(&minValue, 1, MPI_INTEGER, masterNodeID, eDynamicRasterMinValue, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&maxValue, 1, MPI_INTEGER, masterNodeID, eDynamicRasterMaxValue, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&defaultValue, 1, MPI_INTEGER, masterNodeID, eDynamicRasterDefaultValue, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    void OpenMPIMultiNode::receiveRastersFromNode(const int& masterNodeID)
    {
        int rasterID;
        std::string rasterName;
        bool rasterSerialize;
        std::string rasterFileName;
        int rasterMinValue, rasterMaxValue, rasterDefaultValue;

        int numberOfStaticRastersToReceive;
        MPI_Recv(&numberOfStaticRastersToReceive, 1, MPI_INTEGER, masterNodeID, eNumberOfStaticRasters, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < numberOfStaticRastersToReceive; ++i)
        {
            receiveRasterBaseInfo(masterNodeID, rasterID, rasterName, rasterSerialize);
            rasterFileName = receiveStringFromNode(masterNodeID, eStaticRasterFileName);
            
            _world->registerStaticRaster(rasterName, rasterSerialize, rasterID);
	        Engine::GeneralState::rasterLoader().fillGDALRaster(_world->getStaticRaster(rasterName), rasterFileName, _world->getBoundaries());
        }

        int numberOfDynamicRastersToReceive;
        MPI_Recv(&numberOfDynamicRastersToReceive, 1, MPI_INTEGER, masterNodeID, eNumberOfDynamicRasters, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < numberOfDynamicRastersToReceive; ++i)
        {
            receiveRasterBaseInfo(masterNodeID, rasterID, rasterName, rasterSerialize);
            receiveRasterDynamicInfo(masterNodeID, rasterMinValue, rasterMaxValue, rasterDefaultValue);
            
            _world->registerDynamicRaster(rasterName, rasterSerialize, rasterID);
	        _world->getDynamicRaster(rasterName).setInitValues(rasterMinValue, rasterMaxValue, rasterDefaultValue);
        }
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

    void OpenMPIMultiNode::randomlyExecuteAgents(AgentsVector& agentsToExecute)
    {
        std::random_shuffle(agentsToExecute.begin(), agentsToExecute.end());
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

    void OpenMPIMultiNode::executeAgentsInArea(const Rectangle<int>& areaToExecute, AgentsVector& agentsVector)
    {
        for (AgentsList::const_iterator it = _world->beginAgents(); it != _world->endAgents(); ++it)
        {
            AgentPtr agentPtr = *it;
            std::string agentID = agentPtr.get()->getId();
            if (not hasBeenExecuted(agentID) and areaToExecute.contains(agentPtr->getPosition())) 
            {
                _executedAgentsInStep.insert(agentID);
                agentsVector.push_back(agentPtr);
            }
        }
        randomlyExecuteAgents(agentsVector);
    }

    void OpenMPIMultiNode::initializeAgentsToSend(std::map<int, std::list<Agent*>>& agentsByNode) const
    {
        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int neighbourID = it->first;

            if (neighbourID == _masterNodeID and not _assignLoadToMasterNode) continue;
            agentsByNode[neighbourID] = std::list<Agent*>();
        }
    }

    void OpenMPIMultiNode::sendDataRequestToNode(void* data, const MPI_Datatype& mpiDatatype, const int& destinationNode, const int& tag)
    {
        MpiRequest* mpiRequest = new MpiRequest;
        mpiRequest->package = data;
        mpiRequest->requestType = tag;

        MPI_Isend(data, 1, mpiDatatype, destinationNode, tag, MPI_COMM_WORLD, &mpiRequest->request);
        _sendRequests.push_back(mpiRequest);
    }

    void OpenMPIMultiNode::sendGhostAgentsInMap(const std::map<int, std::list<Agent*>>& agentsByNode)
    {
        for (std::map<int, std::list<Agent*>>::const_iterator itNeighbourNode = agentsByNode.begin(); itNeighbourNode != agentsByNode.end(); ++itNeighbourNode)
        {
            int neighbourNodeID = itNeighbourNode->first;
            std::list<Agent*> agentsList = itNeighbourNode->second;
            
            int numberOfAgentsForNode = agentsList.size();
std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " sending numberOfAgents: " << numberOfAgentsForNode << "\tto node: " << neighbourNodeID << "\n").str();
            sendDataRequestToNode(&numberOfAgentsForNode, MPI_INTEGER, neighbourNodeID, eNumGhostAgents);

            for (std::list<Agent*>::const_iterator itAgents = agentsList.begin(); itAgents != agentsList.end(); ++itAgents)
            {
                Agent* agent = *itAgents;
                std::string agentType = agent->getType();

std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " sending agent: " << agent << "\tto node: " << neighbourNodeID << "\n").str();

                int agentTypeID = MpiFactory::instance()->getIDFromTypeName(agentType);
                sendDataRequestToNode(&agentTypeID, MPI_INTEGER, neighbourNodeID, eGhostAgentType);

                void* agentPackage = agent->fillPackage();
                MPI_Datatype* agentTypeMPI = MpiFactory::instance()->getMPIType(agentType);
                sendDataRequestToNode(agentPackage, *agentTypeMPI, neighbourNodeID, eGhostAgent);
            }
        }
    }

    void OpenMPIMultiNode::receiveGhostAgentsFromNeighbouringNodes()
    {
        for (std::map<int, MPINode*>::const_iterator it = _nodeSpace.neighbours.begin(); it != _nodeSpace.neighbours.end(); ++it)
        {
            int sendingNodeID = it->first;
            int numberOfAgents;
std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " prepare to receive numberOfAgents from node: " << sendingNodeID << "\n").str();
            MPI_Recv(&numberOfAgents, 1, MPI_INTEGER, sendingNodeID, eNumGhostAgents, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " receiving numberOfAgents: " << numberOfAgents << "\tfrom node: " << sendingNodeID << "\n").str();
            for (int j = 0; j < numberOfAgents; ++j)
            {
                int agentTypeID;
                MPI_Recv(&agentTypeID, 1, MPI_INTEGER, sendingNodeID, eGhostAgentType, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::string agentTypeName = MpiFactory::instance()->getNameFromTypeID(agentTypeID);
                MPI_Datatype* agentType = MpiFactory::instance()->getMPIType(agentTypeName);

                void* package = MpiFactory::instance()->createDefaultPackage(agentTypeName);
                MPI_Recv(package, 1, *agentType, sendingNodeID, eGhostAgent, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                Agent* agent = MpiFactory::instance()->createAndFillAgent(agentTypeName, package);
                free(package);

std::cout << CreateStringStream("[Process # " << getId() <<  "]\t" << getWallTime() << " receiving agent: " << agent << "\tfrom node: " << sendingNodeID << "\n").str();

                AgentsList::const_iterator agentIt = getAgentIteratorFromID(agent->getId());
                if (agentIt != _world->endAgents())
                    _world->eraseAgent(agentIt);

                if (_nodeSpace.ownedAreaWithOuterOverlaps.contains(agent->getPosition()))
                {
                    _world->addAgent(agent, true);
                    _executedAgentsInStep.insert(agent->getId());
                }
            }
        }
    }

    std::list<int> OpenMPIMultiNode::getNeighboursToSendAgent(const Agent& agent) const
    {
        std::list<int> resultingList = std::list<int>();

        for (std::map<int, Rectangle<int>>::const_iterator it = _nodeSpace.innerSubOverlaps.begin(); it != _nodeSpace.innerSubOverlaps.end(); ++it)
        {
            int currentSubOverlapID = it->first;
            Rectangle<int> subOverlapArea = it->second;

            if (subOverlapArea.contains(agent.getPosition()))
            {
                resultingList = _nodeSpace.innerSubOverlapsNeighbours.at(currentSubOverlapID);
                break;
            }
        }

        return resultingList;
    }

    void OpenMPIMultiNode::addSubOverlapNeighboursToList(std::list<int>& subOverlapNeighboursIDs, const int& subOverlapID) const
    {
        if (subOverlapID >= 1 and subOverlapID <= eNumberOfSubOverlaps)
        {
            std::list<int> originalSubOverlapNeighbourList = _nodeSpace.innerSubOverlapsNeighbours.at(subOverlapID);
            subOverlapNeighboursIDs.insert(subOverlapNeighboursIDs.end(), originalSubOverlapNeighbourList.begin(), originalSubOverlapNeighbourList.end());
            subOverlapNeighboursIDs.unique();
        }
    }

    void OpenMPIMultiNode::synchronizeAgentsIfNecessary(const AgentsVector& agentsVector)
    {
        std::map<int, std::list<Agent*>> agentsByNode;

        initializeAgentsToSend(agentsByNode);

        for (AgentsVector::const_iterator itAgent = agentsVector.begin(); itAgent != agentsVector.end(); ++itAgent)
        {
            Agent* agent = itAgent->get();

            if (agent->exists())
            {
                std::list<int> subOverlapNeighboursIDs = getNeighboursToSendAgent(*agent);
                for (std::list<int>::const_iterator itNeighbourNodeID = subOverlapNeighboursIDs.begin(); itNeighbourNodeID != subOverlapNeighboursIDs.end(); ++itNeighbourNodeID)
                {
                    int neighbourID = *itNeighbourNodeID;
                    agentsByNode.at(neighbourID).push_back(agent);
                }
            }
        }

        sendGhostAgentsInMap(agentsByNode);
        receiveGhostAgentsFromNeighbouringNodes();
    }

    void OpenMPIMultiNode::sendGhostAgentsToNeighbours(const AgentsVector& agentsVector, const int& originalSubOverlapAreaID)
    {
        std::map<int, std::list<Agent*>> agentsByNode;

        initializeAgentsToSend(agentsByNode);

        for (AgentsVector::const_iterator itAgent = agentsVector.begin(); itAgent != agentsVector.end(); ++itAgent)
        {
            Agent* agent = itAgent->get();

            std::list<int> subOverlapNeighboursIDs = getNeighboursToSendAgent(*agent);
            addSubOverlapNeighboursToList(subOverlapNeighboursIDs, originalSubOverlapAreaID);
            for (std::list<int>::const_iterator itNeighbourNodeID = subOverlapNeighboursIDs.begin(); itNeighbourNodeID != subOverlapNeighboursIDs.end(); ++itNeighbourNodeID)
            {
                int neighbourNodeID = *itNeighbourNodeID;
                agentsByNode.at(neighbourNodeID).push_back(agent);
            }
        }

        sendGhostAgentsInMap(agentsByNode);
    }

    void OpenMPIMultiNode::sendRastersToNeighbours()
    {

    }

    void OpenMPIMultiNode::receiveRasters()
    {

    }

    void OpenMPIMultiNode::waitForMessagesToFinishAndClearRequests()
    {
        // free send requests
        // free receive requests

        // free(agentPackage); : packages should be in the MPIRequestStruct








        MPI_Barrier(MPI_COMM_WORLD);
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

    void OpenMPIMultiNode::executeAgents()
    {
        _executedAgentsInStep.clear();
usleep(getId() * 1000000);

_schedulerLogs->writeInDebugFile(CreateStringStream("AGENTS AT STEP " << _world->getCurrentStep() << "; ORIGINAL AGENTS:").str(), *this);
_schedulerLogs->printNodeAgentsInDebugFile(*this, true);

if (_world->getCurrentStep() == 1) exit(0);
        AgentsVector executedAgentsInArea;
        executeAgentsInArea(_nodeSpace.ownedAreaWithoutInnerOverlap, executedAgentsInArea);
        synchronizeAgentsIfNecessary(executedAgentsInArea);
        MPI_Barrier(MPI_COMM_WORLD);

_schedulerLogs->writeInDebugFile(CreateStringStream("AGENTS AT STEP " << _world->getCurrentStep() << "; INNER_MOST EXECUTED:").str(), *this);
_schedulerLogs->printNodeAgentsInDebugFile(*this, true);

std::cout << CreateStringStream("\n").str();
usleep(1000000);

        for (std::map<int, Rectangle<int>>::const_iterator it = _nodeSpace.innerSubOverlaps.begin(); it != _nodeSpace.innerSubOverlaps.end(); ++it)
        {
            int originalSubOverlapAreaID = it->first;
            Rectangle<int> originalOverlapArea = it->second;

usleep(getId() * 1000000);

            executedAgentsInArea.clear();
std::cout << CreateStringStream("[Process # " << getId() <<  "] " << getWallTime() << " executing agents in suboverlap #" << originalSubOverlapAreaID << "\n").str();
            executeAgentsInArea(originalOverlapArea, executedAgentsInArea);

std::cout << CreateStringStream("[Process # " << getId() <<  "] " << getWallTime() << " agents executed -> synchronizing\n").str();
            sendGhostAgentsToNeighbours(executedAgentsInArea, originalSubOverlapAreaID);
            receiveGhostAgentsFromNeighbouringNodes();

_schedulerLogs->writeInDebugFile(CreateStringStream("AGENTS AT STEP " << _world->getCurrentStep() << "; AFTER OVERLAP: " << originalSubOverlapAreaID).str(), *this);
_schedulerLogs->printNodeAgentsInDebugFile(*this, true);

std::cout << CreateStringStream("[Process # " << getId() <<  "] " << getWallTime() << " agents synchronized\n").str();
std::cout << CreateStringStream("[Process # " << getId() <<  "]\n").str();

// std::cout << CreateStringStream("[Process # " << getId() <<  "]\n").str();
        //     sendRastersToNeighbours();
        //     MPI_Barrier(MPI_COMM_WORLD);  // Needed if synchronized?¿
        //     receiveRasters();

        //     waitForMessagesToFinishAndClearRequests();
        MPI_Barrier(MPI_COMM_WORLD);

std::cout << CreateStringStream("\n").str();
usleep(1000000);
        }
    }
    
    void OpenMPIMultiNode::finish() 
    {
        MpiFactory::instance()->cleanTypes();
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
        std::random_shuffle(agentsVector.begin(), agentsVector.end());
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