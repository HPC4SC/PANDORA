
#ifndef __RasterMean_hxx__
#define __RasterMean_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class SimulationRecord;
}

namespace PostProcess
{

class RasterMean : public RasterAnalysis
{

private:

	long int _numCells;

public:

	/**
	 * @brief Construct a new RasterMean.
	 * 
	 */
	RasterMean();

	/**
	 * @brief Destroy the RasterMean.
	 * 
	 */
	virtual ~RasterMean();

	/**
	 * @brief Calculates the mean value of each cell during the simulation.
	 * 
	 * @param rasterHistory Raster values of the simulation.
	 */
	void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory );

	/**
	 * @brief Processes needed to be executed after the computation of the agent.
	 * 
	 */
	void postProcess();
	
};

} // namespace PostProcess

#endif // __RasterMean_hxx__

