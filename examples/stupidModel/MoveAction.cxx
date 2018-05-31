
#include <MoveAction.hxx>

#include <World.hxx>
#include <Bug.hxx>
#include <GeneralState.hxx>

namespace Examples
{

MoveAction::MoveAction()
{
}

MoveAction::~MoveAction()
{
}

void MoveAction::execute( Engine::Agent & agent )
{	
	Engine::World * world = agent.getWorld();
	
	Engine::Point2D<int> newPosition = agent.getPosition();
	int incX = Engine::GeneralState::statistics().getUniformDistValue(-4,4);
	newPosition._x += incX;
	int incY = Engine::GeneralState::statistics().getUniformDistValue(-4,4);
	newPosition._y += incY;

	if(world->checkPosition(newPosition))
	{
		agent.setPosition(newPosition);
	}
}

std::string MoveAction::describe() const
{
	return "MoveAction";
}

} // namespace Examples

