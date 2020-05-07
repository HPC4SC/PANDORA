
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

#ifndef __SimulationRecord_hxx__
#define __SimulationRecord_hxx__

#include <vector>
#include <string>
#include <list>
#include <map>
#include <hdf5.h>

#include <DynamicRaster.hxx>
#include <StaticRaster.hxx>
#include <AgentRecord.hxx>
#include <Point2D.hxx>
#include <Size.hxx>

namespace Engine
{

class SimulationRecord
{
public:
    typedef std::map<std::string, AgentRecord * > AgentRecordsMap;

    typedef std::map<std::string, int > IntAttributesMap;
    typedef std::map<std::string, float > FloatAttributesMap;
    typedef std::map<std::string, std::string > StrAttributesMap;

    typedef std::vector<DynamicRaster> RasterHistory;
    typedef std::map<std::string, RasterHistory> RasterMap;
    typedef std::map<std::string, StaticRaster> StaticRasterMap;
    typedef std::map<std::string, AgentRecordsMap > AgentTypesMap;
    typedef std::vector<AgentRecord *> AgentRecordsVector;
private:

    std::string _name; //! Name of the simulation.
    RasterMap _resources; //! Map of the changes of the rasters during the simulation.
    StaticRasterMap _staticRasters; //! Map of the different raster maps of the simulation.
    AgentTypesMap _types; //! Map of the different agent types of the simulation.
    int _numSteps; //! Number of steps of the simulation.
    int _loadingStep; //! The step currently being loaded.
    int _loadedResolution; //! Resolution of loaded data.
    int _serializedResolution; //! Resolution of serializedData

    IntAttributesMap _minIntValues; //! Minimum values for each integer attribute of the agents.
    IntAttributesMap _maxIntValues; //! Maximum values for each integer attribute of the agents.
    FloatAttributesMap _minFloatValues; //! Minimum values for each float attribute of the agents.
    FloatAttributesMap _maxFloatValues; //! Maximum values for each float attribute of the agents.

    Size<int> _size; //! Size of the simulation world.

    bool _gui; //! False if loading without gui.
    float _loadingPercentageDone; //! Percentage of the total data loaded.
    std::string _loadingState; //! Information about which file is beeing loaded.
    static std::list< std::string > _agentTypes; //! Different agent types.
    static std::list< std::string > _agentAttributes; //! Different agent attributes.

    // we need this function in order to be called by H5Giterate. It must be static to match the C call signature
    //static herr_t registerAgentStep( hid_t loc_id, const char *name, void *opdata );

    /**
     * @brief Iterates throug all of the different agent types.
     * 
     * @param loc_id Local id.
     * @param name Name of the agent.
     * @param linfo Info of the agent file.
     * @param opdata Optative data pointer.
     * @return herr_t 
     */
    static herr_t iterateAgentTypes( hid_t loc_id, const char * name, const H5L_info_t *linfo, void *opdata );

    /**
     * @brief  Iterates throug all of the different agent datasets.
     * 
     * @param loc_id Local id.
     * @param name Name of the agent.
     * @param linfo Info of the agent file.
     * @param opdata Optative data pointer.
     * @return herr_t 
     */
    static herr_t iterateAgentDatasets( hid_t loc_id, const char * name, const H5L_info_t *linfo, void *opdata );

    /**
     * @brief Loads the agents of the simulation.
     * 
     * @param path Path to the agent record file.
     * @param numStepsToLoad Number of steps to be loaded.
     * @param numTasks Number of tasks executing the Agents.
     */
    void loadAgentsFiles( const std::string & path, int numStepsToLoad, int numTasks  );
    
    /**
     * @brief Registers the complete list of agent types into SimulationRecord.
     * 
     * @param rootGroup Base group of the agent files.
     */
    void registerAgentTypes( const hid_t & rootGroup );
     
    /**
     * @brief Looks for the list of agents present at a given time step ( defined by stepGroyp ) and type ( defined in agents ).
     * 
     * @param stepGroup Time step checked.
     * @param indexAgents Names of the agents.
     * @param agents Agent types selected.
     * @return hssize_t 
     */
    hssize_t registerAgentIds( const hid_t & stepGroup, std::vector<std::string> & indexAgents, AgentRecordsMap & agents );
    
    /**
     * @brief Loads the attributes of the agents present at a given time step ( defined by stepGroyp ) and type ( defined in agents ).
     * 
     * @param stepGroup Time step checked.
     * @param numElements Number of attributes of the Agent.
     * @param indexAgents Names of the agents.
     * @param agents Agent types selected.
     */
    void loadAttributes( const hid_t & stepGroup, hssize_t & numElements, const std::vector<std::string> & indexAgents, AgentRecordsMap & agents );
    
    /**
     * @brief updates max values checking value for the attribute key.
     * 
     * @param key Name of the attribute.
     * @param value New max value of the attribute.
     */
    void updateMinMaxAttributeValues( const std::string & key, int value );

    /**
     * @brief updates min/max values checking value for the attribute key
     * 
     * @param key Name of the attribute.
     * @param value New max value of the attribute.
     */
    void updateMinMaxAttributeValues( const std::string & key, float value );
public:
    /**
     * @brief Construct a new SimulationRecord object.
     * 
     * @param loadedResolution Default step resolution.
     * @param gui True the GUI is activated, false the GUI is not activated.
     */
    SimulationRecord( int loadedResolution = 1, bool gui = true );

    /**
     * @brief Destroy the SimulationRecord object.
     * 
     */
    virtual ~SimulationRecord( );

    // the real method, called from registerAgentStep
    //void registerAgent( hid_t loc_id, const char * name );

    /**
     * @brief Loads the specified HDF5 file.
     * 
     * @param fileName Name of the file beeing loaded.
     * @param loadRasters True, the rasters are loaded, false the raster are not loaded.
     * @param loadAgents True, the agents are loaded, false the agents are not loaded.
     * @return true 
     * @return false 
     */
    bool loadHDF5( const std::string & fileName, const bool & loadRasters=true, const bool & loadAgents=true );

    /**
     * @brief Gets the history of the specified raster.
     * 
     * @param key Name of the raster.
     * @return RasterHistory& 
     */
    RasterHistory & getRasterHistory( const std::string & key );

    /**
     * @brief Gets the history of the specified raster.
     * 
     * @param key Name of the raster.
     * @return const RasterHistory& 
     */
    const RasterHistory & getRasterHistory( const std::string & key ) const;

    /**
     * @brief Gets the history of the specified dynamic raster on a selected step.
     * 
     * @param key Name of the raster.
     * @param step Step checked.
     * @return DynamicRaster& 
     */
    DynamicRaster & getDynamicRaster( const std::string & key, const int & step );

    /**
     * @brief Gets the history of the specified static raster.
     * 
     * @param key Name of the raster. 
     * @return StaticRaster& 
     */
    StaticRaster & getStaticRaster( const std::string & key );
    
    /**
     * @brief Gets the history of the specified static raster on a selected step.
     * 
     * @param key Name of the raster.
     * @param step Step checked.
     * @return StaticRaster& 
     */
    StaticRaster & getRasterTmp( const std::string & key, const int & step );

    /**
     * @brief Get the _name attribute.
     * 
     * @return const std::string& 
     */
    const std::string & getName( ) const;

    /**
     * @brief Get the _numSteps attribute.
     * 
     * @return int 
     */
    int getNumSteps( ) const;

    /**
     * @brief Get the _serializedResolution attribute.
     * 
     * @return int 
     */
    int getSerializedResolution( ) const;

    /**
     * @brief Get the Loaded Resolution object
     * 
     * @return int 
     */
    int getLoadedResolution( ) const;
    
    // the final resolution is the result of multiplying serialized x loaded resolution
    /**
     * @brief Get the _loadedResolution attribute.
     * 
     * @return int 
     */
    int getFinalResolution( ) const;

    /**
     * @brief Returns a interator pointing to the first position of the agents of the specified type.
     * 
     * @param type Type of the agents.
     * @return AgentRecordsMap::const_iterator 
     */
    AgentRecordsMap::const_iterator beginAgents( const std::string & type ) const;

    /**
     * @brief eturns a interator pointing to the last position of the agents of the specified type.
     * 
     * @param type Type of the agents.
     * @return AgentRecordsMap::const_iterator 
     */
    AgentRecordsMap::const_iterator endAgents( const std::string & type ) const;

    /**
     * @brief Returns a interator pointing to the first position of the agents of the specified type.
     * 
     * @param type Type of the agents.
     * @return AgentRecordsMap::const_iterator 
     */
    AgentRecordsMap::const_iterator beginAgents( AgentTypesMap::const_iterator & it ) const;

    /**
     * @brief Returns a interator pointing to the last position of the agents of the specified type.
     * 
     * @param type Type of the agents.
     * @return AgentRecordsMap::const_iterator 
     */
    AgentRecordsMap::const_iterator endAgents( AgentTypesMap::const_iterator & it ) const;

    /**
     * @brief Returns if there is a agent of the specified type.
     * 
     * @param type Specified type.
     * @return true 
     * @return false 
     */
    bool hasAgentType( const std::string & type ) const;

    /**
     * @brief Returns a interator pointing to the first position of the _types attribute.
     * 
     * @return AgentTypesMap::const_iterator 
     */
    AgentTypesMap::const_iterator beginTypes( ) const;

    /**
     * @brief Returns a interator pointing to the last position of the _types attribute.
     * 
     * @return AgentTypesMap::const_iterator 
     */
    AgentTypesMap::const_iterator endTypes( ) const;

    /**
     * @brief Returns a interator pointing to the first position of the _resources attribute.
     * 
     * @return AgentTypesMap::const_iterator 
     */
    RasterMap::const_iterator beginRasters( ) const;

    /**
     * @brief Returns a interator pointing to the last position of the _resources attribute.
     * 
     * @return RasterMap::const_iterator 
     */
    RasterMap::const_iterator endRasters( ) const;

    /**
     * @brief Returns a interator pointing to the first position of the _staticRasters attribute.
     * 
     * @return StaticRasterMap::const_iterator 
     */
    StaticRasterMap::const_iterator beginStaticRasters( ) const;

    /**
     * @brief Returns a interator pointing to the last position of the _staticRasters attribute.
     * 
     * @return StaticRasterMap::const_iterator 
     */
    StaticRasterMap::const_iterator endStaticRasters( ) const;

    /**
     * @brief Get the agents at a specified position.
     * 
     * @param step Specified step.
     * @param position Specified position.
     * @return AgentRecordsVector 
     */
    AgentRecordsVector getAgentsAtPosition( int step, const Point2D<int> & position ) const;

    /**
     * @brief Get the mean of a attribute in a determinate step.
     * 
     * @param type Type of the agent.
     * @param attribute Attribute we want to know the mean.
     * @param step Selected step.
     * @return double 
     */
    double getMean( const std::string & type, const std::string & attribute, int step );

    /**
     * @brief Get the mean of a sum in a determinate step.
     * 
     * @param type Type of the agent.
     * @param attribute Attribute we want to know the sum.
     * @param step Selected step.
     * @return double 
     */
    double getSum( const std::string & type, const std::string & attribute, int step );

    /**
     * @brief Get the minimum value of a integer attribute.
     * 
     * @param attribute Name of the attribute.
     * @return int 
     */
    int getMinInt( const std::string & attribute );

    /**
     * @brief Get the maximum value of a integer attribute.
     * 
     * @param attribute Name of the attribute.
     * @return int 
     */
    int getMaxInt( const std::string & attribute );

    /**
     * @brief Get the minimum value of a float attribute.
     * 
     * @param attribute Name of the attribute.
     * @return float 
     */
    float getMinFloat( const std::string & attribute );

    /**
     * @brief Get the maximum value of a float attribute.
     * 
     * @param attribute Name of the attribute.
     * @return float 
     */
    float getMaxFloat( const std::string & attribute );

    /**
     * @brief Get the _size attribute.
     * 
     * @return const Size<int>& 
     */
    const Size<int> & getSize( ) const;

    /**
     * @brief Get the _loadingPercentageDone attribute.
     * 
     * @return const float& 
     */
    const float & getLoadingPercentageDone( ) const;

    /**
     * @brief Get the _loadingState attribute.
     * 
     * @return const std::string& 
     */
    const std::string & getLoadingState( ) const;
};

} // namespace Engine

#endif // __SimulationRecord_hxx__

