
#ifndef __MoveHomeAction_hxx__
#define __MoveHomeAction_hxx__

#include <Action.hxx>
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
	
	void calcInc(int &incX, int &incY);
	
	void separate(int &incX, int &incY);
	void align(int &incX, int &incY);
	void cohere(int &incX, int &incY);
	
	std::string describe() const;
};

} // namespace Examples

#endif // __MoveHomeAction_hxx__

