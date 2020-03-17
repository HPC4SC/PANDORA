
#ifndef __AgentMean_hxx__
#define __AgentMean_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess
{

class AgentMean : public AgentAnalysis
{

private:

	std::string _attributeName; //! Attribute to make the histogram.
	std::vector<int> _numAgents; //! Number of agents taken into acount for the mean.

public:

	/**
	 * @brief Construct a new AgentMean.
	 * 
	 * @param attributeName Name of the attribute we want to make the mean.
	 */
	AgentMean( const std::string & attributeName );

	/**
	 * @brief Destroy the AgentMean.
	 * 
	 */
	virtual ~AgentMean();

	/**
	 * @brief Calculate the mean of the given attribute of all agents.
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

#endif // __AgentMean_hxx__

