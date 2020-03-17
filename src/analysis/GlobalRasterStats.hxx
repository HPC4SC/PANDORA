/*
 * Copyright (c) 2013
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
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

#ifndef __GlobalRasterStats_hxx__
#define __GlobalRasterStats_hxx__

#include <analysis/Output.hxx>
#include <memory>

namespace PostProcess
{
class RasterAnalysis;

class GlobalRasterStats : public Output
{
public:
	typedef std::list< std::list<std::string> > Params;
protected:
	typedef std::list<std::shared_ptr<RasterAnalysis> > RasterAnalysisList;
	RasterAnalysisList _analysisList; //! Analysis actions to be performed in the simulationRecord.

	bool _analysisOwnership; //! If true analysis will be deleted in destructor.

	Params * _params; //! Attributes.
	std::string _groupFile; //! Route to the group file.
	std::string _inputDir; //! Input files directory.

	/**
	 * @brief Write the attributes on the output file.
	 * 
	 * @param line Line to write.
	 * @param fileName Name of the output file.
	 */
	void writeParams( std::stringstream & line, const std::string & fileName );
public:
	/**
	 * @brief Construct a new GlobalRasterStats instance.
	 * 
	 * @param separator Separator between values on the output file.
	 */
	GlobalRasterStats( const std::string & separator=";");

	/**
	 * @brief Destroy the Global RasterStats instance.
	 * 
	 */
	virtual ~GlobalRasterStats();

	/**
	 * @brief Set the AnalysisOwnership.
	 * 
	 * @param analysisOwnership New value of _analysisOwnership.
	 */
	void setAnalysisOwnership( bool analysisOwnership );

	/**
	 * @brief Apply the analysis of the rasters.
	 * 
	 * @param simRecord Simulation record instance.
	 * @param outputFile Route to the ooutput file.
	 * @param type Type of the raster.
	 */
	void apply( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type );

	/**
	 * @brief Add an analysis to execute.
	 * 
	 * @param analysis New analysis to add.
	 */
	void addAnalysis( RasterAnalysis * analysis );

	/**
	 * @brief Add an analysis to execute.
	 * 
	 * @param analysis New analysis to add.
	 */
	void addAnalysis( std::shared_ptr<RasterAnalysis> analysis );

	/**
	 * @brief Get the Name of the calss.
	 * 
	 * 
	 * @return std::string 
	 */
	std::string getName() const;

	/**
	 * @brief Set params for performing group analysis. If a single analysis is applied a time series file will be generated following resolution and numSteps.
	 * 
	 * @param params Attributes.
	 * @param groupFile Route to the group file.
	 * @param inputDir Input files directory.
	 * @param numSteps Number of steps to analyze.
	 * @param resolution Resolution of the steps.
	 */
	void setParams( Params * params, const std::string & groupFile, const std::string & inputDir, int numSteps=1, int resolution=1);
};

} // namespace PostProcess

#endif // __GlobalRasterStats_hxx__

