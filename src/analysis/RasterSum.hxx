
#ifndef __RasterSum_hxx__
#define __RasterSum_hxx__

#include <analysis/Analysis.hxx>
#include <SimulationRecord.hxx>

namespace Engine
{
	class SimulationRecord;
}

namespace PostProcess
{

class RasterSum : public RasterAnalysis
{
public:
	/**
	 * @brief Construct a new RasterSum.
	 * 
	 */
	RasterSum();

	/**
	 * @brief Destroy the RasterSum.
	 * 
	 */
	virtual ~RasterSum();

	/**
	 * @brief Calculates the sum of the values ofteh cells of the given raster.
	 * 
	 * @param rasterHistory Raster values of the simulation.
	 */
	void computeRaster( const Engine::SimulationRecord::RasterHistory & rasterHistory );
};

} // namespace PostProcess

#endif // __RasterSum_hxx__

