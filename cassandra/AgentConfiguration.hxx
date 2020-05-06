/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACI-N
 * http://www.bsc.es
 *
 * This file is part of Cassandra.
 * Cassandra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Cassandra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef __AgentConfiguration_hxx__
#define __AgentConfiguration_hxx__

#include <QIcon>
#include <QColor>

namespace GUI
{

class AgentConfiguration
{
	QColor _color;
	QIcon * _icon;
	bool _useIcon;
	std::string _fileName2D;
	float _size;
   bool _showValue;

    unsigned int seed = time(NULL);
		
public:
	AgentConfiguration();
	AgentConfiguration( const AgentConfiguration & prototype );
	virtual ~AgentConfiguration();

	void setColor( const QColor & color );
	void setUseIcon( const bool & useIcon );
	void setSize( const float & size );
	void setFileName2D( const std::string & fileName2D);
    void showValue( const bool & showValue ) { _showValue = showValue; }

	const bool & useIcon() const;
	const QColor & getColor() const;
	const QIcon & getIcon() const;
	const float & getSize() const;
	const std::string & getFileName2D() const;
    const bool & showValue() const { return _showValue; }
};

} // namespace GUI

#endif // __AgentConfiguration_hxx__

