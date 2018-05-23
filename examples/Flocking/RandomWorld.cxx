
#include <RandomWorld.hxx>

#include <RandomWorldConfig.hxx>
#include <Bird.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <iostream>

namespace Examples 
{

RandomWorld::RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false)
{
}

RandomWorld::~RandomWorld()
{
}

void RandomWorld::createRasters()
{
	registerDynamicRaster("resources", true);
	getDynamicRaster("resources").setInitValues(0, 5, 0);

	for(auto index:getBoundaries())
	{
		int value = Engine::GeneralState::statistics().getUniformDistValue(0,5);
        setMaxValue("resources", index, value);
	}
	updateRasterToMaxValues("resources");
}

void RandomWorld::createAgents()
{
    std::stringstream logName;
	logName << "agents_" << getId();

    const RandomWorldConfig & randomConfig = (const RandomWorldConfig&)getConfig();
    std::cout << "-----------------------------------------------------------------------" << std::endl;
	for(int i=0; i<randomConfig._numBirds; i++)
	{
		if((i%getNumTasks())==getId())
		{
			std::ostringstream oss;
			oss << "Bird_" << i;
			std::cout << "agent: " << oss.str() << " amb atributs: " << "velocity: " << randomConfig._agentVelocity << " sigth: " << randomConfig._agentSigth << " mindist: " << randomConfig._agentMindist << std::endl;
			std::cout << "max align turn: " << randomConfig._agentMaxATrun << " max cohere turn " << randomConfig._agentMaxCTrun << " max separate turn: " << randomConfig._agentMaxSTrun  << std::endl;
			std::cout << std::endl;
			Bird * agent = new Bird(oss.str(),randomConfig._agentVelocity,randomConfig._agentSigth,randomConfig._agentMindist,randomConfig._agentMaxATrun,randomConfig._agentMaxCTrun,randomConfig._agentMaxSTrun);
			addAgent(agent);
			agent->setRandomPosition();
	        log_INFO(logName.str(), getWallTime() << " new agent: " << agent);
		}
	}
	std::cout << "-----------------------------------------------------------------------" << std::endl;
}

} // namespace Examples

