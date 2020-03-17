
#ifndef __AgentHDFtoSHP_hxx__
#define __AgentHDFtoSHP_hxx__

#include <analysis/Output.hxx>
#include <Point2D.hxx>

#ifdef GDAL_VERSION_2
#define GDAL_DATA_CLASS GDALDataset
#else
#define GDAL_DATA_CLASS OGRDataSource
#endif

class GDAL_DATA_CLASS;
class OGRLayer;

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess
{

class AgentHDFtoSHP : public Output 
{
private:
	GDAL_DATA_CLASS * _gdalData;
	OGRLayer * _layer;

	Engine::Point2D<int> _origin; //! Origin of simulation.
	float _resolution; //! Resolution of meters of a simulation cell.
	std::string _srs; //! Spatial reference system (in wkt).

	int _numStep; // If a num step is passed only this step will be stored. If not, the entire simulation will be stored. One of the fields will be numStep, with the number of step of every agent. This variable is checked to see if definition of shapefile is complete (it will be done with the first agent computed).
	bool _definitionComplete; //! If true the definition of the fields is complete. If false the definition of the agents must be done.

	/**
	 * @brief Create a feature for the agent stored in agentRecord in time step timeStep. If storeTimeStep is true, a field called "num step" will be stored.
	 * 
	 * @param agentRecord Agent record instance.
	 * @param timeStep Selected time step.
	 * @param storeTimeStep If true the time step will be stored.
	 */
	void createFeature( const Engine::AgentRecord & agentRecord, int timeStep, bool storeTimeStep );
	
	/**
	 * @brief Defines the complete list of fields for the agentRecord to be stored.
	 * 
	 * @param agentRecord Agent record instance.
	 */
	void defineFields( const Engine::AgentRecord & agentRecord );

	/**
	 * @brief Sortens an attribute to shapefile standard (10 characters without trailing spaces).
	 * 
	 * @param fieldName Name of the field.
	 * @return std::string 
	 */
	std::string getFieldNameFromString( const std::string & fieldName );
public:
	/**
	 * @brief Construct a new AgentHDFtoSHP object.
	 * 
	 * @param origin Origin of simulation.
	 * @param resolution Resolution of meters of a simulation cell.
	 * @param srs Spatial reference system (in wkt).
	 * @param numStep Number of steps.
	 */
	AgentHDFtoSHP( const Engine::Point2D<int> & origin, float resolution, const std::string & srs, int numStep = -1 );

	/**
	 * @brief Destroy the AgentHDFtoSHP object.
	 * 
	 */
	virtual ~AgentHDFtoSHP();

	/**
	 * @brief Processes needed to be executed before the computation of the agent.
	 * 
	 * @param simRecord Simulation record instance.
	 * @param outputFile Output file route.
	 */
	void preProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );

	/**
	 * @brief Create the features of the agents.
	 * 
	 * @param agentRecord Agent record instance.
	 */
	void computeAgent( const Engine::AgentRecord & agentRecord );

	/**
	 * @brief Processes needed to be executed after the computation of the agent.
	 * 
	 * @param simRecord Simulation record instance.
	 * @param outputFile Output file route.
	 */
	void postProcess( const Engine::SimulationRecord & simRecord, const std::string & outputFile );
	
	/**
	 * @brief Name of the instance.
	 * 
	 * @return std::string 
	 */
	std::string getName() const;

};

} // namespace PostProcess

#endif // __AgentHDFtoSHP_hxx__

