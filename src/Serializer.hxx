
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

#ifndef __Serializer_hxx__
#define __Serializer_hxx__

#include <hdf5.h>
#include <string>
#include <map>
#include <vector>
#include <mpi.h>
#include <Rectangle.hxx>
#include <typedefs.hxx>

namespace Engine
{

class Config;
class Agent;
class StaticRaster;
class Raster;
class World;
class OpenMPIMultiNode;

class Serializer
{
private:
    typedef std::map< std::string, Raster> RastersMap;
    typedef std::map< std::string, StaticRaster *> StaticRastersRefMap;

    typedef std::map< std::string, std::vector<int> * > IntMap;
    typedef std::map< std::string, std::vector<float> * > FloatMap;
    typedef std::map< std::string, std::vector<std::string> * > StringMap;
    typedef std::map< std::string, IntMap * > IntAttributesMap;
    typedef std::map< std::string, FloatMap * > FloatAttributesMap;
    typedef std::map< std::string, StringMap * > StringAttributesMap;

    //const SpacePartition & _scheduler; //! Reference to the used Scheduler.
    const OpenMPIMultiNode & _scheduler; //! Reference to the used Scheduler.
    const Config * _config; //! Pointer to the Config instance.

    hid_t _fileId; //! Output file route.
    hid_t _agentsFileId; //! Agent information file route.
    hid_t _currentAgentDatasetId; //! This id is used to track the data set of the agent being serialized.

    StaticRastersRefMap _dynamicRasters; //! Map of the different raster maps.

    IntAttributesMap _intAttributes; //! Map of the integer attributes of the Agents.
    FloatAttributesMap _floatAttributes; //! Map of the float attributes of the Agents.
    StringAttributesMap _stringAttributes; //! Map of the string attributes of the Agents.
    
    std::map<std::string, int> _agentIndexMap; //! Map of the indexs of the Agents.

     /**
     * @brief Serialize the specified raster with the information of the current step.
     * 
     * @param raster Raster to serialize.
     * @param datasetKey Key of the generated dataset.
     */
    void serializeRaster( const StaticRaster & raster, const std::string & datasetKey );

    /**
     * @brief Register the type of agent into the data structures _agentIndexsMap, _stringAttributes and _intAttributes and create HDF5 structures.
     * 
     * @param agent Type of the Agnet.
     */
    void registerType( Agent * agent );

    /**
     * @brief Writes the serialized Agents into the output file.
     * 
     * @param type Agent type.
     * @param step Current simulation step.
     */
    void executeAgentSerialization( const std::string & type, int step );

    /**
     * @brief Resets the _agentIndexMap structure.
     * 
     */
    void resetCurrentIndexs( );

    /**
     * @brief Get the DataSize object.
     * 
     * @param type Type of the Agent.
     * @return int 
     */
    int getDataSize( const std::string & type );

    /**
     * @brief Serialize the information of an Agent un the current step.
     * 
     * @param agent Agent to serialize.
     * @param step Current step of the simulation.
     * @param index Index of the agent in the AgentsList.
     */
    void serializeAgent( Agent * agent, const int & step, int index );

    /**
     * @brief Finished the serialization of the Agents.
     * 
     * @param step Current step of the simulation.
     */
    void finishAgentsSerialization( int step );

public:
    /**
     * @brief Construct a new Serializer object.
     * 
     * @param scheduler Scheduler executing the simulation.
     */
    // Serializer( const SpacePartition & scheduler );

    /**
     * @brief Construct a new Serialize object
     * 
     * @param scheduler 
     */
    Serializer(const OpenMPIMultiNode& scheduler);

    /**
     * @brief Destroy the Serializer object.
     * 
     */
    virtual ~Serializer( );

    /**
     * @brief Initializes the serializer with the current world.
     * 
     * @param world World to be serialized.
     */
    void init( World & world );

    /**
     * @brief closes the output files.
     * 
     */
    void finish( );

    /**
     * @brief Serialize the information of an Agent un the current step.
     * 
     * @param agent Agent to serialize.
     * @param step Current step of the simulation.
     * @param index Index of the agent in the AgentsList.
     */
    void serializeAttribute( const std::string & name, const int & value );


    /**
     * @brief Adds a string attribute of an Agent.
     * 
     * @param type Type of string value.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );

    /**
     * @brief Adds an integer attribute of an Agent.
     * 
     * @param type Type of int value.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addIntAttribute( const std::string & type, const std::string & key, int value );

    /**
     * @brief Adds a float attribute of an Agent.
     * 
     * @param type Type of float value.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addFloatAttribute( const std::string & type, const std::string & key, float value );

    /**
     * @brief Serialize the infotmation of the agents on the current step.
     * 
     * @param step Current simulation time.
     * @param beginAgents Begin pointer of the AgentsList to serialize.
     * @param endAgents End pointer of the AgentsList to serialize.
     */
    void serializeAgents( const int & step, const AgentsList::const_iterator beginAgents, const AgentsList::const_iterator endAgents );

    /**
     * @brief Serializes the state of the static rasters.
     * 
     * @param staticRasters Map of static rasters to serialize.
     */
    void serializeStaticRasters( const StaticRastersRefMap & staticRasters );
    /**
     * @brief Serialize the rasters with the information of the current step.
     * 
     * @param step Current simulation time.
     */
    void serializeRasters( int step );

};

} // namespace Engine

#endif // __Serializer_hxx__

