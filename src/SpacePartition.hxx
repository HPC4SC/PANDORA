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
        Serializer _serializer;

        Point2D<int> _localRasterSize;

        //! map of already executed agents
        std::map<std::string, std::weak_ptr<Agent> > _executedAgentsHash;

        //! position of World inside global limits
        Point2D<int> _worldPos;

        std::list<MpiOverlap*> _sendRequests;
        std::list<MpiOverlap*> _receiveRequests;

        /**
         * @brief this method checks whether all the requests in the pool created by MPI_Isend and MPI_Irecv are finished before continuing.
         * 
         * @param updateMaxValues true updates the max values. Else don't update max values.
         */
        void clearRequests( bool updateMaxValues );

        /**
         * @brief method to send a list of agents to their respective future world.
         * 
         * @param agentsToSend list of agents that are going to change its world.
         */
        void sendAgents( AgentsList & agentsToSend );
         
        /**
         * @brief Method to send overlap zones in the section we have executed
         * 
         * @param sectionIndex index representing wich section it is.
         * @param entireOverlap if true, the node will send its owned zone in overlap as well as adjacents overlapped zones, else it won't send the adjecent ones.
         */
        void sendOverlapZones( const int & sectionIndex, const bool & entireOverlap = true );
        void sendMaxOverlapZones( );
         
        /**
         * @brief method to copy the agents that levae the ovperlap zone to the neighbours cores.
         * 
         * @param sectionIndex index representing the section.
         */
        void sendGhostAgents( const int & sectionIndex );

        //void updateOverlapAgent( Agent * agent );
        /**
         * @brief add the agent to overlap agents list, and remove previous instances if they exist
         * 
         * @param sectionIndex index representing the section.
         */
        void receiveGhostAgents( const int & sectionIndex );
        
        /**
         * @brief  method to receive agents.
         * 
         * @param sectionIndex index representing the section.
         */
        void receiveAgents( const int & sectionIndex );
        
        /**
         * @brief method to receive overlap zones from neighbors that have executed adjacent sections.
         * 
         * @param sectionIndex index representing wich section it is.
         * @param entireOverlap if true, the node will recieve its owned zone in overlap as well as adjacents overlapped zones, else it won't recieve the adjecent ones.
         */
        void receiveOverlapData( const int & sectionIndex, const bool & entireOverlap = true );
        void receiveMaxOverlapData( );

        /**
         * @brief id's of neighboring computer nodes.
         * 
         */
        std::vector<int> _neighbors;
        
        /**
         * @brief area inside boundaries owned by the computer node without overlap.
         * 
         */
        Rectangle<int> _ownedArea;
        
        /**
         * @brief the four sections into a world is divided.
         * 
         */
        std::vector<Rectangle<int> > _sections;

        /**
         * @brief list of agents owned by other nodes in overlapping positions
         * 
         */
        AgentsList _overlapAgents;

        //! this method returns true if neighbor is corner of _id
        bool isCorner( const int & neighbor ) const;

        /**
         * @brief this method returns the general overlap zone between both worlds.
         * 
         * @param id represents the neihgbour world.
         * @param sectionIndex section beeing executed.
         * @return Rectangle<int> 
         */
        Rectangle<int> getOverlap( const int & id, const int & sectionIndex ) const;
        
        /**
         * @brief this method returns the external part of the strict overlap between World and id.
         * 
         * @param id id of the world.
         * @return Rectangle<int> 
         */
        Rectangle<int> getExternalOverlap( const int & id ) const;
        
        /**
         * @brief this method returns the internal part of the strict overlap between World and id.
         * 
         * @param id represents the neighbour world.
         * @return Rectangle<int> 
         */
        Rectangle<int> getInternalOverlap( const int & id ) const;
        
        /**
         * @brief returns true if neighbor id must be updated this section index execution.
         * 
         * @param id id of the neighbour.
         * @param sectionIndex section beeing executed.
         * @return bool
         */
        bool needsToBeUpdated( const int & id, const int & sectionIndex );

        /**
         * @brief returns true if neighbor id will send data to _id, according to index execution.
         * 
         * @param id id of the neighbour
         * @param sectionIndex section being executed
         * @return bool
         */
        bool needsToReceiveData( const int & id, const int & sectionIndex );

        /**
         * @brief amount of width around one boundary considering the side of the World object that owns _overlap.
         * 
         */
        int _overlap;

        //! check correct overlap/size relation
        void checkOverlapSize( );

        /**
         * @brief compute _boundaries based on Size, number of nodes and _overlap.
         * 
         */
        void stablishBoundaries( );
        
        /**
         * @brief define original position of world, given overlap, size and id.
         * 
         */
        void stablishWorldPosition( );

        /**
         * @brief applies next simulation step on the Section of the space identified by parameter 'sectionIndex'.
         * 
         * @param sectionIndex identifier of the section.
         */
        void stepSection( const int & sectionIndex );

        /**
         * @brief returns the id of the section that contains the point 'position'.
         * 
         * @param position 
         * @return int 
         */
        int getIdFromPosition( const Point2D<int> & position );
        //! given the id of a section returns that section position
        Point2D<int> getPositionFromId( const int & id ) const;
        
        /**
         * @brief given the id of a neighbour world section, returns its index, the position in the vector _neighbors.
         * 
         * @param id id of the world.
         * @return int 
         */
        int getNeighborIndex( const int & id );

        /**
         * @brief if true will call MPI_Finalize at the end of run ( default behavior ).
         * 
         */
        bool _finalize;

        /**
         * @brief this method returns true if the agent is already in executedAgents list.
         * 
         * @param id id representing the Agent.
         * @return true 
         * @return false 
         */
        bool hasBeenExecuted( const std::string & id ) const;
        //! return an agent, if it is in the list of ghosts
        AgentsList::iterator getGhostAgent( const std::string & id );
        
        /**
         * @brief this list has the agents that need to be removed at the end of step.
         * 
         */
        AgentsList _removedAgents;
        
        /**
         * @brief return an agent, if it is in the list of owned
         * 
         * @param id id of the Agent.
         * @return AgentsList::iterator 
         */
        AgentsList::iterator getOwnedAgent( const std::string & id );

        /**
         * @brief true if the agent is in the list of agents to remove.
         * 
         * @param id id of the checked Agent.
         * @return true 
         * @return false 
         */
        bool willBeRemoved( const std::string & id );
        double _initialTime;
        //! send overlapping data to neighbours before run
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
         * @param globalPosition 
         * @return Point2D<int> 
         */
        Point2D<int> getRealPosition( const Point2D<int> & globalPosition ) const;

    public:
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
         * @brief responsible for executing the agents and update world.
         * 
         */
        void executeAgents( );

        void agentAdded( AgentPtr agent, bool executedAgent );
        void removeAgents( );
        void removeAgent( Agent * agent );

        //! this method will return an agent, both looking at owned and ghost agents
        Agent * getAgent( const std::string & id );
        AgentsVector getAgent( const Point2D<int> & position, const std::string & type="all" );

        Point2D<int> getRandomPosition( ) const;

        /**
         * @brief MPI version of wall time
         * 
         * @return double 
         */
        double getWallTime( ) const;
        size_t getNumberOfTypedAgents( const std::string & type ) const;

        /**
         * @brief calls the serializer to add an integer attribute of an Agent.
         * 
         * @param type type of int value.
         * @param key name of the attribute.
         * @param value value of the attribute.
         */
        void addIntAttribute( const std::string & type, const std::string & key, int value );

        /**
         * @brief calls the serializer to add a string attribute of an Agent.
         * 
         * @param type type of string value.
         * @param key name of the attribute.
         * @param value value of the attribute.
         */
        void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );

        /**
         * @brief calls the serializer to add a float of an Agent.
         * 
         * @param type type of float value.
         * @param key name of the attribute.
         * @param value value of the attribute.
         */
        void addFloatAttribute( const std::string & type, const std::string & key, float value );
        
        /**
         * @brief serialize the agents with the information of the current step.
         * 
         * @param step current simulation time.
         */
        void serializeAgents( const int & step );

        /**
         * @brief serialize the rasters with the information of the current step.
         * 
         * @param step current simulation time.
         */
        void serializeRasters( const int & step );
        int countNeighbours( Agent * target, const double & radius, const std::string & type );
        AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type );

        void setValue( DynamicRaster & raster, const Point2D<int> & position, int value );
        int getValue( const DynamicRaster & raster, const Point2D<int> & position ) const;
        void setMaxValue( DynamicRaster & raster, const Point2D<int> & position, int value );
        int getMaxValue( const DynamicRaster & raster, const Point2D<int> & position ) const;

        friend class Serializer;

        size_t getNumberOfOverlapAgents( ) const { return _overlapAgents.size( ); }
        AgentsList::const_iterator beginOverlapAgents( ) const{ return _overlapAgents.begin( ); }
        AgentsList::const_iterator endOverlapAgents( ) const{ return _overlapAgents.end( ); }
    };
} // namespace Engine
#endif // __SpacePartition_hxx__
