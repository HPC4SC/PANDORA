#include <DifferentSpaces.hxx>
#include <Agent.hxx>
#include <Config.hxx>
#include <Exception.hxx>
#include <boost/chrono.hpp>

namespace Engine {

    DifferentSpaces::DifferentSpaces() : _serializer(*this) {

    }

    DifferentSpaces::~DifferentSpaces() {}

    void DifferentSpaces::init() {
        stablishBoundaries();
    }

    void DifferentSpacesinitData() {

    }

    void DifferentSpaces::executeAgents() {
        AgentsVector agentsToExecute;

        for (AgentsList::iterator it = _world->beginAgents(); it != _world->endAgents(); ++it) {
            agentsToExecute.push_back(*it);
        }

        std::random_shuffle(agentsToExecute.begin(), agentsToExecute.end());

#ifndef PANDORAEDEBUG
        // shared memory distibution for read-only planning actions, disabled for extreme debug
#pragma omp parallel for schedule(guided)
#endif

        for (size_t i = 0; i < agentsToExecute.size(); i++) {
            Agent *agent = agentsToExecute[i].get();
            agent->updateKnowledge();
            agent->selectActions();
        }

        for (size_t i = 0; i < agentsToExecute.size(); i++) {
            Agent *agent = agentsToExecute[i].get();
            agent->executeActions();
            agent->updateState();
        }
    }

    void DifferentSpaces::stablishBoundaries() {
        // position of world related to the complete set of computer nodes
        int worldsPerRow = sqrt(_numTasks);
        _worldPos = getPositionFromId(_id);

        for (int x = _worldPos._x - 1; x <= _worldPos._x + 1; x++) {
            for (int y = _worldPos._y - 1; y <= _worldPos._y + 1; y++) {
                if (x > -1 && x < worldsPerRow && y > -1 && y < worldsPerRow) {
                    if (x != _worldPos._x || y != _worldPos._y) {
                        _neighbors.push_back(y * worldsPerRow + x);
                    }
                }
            }
        }
        // owned area inside global coordinates, depending on worldPos
        _ownedArea._size._width = _world->getConfig().getSize()._width / sqrt(_numTasks);
        _ownedArea._size._height = _world->getConfig().getSize()._height / sqrt(_numTasks);
        _ownedArea._origin._x = _worldPos._x * _ownedArea._size._width;
        _ownedArea._origin._y = _worldPos._y * _ownedArea._size._height;

        // defining overlap boundaries
        _boundaries = _ownedArea;
        // west boundary
        if (_ownedArea._origin._x != 0) {
            _boundaries._origin._x -= _overlap;
            _boundaries._size._width += _overlap;
        }
        // east boundary
        if (_ownedArea._origin._x != _world->getConfig().getSize()._width - _ownedArea._size._width) {
            _boundaries._size._width += _overlap;
        }
        // north boundary
        if (_ownedArea._origin._y != 0) {
            _boundaries._origin._y -= _overlap;
            _boundaries._size._height += _overlap;
        }
        // south boundary
        if (_ownedArea._origin._y != _world->getConfig().getSize()._height - _ownedArea._size._height) {
            _boundaries._size._height += _overlap;
        }
        // creating sections
        _spaces.resize(4);
        _spaces[0] = Rectangle<int>(_ownedArea._size / 2, _ownedArea._origin);
        _spaces[1] = Rectangle<int>(_ownedArea._size / 2,
                                    Point2D<int>(_ownedArea._origin._x + _ownedArea._size._width / 2,
                                                 _ownedArea._origin._y));
        _spaces[2] = Rectangle<int>(_ownedArea._size / 2, Point2D<int>(_ownedArea._origin._x, _ownedArea._origin._y +
                                                                                              _ownedArea._size._height /
                                                                                              2));
        _spaces[3] = Rectangle<int>(_ownedArea._size / 2,
                                    Point2D<int>(_ownedArea._origin._x + _ownedArea._size._width / 2,
                                                 _ownedArea._origin._y + _ownedArea._size._height / 2));

        if (_ownedArea._size._width % 2 != 0 || _ownedArea._size._height % 2 != 0) {
            std::stringstream oss;
            oss << "DifferentSpaces::init - local raster size: " << _ownedArea._size << " must be divisible by 2";
            throw Exception(oss.str());
        }
        checkOverlapSize();

        std::stringstream logName;
        logName << "simulation_" << _id;
        log_INFO(logName.str(),
                 getWallTime() << " pos: " << _worldPos << ", global size: " << _world->getConfig().getSize()
                               << ", boundaries: " << _boundaries << " and owned area: " << _ownedArea);
        log_INFO(logName.str(),
                 getWallTime() << " sections 0: " << _sections[0] << " - 1: " << _sections[1] << " - 2:" << _sections[2]
                               << " - 3: " << _sections[3]);
    }

    void DifferentSpaces::removeAgents() {

    }

    void DifferentSpaces::removeAgent(Agent *agent) {

    }

    Agent *getAgent(const std::string &id) {

    }

    AgentsVector getAgent(const Point2D<int> &position, const std::string &type = "all") {

    }

    void defineBoundaries() {

    }

    double getWallTime() const {

    }

    size_t getNumberOfTypedAgents(const std::string &type) const {

    }

} //namespace Engine