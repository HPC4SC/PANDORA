
#ifndef __RandomWorld_hxx
#define __RandomWorld_hxx

#include <World.hxx>

namespace Examples 
{

class RandomWorldConfig;

class RandomWorld : public Engine::World
{
	//crea i inicialitza els Rasters sobre els que es fara la simulacio
	void createRasters();
	//crea i inicialitza els agents que duuran a terme la simulacio
	void createAgents();
public:
	//crea el World i inicialitza el scheduler
	RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler = 0);
	//destructora per defecte
	virtual ~RandomWorld();
};

} // namespace Examples 

#endif // __RandomWorld_hxx

