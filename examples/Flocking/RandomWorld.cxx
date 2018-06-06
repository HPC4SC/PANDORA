
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

void RandomWorld::createRasters() {
	registerDynamicRaster("resources", true);
	getDynamicRaster("resources").setInitValues(0, 5, 0);

	for(auto index:getBoundaries()){
		int value = Engine::GeneralState::statistics().getUniformDistValue(0,5);
        setMaxValue("resources", index, value);
	}
	updateRasterToMaxValues("resources");
}

void RandomWorld::createAgents() {
    std::stringstream logName;
	logName << "agents_" << getId();

    const RandomWorldConfig & randomConfig = (const RandomWorldConfig&)getConfig();
	for(int i=0; i<randomConfig._numBirds; i++) {
		if((i%getNumTasks())==getId()){
			std::ostringstream oss;
			oss << "Bird_" << i;
			Bird * agent = new Bird(oss.str(),randomConfig._agentVelocity,randomConfig._agentSigth,randomConfig._agentMindist,randomConfig._agentMaxATrun,randomConfig._agentMaxCTrun,randomConfig._agentMaxSTrun);
			addAgent(agent);
			agent->setRandomPosition();
	        log_INFO(logName.str(), getWallTime() << " new agent: " << agent);
		}
	}
}

} // namespace Examples

