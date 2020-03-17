
#ifndef __AgentSum_hxx__
#define __AgentSum_hxx__

#include <analysis/Analysis.hxx>

namespace Engine
{
	class AgentRecord;
}

namespace PostProcess
{

class AgentSum : public AgentAnalysis
{
	std::string _attributeName; //! Attribute to calculate the sum.
public:
	/**
	 * @brief Construct a new AgentSum.
	 * 
	 * @param attributeName Attribute to calculate the standard deviation.
	 */
	AgentSum( const std::string & attributeName );

	/**
	 * @brief Destroy the AgentSum.
	 * 
	 */
	virtual ~AgentSum();

	/**
	 * @brief Calculate the sum of the given attribute of all agents.
	 * 
	 * @param agentRecord Agent record instance.
	 */
	void computeAgent( const Engine::AgentRecord & agentRecord );
	
};

} // namespace PostProcess

#endif // __AgentSum_hxx__

