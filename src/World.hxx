
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

#ifndef __World_hxx__
#define __World_hxx__

#include <map>
#include <vector>
#include <list>
#include <typedefs.hxx>
#include <DynamicRaster.hxx>
#include <StaticRaster.hxx>
#include <Rectangle.hxx>
#include <Point2D.hxx>
#include <algorithm>
#include <Config.hxx>
#include <memory>

namespace Engine
{
class Scheduler;
class Agent;
class OpenMPSingleNode;

class World
{
public:
    typedef std::map< std::string, int> RasterNameMap;
protected:
    std::shared_ptr<Config> _config; //! Pointer to the configuration of the world.
    AgentsList _agents; //! Global list of agents.

    bool _allowMultipleAgentsPerCell; //! False if each cell can have just one agent.

    int _step; //! Current simulation step.

    Scheduler * _scheduler; //! Pointer to the scheduler of the world.

    std::map<std::string, int> _rasterNames; //! <rasterName, rasterIndex in _rasters>.
    std::map<int, std::string> _rasterIDsToNames; //! <rasterIndex, rasterName>
    std::vector<StaticRaster*> _rasters; //! Rasters of the simulations.
    std::vector<bool> _dynamicRasters; //! True if the raster is dynamic, false the raster is static.
    std::vector<bool> _serializeRasters; //! True if the raster must be serialized, false otherwise.

    /**
     * @brief Stub method for grow resource to max of initialrasters, used by children of world at init time.
     * 
     * @param key Name of the raster.
     */
    void updateRasterToMaxValues( const std::string & key );

    /**
     * @brief Stub method for grow resource to max of initialrasters, used by children of world at init time.
     * 
     * @param index Index of the raster in the _rasters vector.
     */
    void updateRasterToMaxValues( const int & index );

    /**
     * @brief Updates the _currentStepOriginalPosition member for all the current agents of *this World.
     * 
     */
    void updateDiscreteStateStructures() const;

    /**
     * @brief Runs all the needed internal modifications at the beginning of each step. Cannot be overriden by the simulation.
     * 
     */
    void engineStep();

    /**
     * @brief Dumps current state of the simulation into the log files. Then applies next simulation step. Can be overriden by the model.
     * 
     */
    virtual void step( );

public:
    
    /**
	 * @brief The World object is bounded to an instance of Config.
	 * 
	 * @param config Configuration input file for the simulation.
	 * @param scheduler Selected scheduler for the current simulation.
	 * @param allowMultipleAgentsPerCell Defines if more than one agent can occupy a cell of the World.
	 */
    World( Config * config, Scheduler * scheduler = 0, const bool & allowMultipleAgentsPerCell = true );

    /**
     * @brief Destroy the World object.
     * 
     */
    virtual ~World( );

    /**
     * @brief Initialized the selected scheduler, the rasters and the agents of the simulation.
     * 
     * @param argc Not used.
     * @param argv Not used.
     */
    void initialize( int argc = 0, char *argv[] = 0 );
    
    /**
     * @brief Set randomness for the std::random_shuffle method, which shuffles the agents execution in each step of the simulation.
     * 
     * @param seed uint64_t
     */
    void setRandomShuffleSeed(uint64_t seed);

    /**
     * @brief Runs the simulation. Performs each step and stores the states. Requires calling 'init' method a-priori.
     * 
     */
    void run( );

    /**
     * @brief Add an agent to the world, and remove it from overlap agents if exist.
     * 
     * @param agent Agent to be added.
     * @param executedAgent True, the Agent has been executed. False otherwise.
     */
    virtual void addAgent( Agent * agent, bool executedAgent = true );

    /**
     * @brief Returns the number of neighbours of agent 'target' within the radius 'radius' using Euclidean Distance.
     * 
     * @param target Reference agent.
     * @param radius Radius of the circle we want to check.
     * @param type Type of the agents to check.
     * @return int 
     */
    int countNeighbours( Agent * target, const double & radius, const std::string & type="all" );
    
    /**
     * @brief Returns a list with the neighbours of agent 'target' within the radius 'radius' using Euclidean Distance.
     * 
     * @param target Reference agent.
     * @param radius Radius of the circle we want to check.
     * @param type Type of the agents to check.
     * @return AgentsVector 
     */
    AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type="all" );
    
    /**
     * @brief Returns an integer identifying the current step where the simulation is. The identifiers denote an order from older to newer steps.
     * 
     * @return current step. 
     */
    int getCurrentStep( ) const;
   
    /**
     * @brief This method can be redefined by the children in order to modify the execution of each step on a given resource field.
     * 
     */
    virtual void stepEnvironment( );
    
    /**
     * @brief Dump the rasters through a serializer.
     * 
     */
    void serializeRasters( );
    
    /**
     * @brief Dump the static rasters through a serializer.
     * 
     */
    void serializeStaticRasters( );
    
    /**
     * @brief Dump the agents through a serializer.
     * 
     */
    void serializeAgents( );
    
    /**
     * @brief The override of this method allows to modify rasters between step executions.
     * 
     * @param index Index of the raster.
     */
    virtual void stepRaster( const int & index );

    /**
     * @brief Returns raster identified by parameter 'key'.
     * 
     * @param index Represents where the raster is located.
     * @return DynamicRaster& 
     */
    DynamicRaster & getDynamicRaster( const size_t & index );

    /**
     * @brief Returns raster identified by parameter 'key'.
     * 
     * @param index Represents where the raster is located.
     * @return DynamicRaster& 
     */
    const DynamicRaster & getDynamicRaster( const size_t & index ) const;

    /**
     * @brief Returns raster identified by parameter 'key'.
     * 
     * @param key Name identifying the raster.
     * @return DynamicRaster& 
     */
    DynamicRaster & getDynamicRaster( const std::string & key );

    /**
     * @brief Returns raster identified by parameter 'key'.
     * 
     * @param key Name identifying the raster.
     * @return DynamicRaster& 
     */
    const DynamicRaster & getDynamicRaster( const std::string & key ) const;

    /**
     * @brief Return the static raster in position 'index'.
     * 
     * @param index Position of the static raster.
     * @return StaticRaster&
     */
    StaticRaster & getStaticRaster( const size_t & index );

    /**
     * @brief Return the static raster with name 'key'.
     * 
     * @param key Name of the raster.
     * @return StaticRaster& 
     */
    StaticRaster & getStaticRaster( const std::string & key );

    /**
     * @brief Create a new static raster map with the stablished size and given key.
     * 
     * @param key Name of the raster.
     * @param serialize True the raster must be serialized, otherwise the raster must not be serialized.
     * @param index Layer of the raster.
     */
    void registerStaticRaster( const std::string & key, const bool & serialize, int index = -1 );
    
    /**
     * @brief Create a new dynamic raster map with the stablished size and given key.
     * 
     * @param key Name of the raster.
     * @param serialize True the raster must be serialized, otherwise the raster must not be serialized.
     * @param index Layer of the raster.
     */
    void registerDynamicRaster( const std::string & key, const bool & serialize, int index = -1 );
    
    /**
     * @brief Checks if position parameter 'newPosition' is free to occupy by an agent, 
     * 'newPosition' is inside of the world and the maximum of agent cell-occupancy is not exceeded.
     * 
     * @param newPosition Position to check.
     * @return true 
     * @return false 
     */
    bool checkPosition( const Point2D<int> & newPosition ) const;

    /**
     * @brief Gets the name of the raster from its ID (index).
     * 
     * @param id const int&
     * @return std::string 
     */
    std::string getRasterNameFromID(const int& id) const;

    /**
     * @brief Sets the value of raster "key" to value "value" in global position "position".
     * 
     * @param key Name of the raster.
     * @param position Position to be updated.
     * @param value New value.
     */
    void setValue( const std::string & key, const Point2D<int> & position, int value );
    
    /**
     * @brief Sets the value of raster "index" to value "value" in global position "position".
     * 
     * @param index Index of the raster in the _rasters vector.
     * @param position Position to be updated.
     * @param value New value.
     */
    void setValue( const int & index, const Point2D<int> & position, int value );
    
    /**
     * @brief Returns the value of raster "key" in global position "position".
     * 
     * @param key Name of the raster.
     * @param position Position we want to check.
     * @return int 
     */
    int getValue( const std::string & key, const Point2D<int> & position ) const;
    
    /**
     * @brief Returns the value of raster "index" in global position "position".
     * 
     * @param index Index of the raster in the _rasters vector.
     * @param position Position we want to check.
     * @return int 
     */
    int getValue( const int & index, const Point2D<int> & position ) const;

    /**
     * @brief Sets the maximum allowed value of raster "key" to value "value" in global position "position".
     * 
     * @param key Name of the raster.
     * @param position Position we want to update.
     * @param value New maximum value.
     */
    void setMaxValue( const std::string & key, const Point2D<int> & position, int value );
    
    /**
     * @brief Sets the maximum allowed value of raster "index" to value "value" in global position "position".
     * 
     * @param index Index of the raster in the _rasters vector.
     * @param position Position we want to update.
     * @param value New maximum value.
     */
    void setMaxValue( const int & index, const Point2D<int> & position, int value );

    /**
     * @brief Gets the maximum allowed value of raster "key" in global position "position".
     * 
     * @param key Name of the raster.
     * @param position Position we want to check.
     * @return int 
     */
    int getMaxValue( const std::string & key, const Point2D<int> & position ) const;
    
    /**
     * @brief Gets the maximum allowed value of raster "index" in global position "position".
     * 
     * @param index Index of the raster in the _rasters vector.
     * @param position Position we want to check.
     * @return int 
     */
    int getMaxValue( const int & index, const Point2D<int> & position ) const;

    /**
     * @brief Teturns the name of the raster in position 'index'.
     * 
     * @param index Index of the raster in the _rasters vector.
     * @return const std::string&.
     */
    const std::string & getRasterName( const int & index ) const;

    /**
     * @brief Factory method design pattern for creating concrete agents and rasters.
     * It is delegated to concrete Worlds. This method must be defined by children, 
     * it is the method where agents are created and addAgents must be called.
     * 
     */
    virtual void createAgents( )
    {
    }
   
    /**
     * @brief To be defined in the subclass. Create the raster maps used in the simulation.
     * 
     */
    virtual void createRasters( )
    {
    }
    
    /**
     * @brief Get the Config object.
     * 
     * @return const Config&
     */
    const Config & getConfig( ) const 
    {
        return *_config;
    }

    /**
     * @brief Get the value of the _step attribute.
     * 
     * @return int 
     */
    int getCurrentTimeStep( ) const 
    { 
        return _step;
    }

    /**
     * @brief Time from initialization step to the moment the method is executed.
     * 
     * @return double 
     */
    double getWallTime( ) const;
    
    /**
     * @brief Provides a random valid position inside boundaries.
     * 
     * @return Point2D<int> 
     */
    Point2D<int> getRandomPosition( );

    /** Get the boundaries of the world. For sequential executions it will be the boundaries of the entire simulation,
      * but if this is not the case it is the area owned by the instance plus the overlaps.
      */
    const Rectangle<int> & getBoundaries( ) const;

    /**
     * @brief Get the _agents member.
     * 
     * @return AgentsList* 
     */
    AgentsList getAgentsList()
    {
        return _agents;
    }

    /**
     * @brief Returns the iteratior pointing to the first Agent in the _agents vector.
     * 
     * @return AgentsList::iterator 
     */
    AgentsList::iterator beginAgents( ) 
    {
        return _agents.begin( );
    }
    
    /**
     * @brief Returns the iteratior pointing to the last Agent in the _agents vector.
     * 
     * @return AgentsList::iterator 
     */
    AgentsList::iterator endAgents( ) 
    {
        return _agents.end( );
    }
    
    /**
     * @brief Get the Number Of Rasters.
     * 
     * @return size_t
     */
    size_t getNumberOfRasters( ) const 
    {
        return _rasters.size( );
    }

    /**
     * @brief Gets the number of existing rasters.
     * 
     * @return size_t 
     */
    size_t getNumberOfExistingRasters() const
    {
        size_t result = 0;
        for (size_t i = 0; i < _rasters.size(); ++i)
        {
            if (rasterExists(i)) result += 1;
        }
        return result;
    }

    /**
     * @brief Get the Number Of Static Rasters.
     * 
     * @return size_t 
     */
    size_t getNumberOfStaticRasters() const
    {
        size_t result = 0;
        for (size_t i = 0; i < _rasters.size(); ++i)
        {
            if (rasterExists(i) and not isRasterDynamic(i)) result += 1;
        }
        return result;
    }

    /**
     * @brief Get the Number Of Dynamic Rasters.
     * 
     * @return size_t 
     */
    size_t getNumberOfDynamicRasters() const
    {
        size_t result = 0;
        for (size_t i = 0; i < _rasters.size(); ++i)
        {
            if (rasterExists(i) and isRasterDynamic(i)) result += 1;
        }
        return result;
    }

    /**
     * @brief Get the Number Of Agents.
     * 
     * @return size_t.
     */
    size_t getNumberOfAgents( ) const
    {
        return _agents.size( );
    }
    
    /**
     * @brief Get the number of agents of the specified type.
     * 
     * @param type Type of the agents we want to check.
     * @return size_t 
     */
    size_t getNumberOfTypedAgents( const std::string & type ) const;

    /**
     * @brief Erases Agent pointed by "it" form the _agents AgentList.
     * 
     * @param it pointed Agent to be removed.
     */
    void eraseAgent( AgentsList::iterator & it ) 
    { 
        _agents.erase( it ); 
    }

    /**
     * @brief Erases the Agent pointed by 'it' from the _agents member.
     * 
     * @param it AgentsList::const_iterator&
     */
    void eraseAgent(AgentsList::const_iterator& it) 
    { 
        _agents.erase(it); 
    }

    /**
     * @brief Removes the specified agent from the simulation.
     * 
     * @param agent Agent to be removed.
     */
    void removeAgent( Agent * agent );
    
    /**
     * @brief Removes the agent pointed to bu the given pointer.
     * 
     * @param agentPtr Pointe rto the agent to be removed.
     */
    void removeAgent( std::shared_ptr<Agent> agentPtr );

    /**
     * @brief Get the agent of the given id.
     * 
     * @param id Identifier of the agent.
     * @return Agent* 
     */
    Agent * getAgent( const std::string & id );

    /**
     * @brief Get the agents in the speccified position.
     * 
     * @param position Position we want to check.
     * @param type Type of the agents to check.
     * @return AgentsVector 
     */
    AgentsVector getAgent( const Point2D<int> & position, const std::string & type="all" );

    /**
     * @brief Adds a string attrbute of the agents.
     * 
     * @param type Type of the attribute.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );

    /**
     * @brief Adds a integer attrbute of the agents.
     * 
     * @param type Type of the attribute.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addIntAttribute( const std::string & type, const std::string & key, int value );

    /**
     * @brief Adds a float attrbute of the agents.
     * 
     * @param type Type of the attribute.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addFloatAttribute( const std::string & type, const std::string & key, float value );

    /**
     * @brief Factory method for distributed Scheduler based on uneven spatial distribution with dynamic load balancing implementation.
     * 
     * @return Scheduler* 
     */
    static Scheduler* useOpenMPIMultiNode();

    /**
	 * @brief Factory method for distributed Scheduler based on spatial distribution of a simulation.
	 * 
	 * @param overlap Number of depth of the overlap zone.
	 * @param finalize If true will call MPI_Finalize at the end of run ( default behavior ).
	 * @return Scheduler* 
	 */
    //static Scheduler * useSpacePartition( int overlap = 1, bool finalize = true );
    
    /**
	 * @brief Factory method for sequential Scheduler without any non-shared communication mechanism, apt for being executed in a single computer.
	 * 
	 * @return Scheduler* 
	 */
    static Scheduler * useOpenMPSingleNode( );

    /**
     * @brief Get the identifier of the world.
     * 
     * @return const int& 
     */
    const int & getId( ) const;

    /**
     * @brief Get the number of tasks executing the simulation.
     * 
     * @return const int& 
     */
    const int & getNumTasks( ) const;
    
    /**
     * @brief Returns if a raster in index 'index' must be serialized.
     * 
     * @param index Index of the raster.
     * @return true
     * @return false 
     */
    bool rasterToSerialize( size_t index ) { return _serializeRasters.at( index ); }
    
    /**
     * @brief Returns if the dynamic_raster in index 'index' exists.
     * 
     * @param index Index of the raster to checks.
     * @return true 
     * @return false 
     */
    bool isRasterDynamic( size_t index ) const { return _dynamicRasters.at( index ); }
    
    /**
     * @brief Returns if the raster in index 'index' exists.
     * 
     * @param index Index of the raster to check.
     * @return true 
     * @return false 
     */
    bool rasterExists( size_t index ) const
    {
        return bool(_rasters.at(index));
    }
    
    /**
     * @brief [OpenMP] Set whether the agents' actions should be run in parallel or not. If executeActionsInParallel = true, control locks are REQUIRED to be used in the model!
     * 
     * @param updateKnowledgeInParallel bool
     * @param executeActionsInParallel bool
     */
    void setParallelism(bool updateKnowledgeInParallel, bool executeActionsInParallel);

    /**
     * @brief [PARALLELISM FUNCTION] Indicates that the world is going to be changed, so it is necessary to pause the execution of all the threads but the one calling this function.
     * 
     */
    void changingWorld();

    /**
     * @brief [PARALLELISM FUNCTION] Indicates that the world has already been changed, so the paused threads execution can be resumed.
     * 
     */
    void worldChanged();

};

} // namespace Engine

#endif //__World_hxx__

