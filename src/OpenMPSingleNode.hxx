/*
 * Copyright ( c ) 2014
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

#ifndef __OpenMPSingleNode_hxx__
#define __OpenMPSingleNode_hxx__

#include <World.hxx>
#include <typedefs.hxx>
#include <SequentialSerializer.hxx>
#include <list>
#include <vector>
#include <Scheduler.hxx>
#include <boost/timer/timer.hpp>

namespace Engine
{
class Agent;

/** OpenMPSingleNode is a execution scheduler
  * It executes a Pandora run a single computer node using only shared-memory distribution ( OpenMP )
  * It is efficient for simple models, as several simulations can be executed in a single node
  */
class OpenMPSingleNode : public Scheduler
{
private:
    boost::timer::cpu_timer _timer;
    SequentialSerializer _serializer;

    // list of agents that are removed during a time step, and need to be erased by the end of the time step
    AgentsList _removedAgents;

    /**
     * @brief returns the iterator inside World::_agents with _id = id; in case it is not found returns _agents.end( ).
     * 
     * @param id id of the agent.
     * @return AgentsList::iterator 
     */
    AgentsList::iterator getAgentIterator( const std::string & id );
public:
    
    /**
     * @brief Construct a new OpenMPSingleNode object. And creates the corresponding serializer.
     * 
     */
    OpenMPSingleNode( );
    virtual ~OpenMPSingleNode( );
    
    /**
     * @brief procedures that need to be executed after simulation ( i.e. stop the timer )
     * 
     */
    void finish( );

    /**
     * @brief initializes everything needed before creation of agents and rasters ( i.e. sizes )
     * 
     * @param argc Not used
     * @param argv Not used
     */
    void init( int argc, char *argv[] );
    
    /**
     * @brief initialize data processes after creation of agents and rasters
     * 
     */
    void initData( );
    
    /**
     * @brief responsible for executing the agents and update world
     * 
     */
    virtual void executeAgents( );

    /**
     * @brief removes the necessary agents form the simulation.
     * 
     */
    void removeAgents( );
    void removeAgent( Agent * agent );

    //! this method will return an agent, looking at the list of existing agents
    Agent * getAgent( const std::string & id );
    AgentsVector getAgent( const Point2D<int> & position, const std::string & type="all" );

    Point2D<int> getRandomPosition( ) const;

    /**
     * @brief Get the WallTime object
     * 
     * @return double 
     */
    double getWallTime( ) const;
    size_t getNumberOfTypedAgents( const std::string & type ) const { return 0; }

    void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );
    void addFloatAttribute( const std::string & type, const std::string & key, float value );
    void addIntAttribute( const std::string & type, const std::string & key, int value );
    
    /**
     * @brief serialize the agents with the information of the current step
     * 
     * @param step current simulation time
     */
    void serializeAgents( const int & step );
    
    /**
     * @brief serialize the rasters with the information of the current step
     * 
     * @param step current simulation time
     */
    void serializeRasters( const int & step );
    int countNeighbours( Agent * target, const double & radius, const std::string & type );
    AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type );

    void setValue( DynamicRaster & raster, const Point2D<int> & position, int value );
    int getValue( const DynamicRaster & raster, const Point2D<int> & position ) const;
    void setMaxValue( DynamicRaster & raster, const Point2D<int> & position, int value );
    int getMaxValue( const DynamicRaster & raster, const Point2D<int> & position ) const;

//    friend class Serializer;
};

} // namespace Engine

#endif // __OpenMPSingleNode_hxx__

