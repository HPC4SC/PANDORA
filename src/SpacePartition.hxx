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

#ifndef __SpacePartition_hxx__
#define __SpacePartition_hxx__

#include <mpi.h>
#include <World.hxx>
#include <typedefs.hxx>
#include <Serializer.hxx>
#include <list>
#include <vector>
#include <Scheduler.hxx>

namespace Engine
{
    class Agent;

    /**
     * @brief struct containing raster information sent between nodes
     * 
     */
    struct MpiOverlap
    {
        // TODO move from string to index
        std::string _rasterName;
        std::vector<int> _data;
        Rectangle<int> _overlap;
        MPI_Request _request;
    };

    /** SpacePartition is a execution scheduler
      * It distributes a Pandora execution in different nodes using spatial partition
      * Each node contains the same amount of space and the agents inside
      * It is efficient for models where a homogeneous density of agents is expected around the whole world
      */
    class SpacePartition : public Scheduler 
    {

    protected:
        Serializer _serializer; //! Serializer instance.

        Point2D<int> _localRasterSize; //! Raster size of the current world.

        std::map<std::string, std::weak_ptr<Agent> > _executedAgentsHash; //! Map of already executed agents
        
        Point2D<int> _worldPos; //! Position of World inside global limits

        std::list<MpiOverlap*> _sendRequests; //! MPI request sended.
        std::list<MpiOverlap*> _receiveRequests; //! MPI request recieved.

        std::vector<int> _neighbors; //! Id's of neighboring computer nodes.
        Rectangle<int> _ownedArea; //! Area inside boundaries owned by the computer node without overlap.
        std::vector<Rectangle<int> > _sections; //! The four sections into a world is divided.
        AgentsList _overlapAgents; //! List of agents owned by other nodes in overlapping positions.

        bool _finalize; //! If true will call MPI_Finalize at the end of run ( default behavior ).

        AgentsList _removedAgents; //! This list has the agents that need to be removed at the end of step.
        
        double _initialTime; //! Initial simulation time.

        /**
         * @brief This method checks whether all the requests in the pool created by MPI_Isend and MPI_Irecv are finished before continuing.
         * 
         * @param updateMaxValues True updates the max values. Else don't update max values.
         */
        void clearRequests( bool updateMaxValues );

        /**
         * @brief Method to send a list of agents to their respective future world.
         * 
         * @param agentsToSend List of agents that are going to change its world.
         */
        void sendAgents( AgentsList & agentsToSend );
         
        /**
         * @brief Method to send overlap zones in the section we have executed.
         * 
         * @param sectionIndex Index representing wich section it is.
         * @param entireOverlap If true, the node will send its owned zone in overlap as well as adjacents overlapped zones, else it won't send the adjecent ones.
         */
        void sendOverlapZones( const int & sectionIndex, const bool & entireOverlap = true );

        /**
         * @brief Method to send the maximum values in the overlap zones in the section we have executed.
         * 
         */
        void sendMaxOverlapZones( );
         
        /**
         * @brief Method to copy the agents that levae the ovperlap zone to the neighbours cores.
         * 
         * @param sectionIndex Index representing the section.
         */
        void sendGhostAgents( const int & sectionIndex );

        //void updateOverlapAgent( Agent * agent );

        /**
         * @brief Add the agent to overlap agents list, and remove previous instances if they exist
         * 
         * @param sectionIndex Index representing the section.
         */
        void receiveGhostAgents( const int & sectionIndex );
        
        /**
         * @brief  Method to receive agents.
         * 
         * @param sectionIndex Index representing the section.
         */
        void receiveAgents( const int & sectionIndex );
        
        /**
         * @brief Method to receive overlap zones from neighbors that have executed adjacent sections.
         * 
         * @param sectionIndex Index representing wich section it is.
         * @param entireOverlap If true, the node will recieve its owned zone in overlap as well as adjacents overlapped zones, else it won't recieve the adjecent ones.
         */
        void receiveOverlapData( const int & sectionIndex, const bool & entireOverlap = true );

        /**
         * @brief Method to recieve the maximum values in the overlap zones in the section we have executed.
         * 
         */
        void receiveMaxOverlapData( );
 
        /**
         * @brief This method returns true if neighbor is corner of _id.
         * 
         * @param neighbor Id of the neighbour.
         * @return true 
         * @return false 
         */
        bool isCorner( const int & neighbor ) const;

        /**
         * @brief This method returns the general overlap zone between both worlds.
         * 
         * @param id Represents the neihgbour world.
         * @param sectionIndex Section beeing executed.
         * @return Rectangle<int> 
         */
        Rectangle<int> getOverlap( const int & id, const int & sectionIndex ) const;
        
        /**
         * @brief This method returns the external part of the strict overlap between World and id.
         * 
         * @param id Id of the world.
         * @return Rectangle<int> 
         */
        Rectangle<int> getExternalOverlap( const int & id ) const;
        
        /**
         * @brief This method returns the internal part of the strict overlap between World and id.
         * 
         * @param id Represents the neighbour world.
         * @return Rectangle<int> 
         */
        Rectangle<int> getInternalOverlap( const int & id ) const;
        
        /**
         * @brief Returns true if neighbor id must be updated this section index execution.
         * 
         * @param id Id of the neighbour.
         * @param sectionIndex Section beeing executed.
         * @return bool
         */
        bool needsToBeUpdated( const int & id, const int & sectionIndex );

        /**
         * @brief Returns true if neighbor id will send data to _id, according to index execution.
         * 
         * @param id Id of the neighbour
         * @param sectionIndex Section being executed
         * @return bool
         */
        bool needsToReceiveData( const int & id, const int & sectionIndex );

        int _overlap; //! Amount of width around one boundary considering the side of the World object that owns _overlap.

        /**
         * @brief Check correct overlap/size relation.
         * 
         */
        void checkOverlapSize( );

        /**
         * @brief Compute _boundaries based on Size, number of nodes and _overlap.
         * 
         */
        void stablishBoundaries( );
        
        /**
         * @brief Define original position of world, given overlap, size and id.
         * 
         */
        void stablishWorldPosition( );

        /**
         * @brief Applies next simulation step on the Section of the space identified by parameter 'sectionIndex'.
         * 
         * @param sectionIndex Identifier of the section.
         */
        void stepSection( const int & sectionIndex );

        /**
         * @brief Returns the id of the section that contains the point 'position'.
         * 
         * @param position Position of the world.
         * @return int 
         */
        int getIdFromPosition( const Point2D<int> & position );
        
        /**
         * @brief Given the id of a section returns that section position.
         * 
         * @param id Identifier of the world.
         * @return Point2D<int> 
         */
        Point2D<int> getPositionFromId( const int & id ) const;
        
        /**
         * @brief Given the id of a neighbour world section, returns its index, the position in the vector _neighbors.
         * 
         * @param id Identifier of the world.
         * @return int 
         */
        int getNeighborIndex( const int & id );

        /**
         * @brief This method returns true if the agent is already in executedAgents list.
         * 
         * @param id Identifier representing the Agent.
         * @return true 
         * @return false 
         */
        bool hasBeenExecuted( const std::string & id ) const;
        
        /**
         * @brief Return an agent, if it is in the list of ghosts.
         * 
         * @param id Identifier of the agent.
         * @return AgentsList::iterator 
         */
        AgentsList::iterator getGhostAgent( const std::string & id );
        
        /**
         * @brief Teturn an agent, if it is in the list of owned
         * 
         * @param id Identifier of the Agent.
         * @return AgentsList::iterator 
         */
        AgentsList::iterator getOwnedAgent( const std::string & id );

        /**
         * @brief True if the agent is in the list of agents to remove.
         * 
         * @param id Identifier of the checked Agent.
         * @return true 
         * @return false 
         */
        bool willBeRemoved( const std::string & id );
        
        /**
         * @brief Send overlapping data to neighbours before run.
         * 
         */
        void initOverlappingData( );

        /**
         * @brief Get the _ownedArea attribute.
         * 
         * @return const Rectangle<int>& 
         */
        const Rectangle<int> & getOwnedArea( ) const;
        
        /**
         * @brief Get the _overlap attribute.
         * 
         * @return const int& 
         */
        const int & getOverlap( ) const;
        
        /**
         * @brief transform from global coordinates to real coordinates ( in terms of world position ).
         * 
         * @param globalPosition Position in the general World.
         * @return Point2D<int> 
         */
        Point2D<int> getRealPosition( const Point2D<int> & globalPosition ) const;

    public:
        /**
         * @brief Construct a new SpacePartition instance.
         * 
         * @param overlap Overlap distance.
         * @param finalize If true the simulation will finalize if a MPI fault occurs, otherwise the simulation will continue.
         */
        SpacePartition( const int & overlap, bool finalize );
        
        virtual ~SpacePartition( );

        /**
         * @brief procedures that need to be executed after simulation ( i.e. finish communications for parallel nodes )
         * 
         */
        void finish( );

        /**
         * @brief Get the Boundaries object 
         * 
         * @return const Rectangle<int>& 
         */
        const Rectangle<int> & getBoundaries( ) const;        
        
        /**
         * @brief initializes everything needed before creation of agents and rasters ( i.e. sizes ).
         *          initialization of the object World for the simulation. Required to be called before calling run. 
         * 
         * @param argc 
         * @param argv 
         */
        void init( int argc, char *argv[] );
        
        /**
         * @brief initialize data processes after creation of agents and rasters.
         * 
         */
        void initData( );
        
        /**
         * @brief NOT IMPLEMENTED.
         * 
         */
        void updateEnvironmentState() override {}

        /**
         * @brief responsible for executing the agents and update world.
         * 
         */
        void executeAgents( );

        /**
         * @brief Adds an agent to the _executedAgentsHash.
         * 
         * @param agent Agent to be added.
         * @param executedAgent If true the agent has been executed, otherwise th agent has not been executed.
         */
        void agentAdded( AgentPtr agent, bool executedAgent );

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
         * @brief This method will return an agent, both looking at owned and ghost agents.
         * 
         * @param id Identifier of the agent.
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
         * @brief MPI version of wall time.
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
        size_t getNumberOfTypedAgents( const std::string & type ) const;

        /**
         * @brief Calls the serializer to add an integer attribute of an agent.
         * 
         * @param type Type of int value.
         * @param key Name of the attribute.
         * @param value Value of the attribute.
         */
        void addIntAttribute( const std::string & type, const std::string & key, int value );

        /**
         * @brief Calls the serializer to add a string attribute of an agent.
         * 
         * @param type Type of string value.
         * @param key Name of the attribute.
         * @param value Value of the attribute.
         */
        void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );

        /**
         * @brief Calls the serializer to add a float of an agent.
         * 
         * @param type Type of float value.
         * @param key Name of the attribute.
         * @param value Value of the attribute.
         */
        void addFloatAttribute( const std::string & type, const std::string & key, float value );
        
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
         * @param radius Radius of the circle to check.
         * @param type Type of the agents to check.
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

        friend class Serializer;

        /**
         * @brief Get the number of the agents on the overlap zones.
         * 
         * @return size_t 
         */
        size_t getNumberOfOverlapAgents( ) const { return _overlapAgents.size( ); }

        /**
         * @brief Returns a const_iterator pointing to the first position of the _overlapAgents list.
         * 
         * @return AgentsList::const_iterator 
         */
        AgentsList::const_iterator beginOverlapAgents( ) const{ return _overlapAgents.begin( ); }

        /**
         * @brief Returns a const_iterator pointing to the last position of the _overlapAgents list.
         * 
         * @return AgentsList::const_iterator 
         */
        AgentsList::const_iterator endOverlapAgents( ) const{ return _overlapAgents.end( ); }

    };
} // namespace Engine
#endif // __SpacePartition_hxx__
