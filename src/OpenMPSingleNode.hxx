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
    boost::timer::cpu_timer _timer; //! Timer of the running simulation.
    SequentialSerializer _serializer; //! Instance of the simulation's SequentialSerializer.
    AgentsList _removedAgents; //! List of agents that are removed during a time step, and need to be erased by the end of the time step.

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
    OpenMPSingleNode();

    /**
     * @brief Destroy the OpenMPSingle Node object.
     * 
     */
    virtual ~OpenMPSingleNode( );
    
    /**
     * @brief Procedures that need to be executed after simulation ( i.e. stop the timer ).
     * 
     */
    void finish( );

    /**
     * @brief Initializes everything needed before creation of agents and rasters ( i.e. sizes ).
     * 
     * @param argc Not used.
     * @param argv Not used.
     */
    void init( int argc, char *argv[] );

    /**
     * @brief Initialize data processes after creation of agents and rasters.
     * 
     */
    void initData( );
    
    /**
     * @brief NOT IMPLEMENTED.
     * 
     */
    void updateEnvironmentState() override {}

    /**
     * @brief Responsible for executing the agents and update world.
     * 
     */
    virtual void executeAgents( );

    /**
     * @brief Removes the necessary agents form the simulation.
     * 
     */
    void removeAgents( );

    /**
     * @brief Pushes the agent to the _removedAgents.
     * 
     * @param agent Pointer to the agent to be removed.
     */
    void removeAgent( Agent * agent );

    /**
     * @brief This method will return an agent, looking at the list of existing agents.
     * 
     * @param id Identifier of the Agent.
     * @return Agent* 
     */
    Agent * getAgent( const std::string & id );

    /**
     * @brief Get the Agents of the Type "type" in position "position".
     * 
     * @param position Position to check.
     * @param type Type of the selected Agents.
     * @return AgentsVector 
     */
    AgentsVector getAgent( const Point2D<int> & position, const std::string & type="all" );

    /**
     * @brief Gets a random position within the World.
     * 
     * @return Point2D<int> 
     */
    Point2D<int> getRandomPosition( ) const;

    /**
     * @brief Get the WallTime object.
     * 
     * @return double 
     */
    double getWallTime( ) const;

    /**
     * @brief Get the number of agents of the specified type.
     * 
     * @param type Selected Agent type.
     * @return size_t 
     */
    size_t getNumberOfTypedAgents( const std::string & type ) const { return 0; }

    /**
     * @brief Calls the serializer to add an string attribute of an Agent.
     * 
     * @param type Type of int value.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );

    /**
     * @brief Calls the serializer to add a float attribute of an Agent.
     * 
     * @param type Type of float value.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addFloatAttribute( const std::string & type, const std::string & key, float value );

    /**
     * @brief Calls the serializer to add a integer attribute of an Agent.
     * 
     * @param type Type of int value.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addIntAttribute( const std::string & type, const std::string & key, int value );
    
    /**
     * @brief Serialize the agents with the information of the current step.
     * 
     * @param step Current simulation time.
     */
    void serializeAgents( const int & step );
    
    /**
     * @brief Serialize the rasters with the information of the current step.
     * 
     * @param step Current simulation time.
     */
    void serializeRasters( const int & step );

    /**
     * @brief Counts the neighbours of an agent of a concrete type  witith the radius.
     * 
     * @param target Agent in the center of the radius.
     * @param radius Radius of the circle to check.
     * @param type Type of the agents to check.
     * @return int 
     */
    int countNeighbours( Agent * target, const double & radius, const std::string & type );

    /**
     * @brief Gets the neighbours of an agent of a concrete type  witith the radius.
     * 
     * @param target Agent in the center of the radius.
     * @param radius Radius of the circle to get.
     * @param type Type of the agents to get.
     * @return AgentsVector 
     */
    AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type );

    /**
     * @brief Set the value of a concrete position.
     * 
     * @param raster Raster to update.
     * @param position Position to update.
     * @param value New value of the position.
     */
    void setValue( DynamicRaster & raster, const Point2D<int> & position, int value );

    /**
     * @brief Get the value of a concrete position.
     * 
     * @param raster Raster to check.
     * @param position Position to check.
     * @return int 
     */
    int getValue( const DynamicRaster & raster, const Point2D<int> & position ) const;

    /**
     * @brief Set the _maxValue of the position specified.
     * 
     * @param raster Raster to update.
     * @param position Position to change the _maxValue.
     * @param value New maxValue.
     */
    void setMaxValue( DynamicRaster & raster, const Point2D<int> & position, int value );

    /**
     * @brief Get the _maxValue of the specified position.
     * 
     * @param raster Raster to check.
     * @param position Position to check.
     * @return int 
     */
    int getMaxValue( const DynamicRaster & raster, const Point2D<int> & position ) const;

//    friend class Serializer;
};

} // namespace Engine

#endif // __OpenMPSingleNode_hxx__

