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

#ifndef __SequentialSerializer_hxx__
#define __SequentialSerializer_hxx__

#include <hdf5.h>
#include <string>
#include <map>
#include <typedefs.hxx>

namespace Engine
{

class Scheduler;
class World;
class Config;
class StaticRaster;

/** Serializer class that stores simulation data with standard HDF5 interface
  * It can be used for any Scheduler that does not use non-shared memory distribution
  */
class SequentialSerializer
{
private:
    typedef std::map< std::string, StaticRaster *> StaticRastersRefMap;
    typedef std::map< std::string, std::vector<int> * > IntMap;
    typedef std::map< std::string, std::vector<float> * > FloatMap;
    typedef std::map< std::string, std::vector<std::string> * > StringMap;
    typedef std::map< std::string, IntMap * > IntAttributesMap;
    typedef std::map< std::string, FloatMap * > FloatAttributesMap;
    typedef std::map< std::string, StringMap * > StringAttributesMap;

    const Scheduler & _scheduler;
    const Config * _config;

    hid_t _fileId;
    hid_t _agentsFileId;
    StaticRastersRefMap _dynamicRasters;

    StringAttributesMap _stringAttributes;
    IntAttributesMap _intAttributes;
    FloatAttributesMap _floatAttributes;
    std::map<std::string, int> _agentIndexMap;

    /**
     * @brief writes the serialized Agents into the output file.
     * 
     * @param type Agent type.
     * @param step current simulation step.
     */
    void executeAgentSerialization( const std::string & type, int step );

    /**
     * @brief serialize the information of an Agent un the current step
     * 
     * @param agent Agent to serialize.
     * @param step current step of the simulation.
     * @param index index of the agent in the AgentsList.
     */
    void serializeAgent( Agent * agent, const int & step, int index );

    /**
     * @brief finished the serialization of the Agents.
     * 
     * @param step current step of the simulation.
     */
    void finishAgentsSerialization( int step );
    // register the type of agent into the data structures _agentIndexsMap, _stringAttributes and _intAttributes and create HDF5 structures
    void registerType( Agent * agent );
    int getDataSize( const std::string & type );

    /**
     * @brief resets the _agentIndexMap structure.
     * 
     */
    void resetCurrentIndexs( );

    /**
     * @brief serialize the specified raster with the information of the current step.
     * 
     * @param raster raster to serialize.
     * @param datasetKey key of the generated dataset.
     */
    void serializeRaster( const StaticRaster & raster, const std::string & datasetKey );

public:
    
    /**
     * @brief Construct a new Sequential Serializer object
     * 
     * @param scheduler Specifices which schedular data must be serialized.
     */
    SequentialSerializer( const Scheduler & scheduler );
    virtual ~SequentialSerializer( );
    
    /**
     * @brief initializes the serializer with the current world
     * 
     * @param world world to be serialized.
     */
    void init( World & world );

    /**
     * @brief closes the output files.
     * 
     */
    void finish( );

    /**
     * @brief adds a string attribute of an Agent.
     * 
     * @param type type of string value.
     * @param key name of the attribute.
     * @param value value of the attribute.
     */
    void addStringAttribute( const std::string & type, const std::string & key, const std::string & value );
    
    /**
     * @brief adds an integer attribute of an Agent.
     * 
     * @param type type of int value.
     * @param key name of the attribute.
     * @param value value of the attribute.
     */
    void addIntAttribute( const std::string & type, const std::string & key, int value );

    /**
     * @brief adds a float attribute of an Agent.
     * 
     * @param type  type of float value.
     * @param key name of the attribute.
     * @param value value of the attribute.
     */
    void addFloatAttribute( const std::string & type, const std::string & key, float value );

    /**
     * @brief serialize the infotmation of the agents on the current step.
     * 
     * @param step current simulation time.
     * @param beginAgents begin pointer of the AgentsList to serialize.
     * @param endAgents end pointer of the AgentsList to serialize.
     */
    void serializeAgents( const int & step, const AgentsList::const_iterator & beginAgents, const AgentsList::const_iterator & endAgents );

    /**
     * @brief serializes the state of the static rasters.
     * 
     * @param staticRasters map of static rasters to serialize.
     */
    void serializeStaticRasters( const StaticRastersRefMap & staticRasters );

    /**
     * @brief serialize the rasters with the information of the current step.
     * 
     * @param step current simulation time.
     */
    void serializeRasters( int step );

};

} // namespace Engine

#endif // __SequentialSerializer_hxx__

