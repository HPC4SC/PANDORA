
#ifndef __AgentStdDev_hxx__
#define __AgentStdDev_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess
{

class AgentStdDev : public AgentAnalysis
{
	std::string _attributeName; //! Attribute to calculate the standard deviation.
	std::vector<int> _numAgents; //! Number of agents taken into acount for the standard deviation.
	std::vector< std::vector<float> > _values; //! Values of the attribute to check.
public:
	/**
	 * @brief Construct a new AgentStdDev.
	 * 
	 * @param attributeName Attribute to calculate the standard deviation.
	 */
	AgentStdDev( const std::string & attributeName );

	/**
	 * @brief Destroy the AgentStdDev.
	 * 
	 */
	virtual ~AgentStdDev();

	/**
	 * @brief Calculate the standard deviation of the given attribute of all agents.
	 * 
	 * @param agentRecord Agent record instance.
	 */
	void computeAgent( const Engine::AgentRecord & agentRecord );

	/**
	 * @brief Processes needed to be executed before the computation of the agent.
	 * 
	 */
	void preProcess();

	/**
	 * @brief Processes needed to be executed after the computation of the agent.
	 * 
	 */
	void postProcess();
};

} // namespace PostProcess

#endif // __AgentStdDev_hxx__

