
#ifndef __MoveAction_hxx__
#define __MoveAction_hxx__

#include <Action.hxx>
#include <World.hxx>
#include <string>

namespace Engine
{
	class Agent;
}

namespace Examples
{

class MoveAction : public Engine::Action
{
public:
    // creates an instance of a MoveAction
	MoveAction();
	// destroys an instance of a MoveAction
	virtual ~MoveAction();
	// code that each agent will execute when performing a MoveAction
	void execute( Engine::Agent & agent );
	/* for all of the cells within the agent's vision selects the one that
	 * has the most sugar
	 */
	void selectBestPos(int &new_x, int &new_y, const int &vision, Engine::World * world);
	// checks if the position (i,j) is inside the world
	bool inside(int i, int j, Engine::World * world);
	// auxiliar function
	std::string describe() const;
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

