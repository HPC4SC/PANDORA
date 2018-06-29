
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
	MoveAction();
	virtual ~MoveAction();
	void execute( Engine::Agent & agent );
	void selectBestPos(int &new_x, int &new_y, const int &vision, Engine::World * world);
	bool inside(int i, int j, Engine::World * world);
	std::string describe() const;
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

