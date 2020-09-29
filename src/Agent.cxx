
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

#include <Agent.hxx>

#include <Exception.hxx>
#include <World.hxx>

#include <Serializer.hxx>

#include <Action.hxx>
#include <Logger.hxx>
#include <GeneralState.hxx>
#include <Statistics.hxx>

#include <typedefs_public.hxx>

namespace Engine
{

Agent::Agent( const std::string & id ) : _id( id ), _exists( true ), _position( -1, -1 ), _discretePosition(-1, -1), _world( 0 )
{
    _stringAttributes.push_back("id");
    _intAttributes.push_back("x");
    _intAttributes.push_back("y");
    _intAttributes.push_back("x_discrete");
    _intAttributes.push_back("y_discrete");
    _intAttributes.push_back("layer");
    _intAttributes.push_back("layer_discrete");
    _intAttributes.push_back("heading");
    _intAttributes.push_back("heading_discrete");
}

Agent::~Agent( )
{
}

void Agent::setWorld( World * world )
{
    _world = world;
}

World * Agent::getWorld( )
{
    return _world;
}

const World * Agent::getWorld( ) const
{
    return _world;
}

World & Agent::getWorldRef( )
{
    if ( !_world )
    {
        std::stringstream oss;
        oss << "Agent::getWorldRef - accessing world reference without defining Agent's World";
        throw Exception( oss.str( ) );
    }
    return *_world;
}

const Point2D<int> & Agent::getPosition( ) const
{
    return _position;
}

const Point2D<int>& Agent::getDiscretePosition() const
{
    return _discretePosition;
}

const int& Agent::getLayer() const
{
    return _layer;
}

const int& Agent::getDiscreteLayer() const
{
    return _discreteLayer;
}

const int& Agent::getHeading() const
{
    return _heading;
}

const int& Agent::getDiscreteHeading() const
{
    return _discreteHeading;
}

void Agent::setPosition( const Point2D<int> & position )
{
    if (_position.distanceOctile(position) <= _world->getConfig().getOverlapSize())
    {
        _position = position;
        if (_discretePosition.getX() == -1 and _discretePosition.getY() == -1)
            _discretePosition = _position;

        _world->changeAgentInMatrixOfPositions(this);
    }
    else
        throw(CreateStringStream("Agent::setPosition() - agent cannot move from " << _position << " to " << position << ": distance exceeds overlapSize.").str());
}

void Agent::setLayer(const int& layer)
{
    _layer = layer;
}

void Agent::setHeading(const int& heading)
{
    if (heading >= eMinDegree and heading <= eMaxDegree)
        _heading = heading;
    else
    {
        std::stringstream ss;
        ss << "Agent::setHeading - heading degree not valid.";
        throw Exception(ss.str());
    }
}

void Agent::rotate(const int& degrees)
{
    _heading = (_heading + degrees) % 360;
    if (_heading < 0) _heading = 360 + _heading;
}

void Agent::serializeAttribute( const std::string & name, const int & value )
{
    _world->addIntAttribute( getType( ), name, value );
}

void Agent::serializeAttribute( const std::string & name, const float & value )
{
    _world->addFloatAttribute( getType( ), name, value );
}

void Agent::serializeAttribute( const std::string & name, const std::string & value )
{
    _world->addStringAttribute( getType( ), name, value );
}

const std::string & Agent::getId( ) const
{
    return _id;
}

bool Agent::exists( ) const
{
    return _exists;
}

void Agent::setExists( bool exists )
{
    _exists = exists;
}

bool Agent::operator==(const Agent& other) const
{
    if (typeid(*this) != typeid(other)) return false;
    return hasTheSameAttributes(other);
}

bool Agent::operator<(const Agent& other) const
{
    return _id < other._id;
}

bool Agent::hasTheSameAttributes(const Agent& other) const
{
    return  _intAttributes == other._intAttributes and
            _floatAttributes == other._floatAttributes and
            _stringAttributes == other._stringAttributes and
            _id == other._id and
            _exists == other._exists and
            _position == other._position and
            _world->getId() == other._world->getId();
}

std::ostream& Agent::print( std::ostream& os ) const {
    os << "id: " << getId( ) << " pos: " << getPosition( ) << " discrete pos: " << getDiscretePosition() << "\tlayer: " << _layer << " discrete layer: " << _discreteLayer << "\texists: " << exists( );
    return getWorld( ) ? os << " at world: " << getWorld( )->getId( ) : os << " without world";
}

void Agent::remove( )
{
    _exists = false;
    if ( !_world )
    {
        std::stringstream oss;
        oss << "Agent::remove - removing agent without assigned World";
        throw Exception( oss.str( ) );
    }
    _world->addAgentToBeRemoved( this );
}

bool Agent::isType( const std::string & type ) const
{
    if ( type.compare( getType( ) )==0 )
    {
        return true;
    }
    return false;
}

std::string Agent::getType( ) const
{
    unsigned int typePos = _id.find_first_of( "_" );
    return _id.substr( 0, typePos );
}

void Agent::executeActions( )
{
    std::list<Action *>::iterator it = _actions.begin( );
    unsigned i = 0;
    while( it!=_actions.end( ) )
    {
        Action * nextAction = *it;
        nextAction->execute( (Agent& )( *this ));
        it = _actions.erase( it );
        delete nextAction;
        i++;
    }
}

void Agent::setRandomPosition( )
{
    _position = _world->getRandomPosition();
    if (_discretePosition.getX() == -1 and _discretePosition.getY() == -1)
        _discretePosition = _position;

    _world->changeAgentInMatrixOfPositions(this);
}

void Agent::changeType( const std::string & type )
{
    std::string oldType = getType( );
    size_t startPos = _id.find( oldType );
    _id.replace( startPos, oldType.length( ), type );
}

void Agent::copyContinuousValuesToDiscreteOnes()
{
    _discretePosition = _position;
    _discreteLayer = _layer;
    _discreteHeading = _heading;
}

} // namespace Engine

