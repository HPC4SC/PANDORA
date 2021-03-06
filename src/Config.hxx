
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

#ifndef __Config__hxx__
#define __Config__hxx__

#include <cstdlib>
#include <string>
#include <Size.hxx>

class TiXmlDocument;
class TiXmlElement;

namespace Engine
{

class Config
{

protected:
    TiXmlDocument * _doc; 
    TiXmlElement * _root;

    std::string _resultsDir;         //! Base directory where the simulation results will be stored.
    std::string _resultsFile;       //! File where simulation results will be stored.
    Size<int> _size;                //! Space of simulation.
    int _numSteps;                  //! Number of time steps of the simulation.
    int _serializeResolution;       //! Number of steps to execute before serializing the state of the simulation.
    std::string _configFile;        //! XML config file ( if it exists ).
    int _seed;                      //! Seed to be used for the RNG (Statistics class).
    int _overlapSize;               //! [Only for MPI scheduler] Overlap size in number of cells, defined for partition rectangles.
    int _subpartitioningMode;       //! [Only for MPI scheduler] Subpartitioning mode 9 -> 9 subpartitions per node, 4 -> 4 subpartitions per node.
    bool _unevenPartitioning;       //! [Only for MPI scheduler] States whether the partitioning is performed uneven, i.e. regarding the agents position, or splitting the space evenly into K equal parts of ~1/K area each one. Uneven partitioning mode could be used when a uniform distribution of the agents across the simulation grid is expected. The "autoMode" is expected to be use in these kind of situations.

    bool _autoMode;                 //! [Only for MPI scheduler] Says whether the auto rebalancing mode is activated or not.
    bool _initialPartitioning;      //! [Only for MPI scheduler] To perform an initial partitioning or not (used as "false" for models that do not create initial agents and/or having warm-up periods).
    int _rebalancingFreq;           //! [Only for MPI scheduler] Rebalancing frequency check in number of steps. If < 1, then no frequency is stablished.    
    double _maximumLoadPerNode;     //! [Only for MPI scheduler] Maximum load (in secs.) that a node should support just before partitioning.
    double _maximumPercOfUnbalance; //! [Only for MPI scheduler] Maximum percentage allowed of unbalancing among nodes. From this value on, the space should be rebalanced. If 0, no rebalancing by unbalances are applied at all.
    // double _loadLowerThreshold;     //! [Only for MPI scheduler] Lower load threshold to determine whether the simulation needs to add MPI processes.
    // double _loadUpperThreshold;     //! [Only for MPI scheduler] Upper load threshold to determine whether the simulation needs to subtract MPI processes.

    bool _printInConsole;       //! For logging purposes
    bool _printInstrumentation; //! For logging purposes
    
    /**
     * @brief Finds an element in a config file.
     * 
     * @param elementPath Path to the element.
     * @return TiXmlElement* 
     */
    TiXmlElement * findElement( const std::string & elementPath );

    /**
     * @brief Opens an XML input file.
     * 
     * @param filename Name of the file to open.
     * @return TiXmlElement* 
     */
    TiXmlElement * openTiXml( const std::string & filename );

    /**
     * @brief Closes the xml config file.
     * 
     */
    void closeTiXml( );

    /**
     * @brief Loads the compulsatory input data from the config file.
     * 
     */
    void loadBaseParams( );
public:
    
    /**
     * @brief Construct the default new Config object.
     * 
     * @param size Size of the simulation space.
     * @param numSteps Number of steps of the simulation.
     * @param resultsFile Route to the simulations results file.
     * @param serializerResolution Resolution of the steps.
     */
    Config( const Size<int> & size = Size<int>( 0, 0 ), const int & numSteps = 1, const std::string & resultsFile = "data/results.h5", const int & serializerResolution = 1 );
    
    /**
     * @brief Construct a new Config object.
     * 
     * @param configFile Configuration file route.
     */
    Config( const std::string & configFile );

    /**
     * @brief Destroy the Config object.
     * 
     */
    virtual ~Config( );

    /**
    * @brief Loads the parameters from the XML, eg.config.xml, input file into the simulation
    * 
    */
    void loadFile( );

    friend std::ostream & operator<<( std::ostream & stream, const Config & c )
    {
        return stream << "Config( " << c._resultsFile << ", " << c._numSteps << " )";
    }

    /**
     * @brief Get the _size object.
     * 
     * @return const Size<int>& 
     */
    const Size<int> & getSize( ) const;
    
    /**
     * @brief Get the _seed object.
     * 
     * @return const int& 
     */
    const int& getSeed() const;

    /**
     * @brief Gets the _overlapSize member.
     * 
     * @return const int& 
     */
    const int& getOverlapSize() const;

    /**
     * @brief Gets the _printInConsole member.
     * 
     * @return const bool& 
     */
    const bool& getPrintInConsole() const;

    /**
     * @brief Gets the _printInstrumentation member.
     * 
     * @return const bool& 
     */
    const bool& getPrintInstrumentation() const;

    /**
     * @brief Gets the _subPartitioningMode member.
     * 
     * @return const int& 
     */
    const int& getSubpartitioningMode() const;

    /**
     * @brief Returns the _unevenPartitioning member.
     * 
     * @return bool
     */
    const bool& requiresUnevenPartitioning() const;

    /**
     * @brief Gets the _autoMode member.
     * 
     * @return bool
     */
    const bool& getAutoMode() const;

    /**
     * @brief Gets the _initialPartitioning member.
     * 
     * @return const int& 
     */
    const bool& getInitialPartitioning() const;

    /**
     * @brief Gets the _maximumLoadPerNode member.
     * 
     * @return const double&
     */
    const double& getMaximumLoadPerNode() const;

    /**
     * @brief Gets the _rebalancingFreq member.
     * 
     * @return const int& 
     */
    const int& getRebalancingFrequency() const;

    /**
     * @brief Gets the _maximumPercOfUnbalance member.
     * 
     * @return const double& 
     */
    const double& getMaximumPercOfUnbalance() const;

    // /**
    //  * @brief Gets the _loadLowerThreshold member.
    //  * 
    //  * @return const double& 
    //  */
    // const double& getLoadLowerThreshold() const;

    // /**
    //  * @brief Gets the _loadUpperThreshold member.
    //  * 
    //  * @return const double& 
    //  */
    // const double& getLoadUpperThreshold() const;

    /**
     * @brief Get the _numSteps member.
     * 
     * @return const int& 
     */
    const int & getNumSteps( ) const;

    /**
     * @brief Get the _serializeResolution member.
     * 
     * @return const int& 
     */
    const int & getSerializeResolution( ) const;

    /**
     * @brief Gets the _resultsDir member.
     * 
     * @return const std::string& 
     */
    const std::string& getResultsDir() const { return _resultsDir; }

    /**
     * @brief Get the _resultsFile member.
     * 
     * @return const std::string& 
     */
    const std::string & getResultsFile( ) const{return _resultsFile; }

    /**
     * @brief Load the parameters of the simulation of the config file.
     * 
     */
    virtual void loadParams( ){};

    /**
     * @brief Gets the string representation of an attribute.
     * 
     * @param elem  Element to load.
     * @param attrName Name of the attribute.
     * @return std::string 
     */
    std::string getParamStrFromElem( TiXmlElement* elem, const std::string & attrName );
    
    /**
     * @brief Gets the string parameter from the config file.
     * 
     * @param elementPath Path to the element to load.
     * @param attrName Name of the attribute.
     * @return std::string 
     */
    std::string getParamStr( const std::string & elementPath, const std::string & attrName );

    /**
     * @brief Parses a string element to int.
     * 
     * @param elem Element to load.
     * @param attrName Name of the attribute.
     * @return int 
     */
    int getParamIntFromElem( TiXmlElement* elem, const std::string & attrName );

    /**
     * @brief Gets the string parameter from the config file.
     * 
     * @param elementPath Path to the element to load.
     * @param attrName Name of the attribute.
     * @return int 
     */
    int getParamInt( const std::string & elementPath, const std::string & attrName );

    /**
     * @brief Parses a string element to unsigned.
     * 
     * @param elem Element to load.
     * @param attrName Name of the attribute.
     * @return unsigned 
     */
    unsigned getParamUnsignedFromElem( TiXmlElement* elem, const std::string & attrName );

    /**
     * @brief Gets the unsigned parameter from the config file.
     * 
     * @param elementPath Path to the element to load.
     * @param attrName Name of the attribute.
     * @return unsigned 
     */
    unsigned getParamUnsigned( const std::string & elementPath, const std::string & attrName );

    /**
     * @brief Parses a string element to long int.
     * 
     * @param elem Element to load.
     * @param attrName Name of the attribute.
     * @return long int 
     */
    long int getParamLongFromElem( TiXmlElement* elem, const std::string & attrName );

    /**
     * @brief Gets the long int parameter from the config file.
     * 
     * @param elementPath Path to the element to load.
     * @param attrName Name of the attribute.
     * @return long int 
     */
    long int getParamLongInt( const std::string & elementPath, const std::string & attrName );

    /**
     * @brief Parses a string element to float.
     * 
     * @param elem Element to load.
     * @param attrName Name of the attribute.
     * @return float 
     */
    float getParamFloatFromElem( TiXmlElement* elem, const std::string & attrName );

    /**
     * @brief Gets the float parameter from the config file.
     * 
     * @param elementPath Path to the element to load.
     * @param attrName Name of the attribute.
     * @return float 
     */
    float getParamFloat( const std::string & elementPath, const std::string & attrName );

    /**
     * @brief Parses a string element to bool.
     * 
     * @param elem Element to load.
     * @param attrName Name of the attribute.
     * @return true 
     * @return false 
     */
    bool getParamBoolFromElem( TiXmlElement* elem, const std::string & attrName );

    /**
     * @brief Gets the bool parameter from the config file.
     * 
     * @param elementPath Path to the element to load.
     * @param attrName Name of the attribute.
     * @return true 
     * @return false 
     */
    bool getParamBool( const std::string & elementPath, const std::string & attrName );
};

} // namespace Engine

#endif // __Config__hxx__

