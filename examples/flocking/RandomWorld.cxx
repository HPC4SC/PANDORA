
#include <RandomWorld.hxx>

#include <RandomWorldConfig.hxx>
#include <Bird.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <iostream>

namespace Examples {

RandomWorld::RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false) {}

RandomWorld::~RandomWorld() {}

void RandomWorld::createAgents() {
    std::stringstream logName;
	logName << "agents_" << getId();
	/* the agents are created with it's attributes initialized with 
	 * the in values and this is registered in the log files
	 */
    const RandomWorldConfig & randomConfig = (const RandomWorldConfig&)getConfig();
	for(int i=0; i<randomConfig._numBirds; i++) {
		std::ostringstream oss;
		oss << "Bird_" << i;
		Bird * agent = new Bird(oss.str(),randomConfig._agentVelocity,randomConfig._agentSigth,randomConfig._agentMindist,randomConfig._agentMaxATrun,randomConfig._agentMaxCTrun,randomConfig._agentMaxSTrun);
		addAgent(agent);
		agent->setRandomPosition();
		log_INFO(logName.str(), getWallTime() << " new agent: " << agent);
	}
}

void RandomWorld::createRasters()
{
	registerStaticRaster("population", true, 1);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("population"), "resources/population30x30.tiff", getBoundaries());

	registerDynamicRaster("humans", true, 2);
	getDynamicRaster("humans").setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("zombies", true, 3);
	getDynamicRaster("zombies").setInitValues(0, std::numeric_limits<int>::max(), 1);
}

} // namespace Examples

