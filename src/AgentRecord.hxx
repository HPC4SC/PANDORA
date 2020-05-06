
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

#ifndef __AgentRecord_hxx__
#define __AgentRecord_hxx__

#include <map>
#include <vector>
#include <string>

namespace Engine
{

class AgentRecord
{
    typedef std::vector<int> IntValues;
    typedef std::vector<float> FloatValues;
    typedef std::vector<std::string> StrValues;

public:
    typedef std::map<std::string, IntValues > IntAttributesMap;
    typedef std::map<std::string, FloatValues > FloatAttributesMap;
    typedef std::map<std::string, StrValues > StrAttributesMap;

private:
    std::string _id; //! id of the Agent.
    int _numSteps; //! Total number of steps in simulation ( not agent life ).
    IntAttributesMap _intAttributes; //! Map that represents the integer attributes of the Agent.
    FloatAttributesMap _floatAttributes; //! Map that represents the float attributes of the Agent.
    StrAttributesMap _strAttributes; //! Map that represents the string attributes of the Agent.

public:
    /**
     * @brief Construct a new Agent Record object.
     * 
     * @param id Identifier of the Agent.
     * @param numSteps Total number of steps in simulation ( not agent life ).
     */
    AgentRecord( const std::string & id, int numSteps );

    /**
     * @brief Destroy the Agent Record object.
     * 
     */
    virtual ~AgentRecord( );

    /**
     * @brief Adds an integer value to the _intAttributes map. If the attribute exists it's value it's updated.
     * 
     * @param numStep Current step of the simulation.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addInt( int numStep, const std::string & key, int value );

    /**
     * @brief Adds an float value to the _floatAttributes map. If the attribute exists it's value it's updated.
     * 
     * @param numStep Current step of the simulation.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addFloat( int numStep, const std::string & key, float value );

    /**
     * @brief Adds an float value to the _strAttributes map. If the attribute exists it's value it's updated.
     * 
     * @param numStep Current step of the simulation.
     * @param key Name of the attribute.
     * @param value Value of the attribute.
     */
    void addStr( int numStep, const std::string & key, const std::string & value );

    /**
     * @brief Get the value of the key integer attribute in the thep numStep.
     * 
     * @param numStep Step we want to check.
     * @param key Name of the attribute.
     * @return int 
     */
    int getInt( int numStep, const std::string & key ) const;

    /**
     * @brief Get the value of the key float attribute in the thep numStep.
     * 
     * @param numStep Step we want to check.
     * @param key Name of the attribute.
     * @return float 
     */
    float getFloat( int numStep, const std::string & key ) const;

    /**
     * @brief Get the value of the key string attribute in the thep numStep.
     * 
     * @param numStep Step we want to check.
     * @param key Name of the attribute.
     * @return const std::string& 
     */
    const std::string & getStr( int numStep, const std::string & key ) const;

    /**
     * @brief Get the state of the Agent in step numStep.
     * 
     * @param numStep Step we want to check.
     * @return std::string 
     */
    std::string getCompleteState( int numStep ) const;

    /**
     * @brief Returns the iterator to the first position of the _intAttributes.
     * 
     * @return IntAttributesMap::const_iterator 
     */
    IntAttributesMap::const_iterator beginInt( ) const { return _intAttributes.begin( ); }

    /**
     * @brief Returns the iterator to the last position of the _intAttributes.
     * 
     * @return IntAttributesMap::const_iterator 
     */
    IntAttributesMap::const_iterator endInt( ) const { return _intAttributes.end( ); }

    /**
     * @brief Returns the iterator to the first position of the _floatAttributes.
     * 
     * @return FloatAttributesMap::const_iterator 
     */
    FloatAttributesMap::const_iterator beginFloat( ) const { return _floatAttributes.begin( ); }

    /**
     * @brief Returns the iterator to the last position of the _floatAttributes.
     * 
     * @return FloatAttributesMap::const_iterator 
     */
    FloatAttributesMap::const_iterator endFloat( ) const { return _floatAttributes.end( ); }

    /**
     * @brief Returns the iterator to the first position of the _strAttributes.
     * 
     * @return StrAttributesMap::const_iterator 
     */
    StrAttributesMap::const_iterator beginStr( ) const { return _strAttributes.begin( ); }

    /**
     * @brief Returns the iterator to the last position of the _strAttributes.
     * 
     * @return StrAttributesMap::const_iterator 
     */
    StrAttributesMap::const_iterator endStr( ) const { return _strAttributes.end( ); }

    /**
     * @brief Get the _id attribute.
     * 
     * @return const std::string& 
     */
    const std::string & getId( ) const { return _id; }

    /**
     * @brief Get the _numSteps attribute.
     * 
     * @return int 
     */
    int getNumSteps( ) const { return _numSteps; }

    /**
     * @brief true id key is in _intAttributres, false otherwise.
     * 
     * @param key Name of the attribute.
     * @return true 
     * @return false 
     */
    bool isInt( const std::string & key ) const;

    /**
     * @brief true id key is in _floatAttributres, false otherwise.
     * 
     * @param key Name of the attribute.
     * @return true 
     * @return false 
     */
    bool isFloat( const std::string & key ) const;

    /**
     * @brief true id key is in _strAttributres, false otherwise.
     * 
     * @param key Name of the attribute.
     * @return true 
     * @return false 
     */
    bool isStr( const std::string & key ) const;
};

} // namespace Engine

#endif // __AgentRecord_hxx__

