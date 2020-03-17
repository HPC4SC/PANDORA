
#ifndef __AgentNum_hxx__
#define __AgentNum_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess
{

class AgentNum : public AgentAnalysis
{

public:

	/**
	 * @brief Construct a new AgentNum.
	 * 
	 */
	AgentNum();

	/**
	 * @brief Destroy the AgentNum.
	 * 
	 */
	virtual ~AgentNum();

	/**
	 * @brief Counts the number of the agents in the simulation.
	 * 
	 * @param agentRecord Agent record instance.
	 */
	void computeAgent( const Engine::AgentRecord & agentRecord );
	
};

} // namespace PostProcess

#endif // __AgentNum_hxx__

