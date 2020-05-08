/*
 * Copyright (c) 2013
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

#ifndef __AnalysisControlThread_hxx__
#define __AnalysisControlThread_hxx__

#include <QThread>

namespace PostProcess 
{
	class Output;
}

namespace GUI
{

class AnalysisControlThread : public QThread
{
	Q_OBJECT

	bool _cancelExecution;

	std::string _baseDir;
	std::string _type;
	std::string _outputDir;

	int _numberOfSimulations;
	int _resolution;
	bool _isRaster;

	PostProcess::Output * _output;
	void analyseSimulation( const std::string & dataFile, const std::string & fileName);

public:
	AnalysisControlThread( const std::string & baseDir, const std::string & type, const std::string & outputDir, int resolution, bool isRaster );
	virtual ~AnalysisControlThread();

	void run();
	int getNumberOfSimulations();
	void setOutput( PostProcess::Output * output);

signals:
	void nextSimulation();

public slots:
	void cancelExecution();
	
};


} // namespace GUI

#endif // __AnalysisControlThread_hxx__

