
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include <Action.hxx>
#include <string>

#include <Point2D.hxx>
#include <World.hxx>


namespace Engine 
{
	class Agent;
}

namespace Examples 
{

class MoveAction : public Engine::Action {

public:
	// constructs a MoveAction instance
	MoveAction();
	// destroys a MoveAction instance
	virtual ~MoveAction();
	// code that de agent executes in a MoveAction
	void execute( Engine::Agent & agent );
	// auxiliar method that selects the best position for the agent to move to
	void moveBestPos(int &new_x, int &new_y, Engine::World * world);
	// auxiliat method that checks if the position i,j is inside the world
	bool inside(int i, int j, Engine::World * world);
	// auxiliar function
	std::string describe() const;
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

