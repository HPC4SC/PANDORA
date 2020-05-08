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

#ifndef __RasterSelection_hxx__
#define __RasterSelection_hxx__

//#include <QWidget>
#include <list>
#include <string>

#include <QListWidget>

class QDropEvent;

namespace Engine
{
	class SimulationRecord;
}

namespace GUI
{

class RasterSelection : public QListWidget
{
	Q_OBJECT

	Engine::SimulationRecord * _simulationRecord;
	std::list<std::string> _rasterList;
	std::list<bool> _rasterView;
	void dropEvent( QDropEvent * event);
	void updateRasters();
public:

	RasterSelection(QWidget * parent = 0);
	virtual ~RasterSelection();
	void setSimulationRecord( Engine::SimulationRecord * simulationRecord );
	const std::list<std::string> & getRasterList() const;
	const std::list<bool> & getRasterView() const;

private slots:
    void updateRastersSlot(QListWidgetItem*);

signals:
	void rastersRearranged(std::list<std::string> items, std::list<bool> views);
}; 

} // namespace GUI

#endif // __RasterSelection_hxx__

