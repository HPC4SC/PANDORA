
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
#include <Exception.hxx>
#include <typedefs_public.hxx>

#include <math.h>

namespace Examples  
{

RandomWorld::RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false) {}

RandomWorld::~RandomWorld() {}

void RandomWorld::createInitialRasters() {
	const RandomWorldConfig & randomConfig = (const RandomWorldConfig&)getConfig();
	// the raster of the food is created and initialized with value 0 and maximum value 100
	registerDynamicRaster("food", true);

	int maxI = randomConfig.getSize().getHeight();
	int maxJ = randomConfig.getSize().getWidth();
	getDynamicRaster("food").setInitValues(0, std::max(maxI, maxJ),0);
	getDynamicRaster("food").setLayer(0);
	for (int i = 0; i < maxI; ++i)
	{
		for (int j = 0; j < maxJ; ++j)
		{
			Engine::Point2D<int> point = Engine::Point2D<int>(i, j);
			getDynamicRaster("food").setValue(point, std::abs(j-i));
		}
	}
	// the attribute _maxFoodProduction with the in value
	setMaxProductionRate(randomConfig._maxFoodProduction);
}

void RandomWorld::createInitialAgents() {
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
			bug->setLayer(0);
			bug->setHeading(Engine::eNorth);
			// all of this steps are registered in the log files
	        log_DEBUG(logName.str(), getWallTime() << " new bug: " << bug);
	}

	for (int i = 0; i < randomConfig._numPredators; i++) {
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
	const RandomWorldConfig& randomConfig = (const RandomWorldConfig&) getConfig();
	int maxValue = std::max(randomConfig.getSize().getHeight(), randomConfig.getSize().getWidth());
	// for all of the cells they grow a random quantity of food wihtin the stipulated range
	for(auto index : getBoundaries()) {
		float oldFood = getValue("food",index);
		//float foodProduced = Engine::GeneralState::statistics().getUniformDistValue(0,_maxProductionRate);
		float foodProduced = _maxProductionRate;

		// also we must check if the food value is more than the maximum and update it correcly
		if ((oldFood + foodProduced) > maxValue) setValue("food", index, maxValue);
		else setValue("food", index, oldFood + foodProduced);
	}
}

int RandomWorld::getMaxProductionRate() const {
	return _maxProductionRate;
}

void RandomWorld::setMaxProductionRate(const int& maxProductionRate) {
	_maxProductionRate = maxProductionRate;
}


} // namespace Examples

