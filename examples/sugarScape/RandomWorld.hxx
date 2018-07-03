
#ifndef __RandomWorld_hxx
#define __RandomWorld_hxx

#include <World.hxx>
#include <vector>

namespace Examples 
{

class RandomWorldConfig;

class RandomWorld : public Engine::World
{
	typedef std::vector<char> Row;
	typedef std::vector<Row> Matrix;
	
	Matrix gridValues{50,Row(50)}; // attribute that represents the max values of each cell
	
	// creates and initializes the Rasters of the simulation
	void createRasters();
	// creates and initializes the Agents of the simulation
	void createAgents();
	// initializes the Matrix with the given values
	void initValues();
public:
	// creates a RandomWold instance
	RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler = 0);
	// destroys a RandomWorld instance
	virtual ~RandomWorld();
	// executes the simulation one time-step 
	void step();
	// executes and chacks the changes of the rasters of the simulation one time-step 
	void stepEnvironment();
};

} // namespace Examples 

#endif // __RandomWorld_hxx

