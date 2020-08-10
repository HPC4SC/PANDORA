
#include <RandomWorld.hxx>

#include <RandomWorldConfig.hxx>
#include <Bug.hxx>
#include <Predator.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Scheduler.hxx>
#include <Logger.hxx>
#include <RNGNormal.hxx>

namespace Examples  
{

RandomWorld::RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false) {}

RandomWorld::~RandomWorld() {}

void RandomWorld::createRasters() {
	const RandomWorldConfig & randomConfig = (const RandomWorldConfig&)getConfig();
	// the raster of the food is created and initialized with value 0 and maximum value 100
	registerDynamicRaster("food", true);
	getDynamicRaster("food").setInitValues(0,100,0);
	for (int i = 0; i < randomConfig.getSize().getHeight(); ++i)
	{
		for (int j = 0; j < randomConfig.getSize().getWidth(); ++j)
		{
			Engine::Point2D<int> point = Engine::Point2D<int>(i, j);
			getDynamicRaster("food").setValue(point, std::abs(j-i));
		}
	}
	// the attribute _maxFoodProduction with the in value
	setMaxProductionRate(randomConfig._maxFoodProduction);
}

void RandomWorld::createAgents() {
    std::stringstream logName;
	logName << "agents_" << getId();
    const RandomWorldConfig & randomConfig = (const RandomWorldConfig&)getConfig();
    // the bugs are created with the in values of the config file

	Engine::RNGNormal rngNormal(randomConfig.getSeed(), (double) randomConfig._initialBugSizeMean, (double) randomConfig._initialBugSizeSD);
	for (int i = 0; i < randomConfig._numBugs; i++) {
			std::ostringstream oss;
			oss << "Bug_" << i;
			double size = rngNormal.draw();
			if (size < 0.0) size = 0.0; // checks that the in value of size is correct
			Bug * bug = new Bug(oss.str(), randomConfig._maxBugMovement, randomConfig._bugMaxConsumptionRate, randomConfig._survivalProbability, (int)size);
			addAgent(bug);
			bug->setRandomPosition();
			// all of this steps are registered in the log files
	        log_INFO(logName.str(), getWallTime() << " new bug: " << bug);
	}
	for (int i = 0; i < randomConfig._numPredators; i++) { // 200 predators are created and placed randomly
			std::ostringstream oss;
			oss << "Predator_" << i;
			Predator * predator = new Predator(oss.str(), randomConfig._maxPredatorHuntDistance);
			addAgent(predator);
			predator->setRandomPosition();
			log_INFO(logName.str(), getWallTime() << " new predator: " << predator);
	}
}

void RandomWorld::step() {
	// all of the events that take place during the simulation are also ragistered in the log files
	std::stringstream logName;
	logName << "simulation_" << getId();
	log_INFO(logName.str(), getWallTime() << " executing step: " << _step);
	
	if (_step%_config->getSerializeResolution() == 0) {
		_scheduler->serializeRasters(_step);
		_scheduler->serializeAgents(_step);
		log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " serialization done");
	}
	// first of all the updates of the raster are executed
	stepEnvironment();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has executed step enviroment");
	// then the agents perform their actions
	_scheduler->updateEnvironmentState();
	_scheduler->executeAgents();
	_scheduler->removeAgents();
	log_INFO(logName.str(), getWallTime() << " finished step: " << _step);
}

void RandomWorld::stepEnvironment() {
	// for all of the cells they grow a random quantity of food wihtin the stipulated range
	for(auto index : getBoundaries()) {
		float oldFood = getValue("food",index);
		//float foodProduced = Engine::GeneralState::statistics().getUniformDistValue(0,_maxProductionRate);
		float foodProduced = _maxProductionRate;
		// also we must check if the food value is more than the maximum and update it correcly
		if ((oldFood + foodProduced) > 100) setValue("food",index,100);
		else setValue("food",index,oldFood + foodProduced);
	}
}

int RandomWorld::getMaxProductionRate() const {
	return _maxProductionRate;
}

void RandomWorld::setMaxProductionRate(const int& maxProductionRate) {
	_maxProductionRate = maxProductionRate;
}


} // namespace Examples

