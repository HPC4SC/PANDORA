
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include <Action.hxx>
#include <string>

#include <Point2D.hxx>
#include <World.hxx>


namespace Engine {
	class Agent;
}

namespace Examples {

class MoveAction : public Engine::Action {

public:
	MoveAction();
	virtual ~MoveAction();
	void execute( Engine::Agent & agent );
	void MoveBestPos(int &new_x, int &new_y, Engine::World * world);
	bool inside(int i, int j, Engine::World * world);
	std::string describe() const;
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

