#ifndef __Scheduler_hxx__
#define __Scheduler_hxx__

#include <iostream>
#include <Agent.hxx>

#include <omp.h>

namespace Engine
{
    /** Scheduler is the base class to create simulation schedulers that control the flow of World and Agents execution
      * This is an implementation of the Bridge pattern, decoupling agent and position management from World class
      */
    class Scheduler
    {
    protected:
        int                     _id;            //! Identifier of the Scheduler.
        int                     _numTasks;      //! Number of MPI tasks executing the simulation
        int                     _numTasksMax;   //! Maximum number of MPI tasks available to execute the simulation.
        Engine::Rectangle<int>  _boundaries;    //! Limits of the simulation space.
        World                  *_world;         //! Pointer to the World of the simulation

        bool _printInConsole;                   //! For logging purposes
        bool _printInstrumentation;             //! For logging purposes

        bool _updateKnowledgeInParallel;        //! 
        bool _executeActionsInParallel;         //! Initialized to False by default in the init() method.
        omp_lock_t _ompLock;                    //! Lock object of OpenMP management
        int _overlapSize;                       //! [Only for MPI scheduler] Overlap size in number of cells, defined for partition rectangles.
        int _subpartitioningMode;               //! [Only for MPI scheduler] Subpartitioning mode 9 = 9 subpartitions per node, 4 = 4 subpartitions per node.

        /**
         * @brief This method returns a list with the list of agents in euclidean distance radius of position. If include center is false, position is not checked.
         * 
         * @tparam T 
         */
        template<class T> struct aggregator : public std::unary_function<T, void>
        {
            aggregator( double radius, Agent &center, const std::string & type ) :  _radius( radius ), _center( center ), _type( type )
            {
                _particularType = _type.compare( "all" );
            }
            virtual ~aggregator( ){}
            void operator( )( T neighbor )
            {
                Agent* agent = neighbor.second.get();
                if (agent->getId() == _center.getId() or not agent->exists( ) )
                {
                    return;
                }
                if (_particularType and not agent->isType(_type))
                {
                    return;
                }
                if (_center.getPosition().distance(agent->getPosition()) - _radius <= 0.0001)
                {
                    execute( neighbor );
                }
            }
            virtual void execute( T neighbor )=0;
            bool _particularType;
            double _radius;
            Agent & _center;
            std::string _type;
        };
        template<class T> struct aggregatorCount : public aggregator<T>
        {
            aggregatorCount( double radius, Agent & center, const std::string & type ) : aggregator<T>( radius, center, type ), _count( 0 ) { }
            void execute( T neighbor )
            {
                _count++;
            }
            int _count;
        };
        template<class T> struct aggregatorGet : public aggregator<T>
        {
            aggregatorGet( double radius, Agent & center, const std::string & type ) : aggregator<T>( radius, center, type ) {}
            void execute( T neighbor )
            {
                 AgentPtr agentPtr = neighbor.second;
                _neighbors.push_back(agentPtr);
            }
            AgentsVector _neighbors;
        };

    public:
        /**
         * @brief Construct a new Scheduler object created by default.
         * 
         */
        Scheduler( ) : _id( 0 ), _numTasks(1), _world( 0 ), _updateKnowledgeInParallel(false), _executeActionsInParallel(false) { }

        /**
         * @brief Destroy the Scheduler object
         * 
         */
        virtual ~Scheduler( ) {}

        /**
         * @brief Set the World object to the current scheduler
         * 
         * @param world pointer to the world currently used in the simulation
         */
        void setWorld( World * world ) 
        {
            _world = world;
        }

        /**
         * @brief Any initializing procedure BEFORE creating agents/rasters ( i.e. init communications and define boundaries for parallel nodes ). Must be implemented in child.
         * 
         * @param argc Not used
         * @param argv Not used
         */
        virtual void init( int argc, char *argv[] ) = 0;
        
        /**
         * @brief Initializing all the data needed (i.e. rasters, agents, send initial data to other nodes in parallel schedulers, etc.). Must be implemented in child.
         * 
         */
        virtual void initData( ) = 0;

        /**
         * @brief Gets whether the MPI process has tagged as 'go to sleep'.
         * 
         * @return bool
         */
        virtual bool hasBeenTaggedAsGoToSleep() const = 0;

        /**
         * @brief Gets whether the MPI process has been tagged as 'just finished'.
         * 
         * @return bool
         */
        virtual bool hasBeenTaggedAsJustFinished() const = 0;

        /**
         * @brief Gets whether the MPI process has been tagged as 'just awaken'.
         * 
         * @return true 
         * @return false 
         */
        virtual bool hasBeenTaggedAsJustAwaken() const = 0;

        /**
         * @brief Sets the process to sleep.
         * 
         */
        virtual void goToSleep() = 0;

        /**
         * @brief Updates the resources modified in the World::stepEnvironment() method. Must be implemented in child.
         * 
         */
        virtual void updateEnvironmentState() = 0;

        /**
         * @brief Rebalances the space if the scheduler have this feature.
         * 
         */
        virtual void checkForRebalancingSpace() = 0;

        /**
         * @brief Responsible for executing the agents and update world. Must be implemented in child.
         * 
         */
        virtual void executeAgents( ) = 0;
        
        /**
         * @brief Procedures that need to be executed after simulation ( i.e. finish communications for parallel nodes ). Must be implemented in child.
         * 
         */
        virtual void finish( ) = 0;

        /**
         * @brief Basic method to use while exploring boundaries of World.
         * 
         * @return const Rectangle<int>& 
         */
        virtual const Rectangle<int> & getBoundaries( ) const { return _boundaries; };

        /**
         * @brief Get a random Point2D within the area owned by this node/scheduler. Must be implemented in child.
         * 
         * @return Point2D<int> 
         */
        virtual Point2D<int> getRandomPosition( ) const = 0;

        /**
         * @brief Gets the _id member. Id will always be 0 unless the execution is distributed in some way.
         * 
         * @return const int& 
         */
        const int & getId( ) const { return _id; }



        /**
         * @brief Gets _numTasks, will always be 1 unless the execution is distributed in some way.
         * 
         * @return const int& 
         */
        const int & getNumTasks() const { return _numTasks; }

        /**
         * @brief Gets the _numTasksMax member.
         * 
         * @return const int& 
         */
        const int& getNumTasksMax() const { return _numTasksMax; }

        /**
         * @brief Get the Wall Time of the simulatio. Must be implemented in the children.
         * 
         * @return double 
         */
        virtual double getWallTime( ) const = 0;

        /**
         * @brief Do anything needed after adding agent to the list of World _agents
         * 
         * @param agent Pointer of teh selected Agent.
         * @param executedAgent True if the Agent has been executed, false otherwise.
         */
        virtual void agentAdded( AgentPtr agent, bool executedAgent )
        {
        }

        /**
         * @brief removes the necessary agents form the simulation.
         * 
         */
        virtual void removeAgents( ) = 0;

        /**
         * @brief Removes an Agent from the simulation.
         * 
         * @param agent Pointer to the removed Agent.
         */
        virtual void addAgentToBeRemoved( Agent * agent ) = 0;

        /**
         * @brief This method will return an agent, both looking at owned and ghost agents
         * 
         * @param id 
         * @return Agent* 
         */
        virtual Agent * getAgent( const std::string & id ) = 0;

        /**
         * @brief Get the Agents of the Type "type" in position "position". Must be implemented in child.
         * 
         * @param position Position to check.
         * @param layer Layer in which agents are retrieved.
         * @param type Type of the selected Agents.
         * @return AgentsVector 
         */
        virtual AgentsVector getAgent(const Point2D<int> & position, const std::string & type = "all", const int& layer = 0) = 0;

        /**
         * @brief Returns true if the 'position' is within this node boundaries (considering outer overlaps). False otherwise.
         * 
         * @param position const Engine::Point2D<int>&
         * @return bool
         */
        virtual bool positionBelongsToNode(const Engine::Point2D<int>& position) const = 0;

        /**
         * @brief Counts the neighbours of an agent of a concrete type  witith the radius. Must be implemented in child.
         * 
         * @param target Agent in the center of the radius.
         * @param radius Radius of the circle to check.
         * @param type Type of the agents to check.
         * @return int 
         */
        virtual int countNeighbours( Agent * target, const double & radius, const std::string & type ) = 0;

        /**
         * @brief Gets the neighbours of an agent of a concrete type  witith the radius. Must be implemented in child.
         * 
         * @param target Agent in the center of the radius.
         * @param radius Radius of the circle to check.
         * @param type Type of the agents to check.
         * @return AgentsVector 
         */
        virtual AgentsVector getNeighbours( Agent * target, const double & radius, const std::string & type ) = 0;

        /**
         * @brief Get the NumberOfTypedAgents. Must be implemented in the children.
         * 
         * @param type Type selected of Agent.
         * @return size_t 
         */
        virtual size_t getNumberOfTypedAgents( const std::string & type ) const = 0;

        /**
         * @brief Set the value of a concrete position. Must be implemented in child.
         * 
         * @param raster Raster to update.
         * @param position Position to update.
         * @param value New value of the position.
         */
        virtual void setValue( DynamicRaster & raster, const Point2D<int> & position, int value ) = 0;

        /**
         * @brief Get the value of a concrete position. Must be implemented in child.
         * 
         * @param raster Raster to check.
         * @param position Position to check.
         * @return int 
         */
        virtual int getValue( const DynamicRaster & raster, const Point2D<int> & position ) const = 0;

        /**
         * @brief Set the _maxValue of the position specified. Must be implemented in child.
         * 
         * @param raster Raster to update.
         * @param position Position to change the _maxValue.
         * @param value New maxValue.
         */
        virtual void setMaxValue( DynamicRaster & raster, const Point2D<int> & position, int value ) = 0;

        /**
         * @brief Get the _maxValue of the specified position. Must be implemented in child.
         * 
         * @param raster Raster to check.
         * @param position Position to check.
         * @return int 
         */
        virtual int getMaxValue( const DynamicRaster & raster, const Point2D<int> & position ) const = 0;

        /**
         * @brief Calls the serializer to add an string attribute of an Agent. Must be implemented in child.
         * 
         * @param type Type of int value.
         * @param key Name of the attribute.
         * @param value Value of the attribute.
         */
        virtual void addStringAttribute( const std::string & type, const std::string & key, const std::string & value ) = 0;

        /**
         * @brief Calls the serializer to add a integer attribute of an Agent. Must be implemented in child.
         * 
         * @param type Type of int value.
         * @param key Name of the attribute.
         * @param value Value of the attribute.
         */
        virtual void addIntAttribute( const std::string & type, const std::string & key, int value ) = 0;

        /**
         * @brief Calls the serializer to add a float attribute of an Agent. Must be implemented in child.
         * 
         * @param type Type of float value.
         * @param key Name of the attribute.
         * @param value Value of the attribute.
         */
        virtual void addFloatAttribute( const std::string & type, const std::string & key, float value ) = 0;
        
        /**
         * @brief Serialize the agents with the information of the current step. It's mandatory to implement.
         * 
         * @param step Current simulation time
         */
        virtual void serializeAgents( const int & step ) = 0;
        
        /**
         * @brief Serialize the rasters with the information of the current step. It's mandatory to implement.
         * 
         * @param step Current simulation time
         */
        virtual void serializeRasters( const int & step ) = 0;

        /**
         * @brief For logging purposes.
         * 
         * @param printInConsole 
         */
        void setPrintInConsole(bool printInConsole) { _printInConsole = printInConsole; }

        /**
         * @brief For logging purposes.
         * 
         * @param printInstrumentation 
         */
        void setPrintInstrumentation(bool printInstrumentation) { _printInstrumentation = printInstrumentation; }

        /**
         * @brief [Only implemented in MPI scheduler] Sets the _overlapSize member.
         * 
         * @param overlapSize int
         */
        void setOverlapSize(const int& overlapSize) { _overlapSize = overlapSize; }

        /**
         * @brief [Only implemented in MPI scheduler] Sets the _subpartitioningMode member.
         * 
         * @param subpartitioningMode int
         */
        void setSubpartitioningMode(const int& subpartitioningMode) { _subpartitioningMode = subpartitioningMode; }

        /**
         * @brief [OpenMP] Method to enable/disable the OpenMP paralellism.
         * 
         * @param updateKnowledgeInParallel bool
         * @param executeActionsInParallel bool
         */
        void setParallelism(bool updateKnowledgeInParallel, bool executeActionsInParallel) 
        {
            if (hasBeenTaggedAsJustFinished()) return;

            _updateKnowledgeInParallel = updateKnowledgeInParallel;
            _executeActionsInParallel = executeActionsInParallel;
        }

        /**
         * @brief [OpenMP] Pause all the threads but the one calling this function.
         * 
         */
        void pauseParallelization() { omp_set_lock(&_ompLock); }

        /**
         * @brief [OpenMP] Resume the threads that were locked in the pauseParallelization() method.
         * 
         */
        void resumeParallelization() { omp_unset_lock(&_ompLock); }

    };
} // namespace Engine
#endif // __Scheduler_hxx__

