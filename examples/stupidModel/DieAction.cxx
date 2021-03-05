#include <DieAction.hxx>
#include <Bug.hxx>
#include <GeneralState.hxx>
#include <string>
#include <Point2D.hxx>

namespace Examples 
{
	
DieAction::DieAction() {}
DieAction::~DieAction() {}
	
void DieAction::execute( Engine::Agent & agent ) {
	Bug & bugAgent = (Bug&)agent;
	// the bug draws a number to see if it I'll survive sudden death
	int draw = Engine::GeneralState::statistics().getUniformDistValue(0,100);
	if (draw > bugAgent.getSurvivalProbability()) bugAgent.remove();
	// if the bug is big enough it will reproduce and the die
	else if (bugAgent.getSize() >= 100 and agent.exists()) {
		std::string parentId = bugAgent.getId();
		for(int i = 0; i < 5; ++i) {
			bugAgent.reproduce(parentId + '_' +  std::to_string(i));
		}
		bugAgent.remove();
	}
}
	
std::string DieAction::describe() const{
		return "DieAction";
}

}
