#ifndef __Agent_hxx__
#define __Agent_hxx__

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
#include <Point2D.hxx>
#include <World.hxx>
#include <string>

namespace Engine
{
    class Action;

    /*
      This class encapsulates all the needed code to create a standard agent. Apart from id, main important attributes are the Position
      of the agent inside space ( in global coords ) and an instance to the World that owns the agent.
    */

    class Agent
    {
    public:
        typedef std::list<std::string> AttributesList;

    private:
        AttributesList _intAttributes;  //! list of integer attributes of the Agent.
        AttributesList _floatAttributes; //! list of float attributes of the Agent.
        AttributesList _stringAttributes; //! list of string attributes of the Agent.

    protected:
        
        std::string _id;                //! Agent identifier.
        bool _exists;                   //! Flag to control if agent is "dead" or "alive". it is used in analyzer in order to know if agent must be painted.

        Point2D<int> _position;         //! Up-to-date position of the agent, in global coordinates.
        Point2D<int> _discretePosition; //! Position in the current step 'i', not 'i+1'. Only updated at the beginning of each step.
        int _layer;                     //! Layer in which the agent currently is.
        int _discreteLayer;             //! Layer in which the agent was at the beginning of the current step.
        int _heading;                   //! Heading of the agent (can be ignored).
        int _discreteHeading;           //! Heading of the agent at the beginning of the current step.

        World * _world;                 //! Pointer to the world that owns this agent.

        std::list<Action*> _actions;    //! list of actions to be executed by the Agent.

        /**
         * @brief used in child class. Serializes a float attribute.
         * 
         * @param name name of the serialized attribute.
         * @param value value of the serializred attribute.
         */
        void serializeAttribute( const std::string & name, const float & value );

        /**
         * @brief used in child class. Serializes a integer attribute.
         * 
         * @param name name of the serialized attribute.
         * @param value value of the serializred attribute.
         */
        void serializeAttribute( const std::string & name, const int & value );

        /**
         * @brief used in child class. Serializes a string attribute.
         * 
         * @param name name of the serialized attribute.
         * @param value value of the serializred attribute.
         */
        void serializeAttribute( const std::string & name, const std::string & value );

    public:

        /**
         * @brief get the size of the intAttributes list.
         * 
         * @return int 
         */
        int  getIntSize( ) { return _intAttributes.size( ); }

        /**
         * @brief get the size of the floatAttributes list.
         * 
         * @return int 
         */
        int  getFloatSize( ) { return _floatAttributes.size( ); }

        /**
         * @brief get the size of the stringAttributes list.
         * 
         * @return int 
         */
        int  getStrSize( ) { return _stringAttributes.size( ); }

        /**
         * @brief adds an attribute to the intAttriutes list.
         * 
         * @param name name of the attribute.
         */
        void registerIntAttribute( const std::string & name ) { _intAttributes.push_back( name ); }

        /**
         * @brief adds an attribute to the floatAttriutes list.
         * 
         * @param name name of the attribute.
         */
        void registerFloatAttribute( const std::string & name ) { _floatAttributes.push_back( name ); }

        /**
         * @brief adds an attribute to the stringAttriutes list.
         * 
         * @param name name of the attribute.
         */
        void registerStringAttribute( const std::string & name ) { _stringAttributes.push_back( name ); }

       /**
        * @brief Construct a new Agent object.
        * 
        * @param id  unique identifier of the Agent.
        */
        Agent( const std::string & id );

        /**
         * @brief Destroy the Agent object.
         * 
         */
        virtual ~Agent( );

        /**
         * @brief get the id of the Agent.
         * 
         * @return const std::string&.
         */
        const std::string & getId( ) const;

        /**
         * @brief True if the Agent exists in the simulation. False otherwise.
         * 
         * @return true 
         * @return false 
         */
        bool exists( ) const;

        /**
         * @brief set the exists attribute.
         * 
         * @param exists new value of the exists attribute.
         */
        void setExists( bool exists );

        /**
         * @brief get the _position of the Agent.
         * 
         * @return const Point2D<int>& 
         */
        const Point2D<int> & getPosition( ) const;

        /**
         * @brief Gets the _discretePosition of the Agent.
         * 
         * @return const Point2D<int>& 
         */
        const Point2D<int>& getDiscretePosition() const;

        /**
         * @brief Gets the _layer member.
         * 
         * @return const int& 
         */
        const int& getLayer() const;

        /**
         * @brief Gets the _discreteLayer member.
         * 
         * @return const int& 
         */
        const int& getDiscreteLayer() const;

        /**
         * @brief Gets the _heading member.
         * 
         * @return const int& 
         */
        const int& getHeading() const;

        /**
         * @brief Gets the _discreteHeading member.
         * 
         * @return const int& 
         */
        const int& getDiscreteHeading() const;

        /**
         * @brief Checks whether this agent is able to move to 'position'. Should be used before calling the setPosition('position') method.
         * 
         * @param position const Point2D<int>&
         * @return bool
         */
        bool ableToMoveToPosition(const Point2D<int>& position) const;

        /**
         * @brief Sets the _position member attribute.
         * 
         * @param position new value of the position attribute.
         */
        void setPosition( const Point2D<int> & position );

        /**
         * @brief Sets the _layer member.
         * 
         * @param layer const int&
         */
        void setLayer(const int& layer);

        /**
         * @brief Sets the _heading member.
         * 
         * @param heading const int&
         */
        void setHeading(const int& heading);

        /**
         * @brief Rotates the agent 'degrees' degrees. If 'degrees' > 0 then _heading is rotated clockwise, if 'degrees' < 0 then _heading is rotated counterclockwise.
         * 
         * @param degrees const int&
         */
        void rotate(const int& degrees);

        /**
         * @brief delete the Agent from world.
         * 
         */
        void remove( );

        /**
         * @brief set the _world attribute of the Agent.
         * 
         * @param world new world attribute.
         */
        void setWorld( World * world );

        /**
         * @brief get the _world attribute pointer.
         * 
         * @return World*.
         */
        World * getWorld( );

        /**
         * @brief get the _world attribute reference.
         * 
         * @return World&.
         */
        World & getWorldRef( );

        /**
         * @brief get the _world attribute const pointer.
         * 
         * @return const World*.
         */
        const World * getWorld( ) const;

        /**
         * @brief method to locate the agent to a valid random position of _world.
         * 
         */
        void setRandomPosition( );

        /**
         * @brief Prints a representation of the state to the given stream.
         * 
         * @param os std::ostream &
         * @param agent const Agent&
         * @return std::ostream& 
         */
        friend std::ostream& operator << ( std::ostream &os, const Agent&  agent ) 
        { 
            return agent.print( os ); 
        }

        /**
         * @brief Prints a representation of the state to the given stream.
         * 
         * @param os std::ostream &
         * @param agent const Agent*
         * @return std::ostream& 
         */
        friend std::ostream& operator << ( std::ostream &os, const Agent*  agent )
        {
            return agent->print( os );
        }

        /**
         * @brief Comparison operator overload.
         * 
         * @param other const Agent&
         * @return bool
         */
        bool operator==(const Agent& other) const;

        /**
         * @brief Comparisson operator overload.
         * 
         * @param other const Agent&
         * @return bool
         */
        bool operator<(const Agent& other) const;

        /**
         * @brief Checks whether 'this' object has the exact same class attributes than 
         * 
         * @param other 
         * @return bool
         */
        virtual bool hasTheSameAttributes(const Agent& other) const;


        /**
         * @brief Checks whether the _id member of 'this' object is equal to the one passed by parameter ('agent').
         * 
         * @param agent Const Agent&
         * @return bool
         */
        bool isEqual(const Agent& agent) const
        {
            return _id == agent.getId();
        }

        /**
         * @brief Prints a representation of the state to the given stream.
         * 
         * @param os std::ostream &
         * @return std::ostream& 
         */
        virtual std::ostream& print( std::ostream& os ) const;

        /**
         * @brief Gets ALL the attributes of the agent in std::string format.
         * 
         * @return std::string
         */
        virtual std::string getFullInfo() const;

        /**
         * @brief this function returns true if the type of the agent is the one passed by reference.
         * 
         * @param type type we want to compare with.
         * @return true.
         * @return false. 
         */
        bool isType( const std::string & type ) const;

        /**
         * @brief get the type of the Agent.
         * 
         * @return std::string.
         */
        virtual std::string getType( ) const;

        /**
         * @brief defined in children, it must use serializeAttribute to save valuable data.
         * 
         */
        virtual void serialize( ){};
        
        /**
         * @brief defined in children. Updated the knowledge of the agent at the beginning of the step.
         * 
         */
        virtual void updateKnowledge( ){};
        
        /**
         * @brief defined in children. Selects the actions that the agent will perform during the current step.
         * 
         */
        virtual void selectActions( ) {};

        /**
         * @brief defined in children. Updates the state of the agents once all actions are executed.
         * 
         */
        virtual void updateState( ) {};

        /**
         * @brief executes the selected actions of the agent in the curreny step. 
         * 
         */
        void executeActions( );
        
        /**
         * @brief fills an MPI package with information of an Agent.
         * 
         * @return void* 
         */
        virtual void * fillPackage() const = 0;

        /**
         * @brief Frees the allocated memory that 'package' is pointing to.
         * 
         */
        virtual void freePackage(void* package) const = 0;

        /**
         * @brief Creates a serialized package with all the information about the changed complex data structures of the subclass and saves it in the own agent _deltaPackage. Returns the total size of the created package. Need to be implemented in the sub-agent.
         * 
         * @return sizeOfPackage int&
         */
        virtual int createComplexAttributesDeltaPackage() = 0;

        /**
         * @brief Gets the previously created package for the agent's complex attributes.
         * 
         * @param sizeOfPackage int&
         * @return void* 
         */
        virtual void* getComplexAttributesDeltaPackage(int& sizeOfPackage) = 0;

        /**
         * @brief Applies the delta changes in 'package' to this agent complex attributes.
         * 
         * @param package void*
         */
        virtual void applyComplexAttributesDeltaPackage(void* package) = 0;

        /**
         * @brief Frees the complex attributePackage identified by 'packageID'
         * 
         * 
         */
        virtual void freeComplexAttributesDeltaPackage() = 0;

        /**
         * @brief Updates the discrete data structures that have been defined in the sub-agent.
         * 
         */
        virtual void copyContinuousValuesToDiscreteOnes() = 0;

        /**
         * @brief Gets all the attributes of the agent in a std::string format. The decodeAndSetAllAttributesFromString() method assumes the same format. Should be overwritten by the subclass at the model.
         * 
         * @return std::string 
         */
        virtual std::string encodeAllAttributesInString() const;

        /**
         * @brief Gets a vector of tokens from 'line' after splitting them according to the 'delimiter'.
         * 
         * @param line const std::string&
         * @param delimiter const char&
         * @return std::vector<std::string> 
         */
        std::vector<std::string> getLineTokens(const std::string& line, const char& delimiter) const;

        /**
         * @brief Fills up this agent attributes based on the 'encodedAgent' stream. Lets the split attributes in 'tokens'. Returns the last used index for 'encodedAgent' when splitting it by '|', i.e. the number of agent base attributes-1 in 'encodedAgent'.
         * 
         * @param encodedAgent const std::string&
         * @param tokens std::vector<std::string>&
         * @return int
         */
        int fillUpBaseAttributesFromEncodedAgent(const std::string& encodedAgent, std::vector<std::string>& tokens);
        
        /**
         * @brief returns the interator pointing to the begin() position of _stringAttributes.
         * 
         * @return AttributesList::iterator.
         */
        AttributesList::iterator beginStringAttributes( ){ return _stringAttributes.begin( ); }

        /**
         * @brief returns the interator pointing to the end() position of _stringAttributes.
         * 
         * @return AttributesList::iterator.
         */
        AttributesList::iterator endStringAttributes( ){ return _stringAttributes.end( ); }

        /**
         * @brief returns the interator pointing to the begin() position of _intAttributes.
         * 
         * @return AttributesList::iterator.
         */
        AttributesList::iterator beginIntAttributes( ){ return _intAttributes.begin( ); }

        /**
         * @brief returns the interator pointing to the end() position of _intAttributes.
         * 
         * @return AttributesList::iterator.
         */
        AttributesList::iterator endIntAttributes( ){ return _intAttributes.end( ); }

        /**
         * @brief returns the interator pointing to the begin() position of _floatAttributes.
         * 
         * @return AttributesList::iterator.
         */
        AttributesList::iterator beginFloatAttributes( ){ return _floatAttributes.begin( ); }

        /**
         * @brief returns the interator pointing to the end() position of _floatAttributes.
         * 
         * @return AttributesList::iterator.
         */
        AttributesList::iterator endFloatAttributes( ){ return _floatAttributes.end( ); }

        /**
         * @brief registers the Agent attributes to be serialized.
         * 
         */
        virtual void registerAttributes( );

        /**
         * @brief changes the type of the Agent.
         * 
         * @param newType new type of the Agent.
         */
        void changeType( const std::string & newType );

        /**
         * @brief Gets the weight for this agent. By default it is 1. Can be overriden in the concrete Agent classes, in order to return a different weight per class/per agent.
         * 
         * @return int 
         */
        virtual int getWeight() const
        {
            return 1;
        }

        /**
         * @brief Initializes everything that the agent needs to run when it is loaded from a checkpoint.
         * 
         */
        virtual void initializeCPOrMPIAgent(){};

    };
} // namespace Engine
#endif //__Agent_hxx__

