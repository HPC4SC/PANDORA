
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
class SpacePartition;

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

    const Config * _config;
    const SpacePartition & _scheduler;

    StaticRastersRefMap _dynamicRasters;

    hid_t _agentsFileId;
    hid_t _fileId;
    // this id is used to track the data set of the agent being serialized
    hid_t _currentAgentDatasetId;

     /**
     * @brief serialize the specified raster with the information of the current step.
     * 
     * @param raster raster to serialize.
     * @param datasetKey key of the generated dataset.
     */
    void serializeRaster( const StaticRaster & raster, const std::string & datasetKey );

    // register the type of agent into the data structures _agentIndexsMap, _stringAttributes and _intAttributes and create HDF5 structures
    void registerType( Agent * agent );
    IntAttributesMap _intAttributes;
    FloatAttributesMap _floatAttributes;
    StringAttributesMap _stringAttributes;

    std::map<std::string, int> _agentIndexMap;

    void executeAgentSerialization( const std::string & type, int step );
    void resetCurrentIndexs( );

    int getDataSize( const std::string & type );

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

public:
    Serializer( const SpacePartition & scheduler );
    virtual ~Serializer( );

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

    void serializeAttribute( const std::string & name, const int & value );


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
     * @param type type of float value.
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
    void serializeAgents( const int & step, const AgentsList::const_iterator beginAgents, const AgentsList::const_iterator endAgents );

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

#endif // __Serializer_hxx__

