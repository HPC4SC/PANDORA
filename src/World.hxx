
/*
 * Copyright ( c ) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
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
class SpacePartition;
class OpenMPSingleNode;

class World
{
public:
    typedef std::map< std::string, int> RasterNameMap;
protected:
    std::shared_ptr<Config> _config;
    //! global list of agents
    AgentsList _agents;

    //! false if each cell can have just one agent
    bool _allowMultipleAgentsPerCell;

    //! current simulation step
    int _step;

    Scheduler * _scheduler;

protected:
    // rasters that won't change values during the simulation
    std::map<std::string, int> _rasterNames;
    std::vector<StaticRaster * > _rasters;
    // true if the raster is dynamic
    std::vector<bool> _dynamicRasters;
    std::vector<bool> _serializeRasters;

    //! stub method for grow resource to max of initialrasters, used by children of world at init time
    void updateRasterToMaxValues( const std::string & key );
    void updateRasterToMaxValues( const int & index );


    /**
     * @brief dumps current state of the simulation. Then applies next simulation step.
     * 
     */
    virtual void step( );

public:
    
    /**
	 * @brief The World object is bounded to an instance of Config.
	 * 
	 * @param config configuration input file for the simulation.
	 * @param scheduler selected scheduler for the current simulation.
	 * @param allowMultipleAgentsPerCell defines if more than one agent can occupy a cell of the World.
	 */
    World( Config * config, Scheduler * scheduler = 0, const bool & allowMultipleAgentsPerCell = true );

    virtual ~World( );

    /**
     * @brief Initialized the selected scheduler, the rasters and the agents of the simulation.
     * 
     * @param argc Not used
     * @param argv Not used
     */
    void initialize( int argc = 0, char *argv[] = 0 );
    //! Runs the simulation. Performs each step and stores the states. Requires calling 'init' method a-priori.
    void run( );

    /**
     * @brief add an agent to the world, and remove it from overlap agents if exist
     * 
     * @param agent Agent to be added
     * @param executedAgent true, the Agent has been executed. False otherwise.
     */
    virtual void addAgent( Agent * agent, bool executedAgent = true );

    //! returns the number of neighbours of agent 'target' within the radius 'radius' using Euclidean Distance.
    int countNeighbours( Agent * target, const double & radius, const std::string & type="all" );
    //! returns a list with the neighbours of agent 'target' within the radius 'radius' using Euclidean Distance.
    AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type="all" );
    
    /**
     * @brief returns an integer identifying the current step where the simulation is. The identifiers denote an order from older to newer steps.
     * 
     * @return current step. 
     */
    int getCurrentStep( ) const;
   
    /**
     * @brief this method can be redefined by the children in order to modify the execution of each step on a given resource field. Default is grow 1 until max.
     * 
     */
    virtual void stepEnvironment( );
    //! dump the rasters through a serializer.
    void serializeRasters( );
    //! dump the static rasters through a serializer.
    void serializeStaticRasters( );
    //! dump the agents through a serializer.
    void serializeAgents( );
    
    /**
     * @brief the override of this method allows to modify rasters between step executions.
     * 
     * @param index index of the raster.
     */
    virtual void stepRaster( const int & index );

    /**
     * @brief returns raster identified by parameter 'key'.
     * 
     * @param index represents where the raster is located.
     * @return DynamicRaster& 
     */
    DynamicRaster & getDynamicRaster( const size_t & index );

    /**
     * @brief returns raster identified by parameter 'key'.
     * 
     * @param index represents where the raster is located.
     * @return DynamicRaster& 
     */
    const DynamicRaster & getDynamicRaster( const size_t & index ) const;

    /**
     * @brief returns raster identified by parameter 'key'.
     * 
     * @param key name identifying the raster.
     * @return DynamicRaster& 
     */
    DynamicRaster & getDynamicRaster( const std::string & key );

    /**
     * @brief returns raster identified by parameter 'key'.
     * 
     * @param key name identifying the raster.
     * @return DynamicRaster& 
     */
    const DynamicRaster & getDynamicRaster( const std::string & key ) const;

    /**
     * @brief return the static raster in position 'index'.
     * 
     * @param index position of the static raster.
     * @return StaticRaster&
     */
    StaticRaster & getStaticRaster( const size_t & index );
    StaticRaster & getStaticRaster( const std::string & key );

    //! create a new static raster map with the stablished size and given key
    void registerStaticRaster( const std::string & key, const bool & serialize, int index = -1 );
    //! create a new raster map with the stablished size and given key
    void registerDynamicRaster( const std::string & key, const bool & serialize, int index = -1 );
    //! checks if position parameter 'newPosition' is free to occupy by an agent, 'newPosition' is inside of the world and the maximum of agent cell-occupancy is not exceeded.
    bool checkPosition( const Point2D<int> & newPosition ) const;

    //! sets the value of raster "key" to value "value" in global position "position"
    void setValue( const std::string & key, const Point2D<int> & position, int value );
    //! sets the value of raster "index" to value "value" in global position "position"
    void setValue( const int & index, const Point2D<int> & position, int value );
    //! returns the value of raster "key" in global position "position"
    int getValue( const std::string & key, const Point2D<int> & position ) const;
    //! returns the value of raster "index" in global position "position"
    int getValue( const int & index, const Point2D<int> & position ) const;

    //! sets the maximum allowed value of raster "key" to value "value" in global position "position"
    void setMaxValue( const std::string & key, const Point2D<int> & position, int value );
    //! sets the maximum allowed value of raster "index" to value "value" in global position "position"
    void setMaxValue( const int & index, const Point2D<int> & position, int value );

    //! gets the maximum allowed value of raster "key" in global position "position"
    int getMaxValue( const std::string & key, const Point2D<int> & position ) const;
    //! gets the maximum allowed value of raster "index" in global position "position"
    int getMaxValue( const int & index, const Point2D<int> & position ) const;

    /**
     * @brief returns the name of the raster in position 'index'.
     * 
     * @param index the position of the reaster.
     * @return const std::string&.
     */
    const std::string & getRasterName( const int & index ) const;

    /**
     * @brief Factory method design pattern for creating concrete agents and rasters.
     * It is delegated to concrete Worlds. This method must be defined by children, 
     * it is the method where agents are created and addAgents must be called.
     * 
     */
    virtual void createAgents( ){ };
   
    /**
     * @brief to be defined in the subclass. Create the raster maps used in the simulation.
     * 
     */
    virtual void createRasters( ){ }
    
    /**
     * @brief Get the Config object.
     * 
     * @return const Config&
     */
    const Config & getConfig( ) const { return *_config; }

    int    getCurrentTimeStep( ) const { return _step; }
    //! time from initialization step to the moment the method is executed
    double getWallTime( ) const;
    //! provides a random valid position inside boundaries
    Point2D<int> getRandomPosition( );

    /** get the boundaries of the world. For sequential executions it will be the boundaries of the entire simulation,
      * but if this is not the case it is the area owned by the instance plus the overlaps
      */
    const Rectangle<int> & getBoundaries( ) const;

    /**
     * @brief returns the iteratior pointing to the first Agent in the _agents vector.
     * 
     * @return AgentsList::iterator 
     */
    AgentsList::iterator beginAgents( ) { return _agents.begin( ); }
    
    /**
     * @brief returns the iteratior pointing to the last Agent in the _agents vector.
     * 
     * @return AgentsList::iterator 
     */
    AgentsList::iterator endAgents( ) { return _agents.end( ); }
    
    /**
     * @brief Get the Number Of Rasters.
     * 
     * @return size_t
     */
    size_t getNumberOfRasters( ) const { return _rasters.size( ); }

    /**
     * @brief Get the Number Of Agents.
     * 
     * @return size_t.
     */
    size_t getNumberOfAgents( ) const { return _agents.size( ); }
    size_t getNumberOfTypedAgents( const std::string & type ) const;

    /**
     * @brief erases Agent pointed by "it" form the _agents AgentList.
     * 
     * @param it pointed Agent to be removed.
     */
    void eraseAgent( AgentsList::iterator & it ) { _agents.erase( it ); }
    void removeAgent( Agent * agent );
    void removeAgent( std::shared_ptr<Agent> agentPtr );
    Agent * getAgent( const std::string & id );
    AgentsVector getAgent( const Point2D<int> & position, const std::string & type="all" );
    void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );
    void addIntAttribute( const std::string & type, const std::string & key, int value );
    void addFloatAttribute( const std::string & type, const std::string & key, float value );
    const int & getId( ) const;
    const int & getNumTasks( ) const;
    
    /**
     * @brief returns if a raster in index 'index' must be serialized.
     * 
     * @param index index of the raster.
     * @return true
     * @return false 
     */
    bool rasterToSerialize( size_t index ) { return _serializeRasters.at( index ); }
    
    /**
     * @brief returns if the dynamic_raster in index 'index' exists.
     * 
     * @param index index of the raster to checks.
     * @return true 
     * @return false 
     */
    bool isRasterDynamic( size_t index ) { return _dynamicRasters.at( index ); }
    
    /**
     * @brief returns if the raster in index 'index' exists.
     * 
     * @param index index of the raster to check.
     * @return true 
     * @return false 
     */
    bool rasterExists( size_t index )
    {
        /*if ( _rasters.at( index ))
        {
            return true;
        }
        return false;*/
        return bool(_rasters.at( index ));
    }

    /**
	 * @brief factory method for distributed Scheduler based on spatial distribution of a simulation.
	 * 
	 * @param overlap number of depth of the overlap zone.
	 * @param finalize if true will call MPI_Finalize at the end of run ( default behavior ).
	 * @return Scheduler* 
	 */
    static Scheduler * useSpacePartition( int overlap = 1, bool finalize = true );
    
    /**
	 * @brief factory method for sequential Scheduler without any non-shared communication mechanism, apt for being executed in a single computer.
	 * 
	 * @return Scheduler* 
	 */
    static Scheduler * useOpenMPSingleNode( );
};

} // namespace Engine

#endif //__World_hxx__

