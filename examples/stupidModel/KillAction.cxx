#include <KillAction.hxx>
#include <Bug.hxx>
#include <GeneralState.hxx>
#include <string>
#include <Point2D.hxx>

namespace Examples 
{
	
	KillAction::KillAction() {}
	KillAction::~KillAction() {}
	
	void KillAction::execute( Engine::Agent & agent ) {
		if (agent.exists()) {
			Bug & bugAgent = (Bug&)agent;
			int draw = Engine::GeneralState::statistics().getUniformDistValue(0,100);
			if (draw > bugAgent.getSurvivalProbability()) {
				bugAgent.kill();
				}
			else if (bugAgent.getSize() >= 100 and agent.exists()) {
				std::string parentId = bugAgent.getId();
				for(int i = 0; i < 5; ++i) {
					bugAgent.reproduce(parentId + '_' +  std::to_string(i));
				}
				bugAgent.kill();
			}
		}
	}
	
	std::string KillAction::describe() const{
		return "KillAction";
	}
}
