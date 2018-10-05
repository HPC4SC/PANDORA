
#include <RandomWorld.hxx>

#include <RandomWorldConfig.hxx>
#include <RandomAgent.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>
#include <Logger.hxx>
#include <Scheduler.hxx>

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
	// we create the raster with maximum value 4
	registerDynamicRaster("sugar", true);
	getDynamicRaster("sugar").setInitValues(0,4,0);
	int w = getBoundaries().right();
	int h = getBoundaries().bottom();
	// and we give the initial values to the raster's cells
	for(auto index : getBoundaries()) {
		if ((index._x * 50/w) + (index._y * 50/h) < 15) {
			getDynamicRaster("sugar").setValue(index,0);
		}
		else if ((index._x * 50/w) + (index._y * 50/h) >= 82) {
			getDynamicRaster("sugar").setValue(index,0);
		}
		else if ((index._x * 50/w) + (index._y * 50/h) >= 15 and (index._x * 50/w) + (index._y * 50/h) < 28) {
			getDynamicRaster("sugar").setValue(index,1);
		}
		else if ((index._x * 50/w) + (index._y * 50/h) >= 69 and (index._x * 50/w) + (index._y * 50/h) < 82) {
			getDynamicRaster("sugar").setValue(index,1);
		}
		else if ((index._x * 50/w) - (index._y * 50/h) <= -40) {
			getDynamicRaster("sugar").setValue(index,1);
		}
		else if ((index._x * 50/w) + (index._y * 50/h) >= 28 and (index._x * 50/w) + (index._y * 50/h) < 35) {
			getDynamicRaster("sugar").setValue(index,2);
		}
		else if ((index._x * 50/w) + (index._y * 50/h) >= 64 and (index._x * 50/w) + (index._y * 50/h) < 69) {
			getDynamicRaster("sugar").setValue(index,2);
		}
		else if ((index._x * 50/w) - (index._y * 50/h) >= 44) {
			getDynamicRaster("sugar").setValue(index,2);
		}
		else if ((index._x * 50/w) - (index._y * 50/h) >= -40 and (index._x * 50/w) - (index._y * 50/h) < -34) {
			getDynamicRaster("sugar").setValue(index,2);
		}
		else if ((index._x * 50/w) - (index._y * 50/h) >= -1 and (index._x * 50/w) - (index._y * 50/h) < 12) {
			getDynamicRaster("sugar").setValue(index,2);
		}
		else if ((index._x * 50/w) + (index._y * 50/h) >= 35 and (index._x * 50/w) + (index._y * 50/h) < 42) {
			getDynamicRaster("sugar").setValue(index,3);
		}
		else if ((index._x * 50/w) + (index._y * 50/h) >= 58 and (index._x * 50/w) + (index._y * 50/h) < 64) {
			getDynamicRaster("sugar").setValue(index,3);
		}
		else if ((index._x * 50/w) - (index._y * 50/h) >= 38 and (index._x * 50/w) - (index._y * 50/h) < 44) {
			getDynamicRaster("sugar").setValue(index,3);
		}
		else if ((index._x * 50/w) - (index._y * 50/h) >= 12 and (index._x * 50/w) - (index._y * 50/h) < 22) {
			getDynamicRaster("sugar").setValue(index,3);
		}
		else if ((index._x * 50/w) - (index._y * 50/h) >= -11 and (index._x * 50/w) - (index._y * 50/h) < -1) {
			getDynamicRaster("sugar").setValue(index,3);
		}
		else if ((index._x * 50/w) - (index._y * 50/h) >= -34 and (index._x * 50/w) - (index._y * 50/h) < -27) {
			getDynamicRaster("sugar").setValue(index,3);
		}
		else getDynamicRaster("sugar").setValue(index,4);
	}
}

void RandomWorld::createAgents()
{
    std::stringstream logName;
	logName << "agents_" << getId();
	const RandomWorldConfig & randomConfig = (const RandomWorldConfig&)getConfig();
	// this loop creates the agents of the simulation and gives them random values for their attributes
	std::cout << "numAgents: " << randomConfig._numAgents << std::endl;
	for(int i = 0; i < randomConfig._numAgents; i++) {
		if((i%getNumTasks())==getId()) {
			std::ostringstream oss;
			oss << "RandomAgent_" << i;
			int wealth =  Engine::GeneralState::statistics().getUniformDistValue(5,25);
			int vision =  Engine::GeneralState::statistics().getUniformDistValue(1,6);
			int metabolicRate =  Engine::GeneralState::statistics().getUniformDistValue(1,4);
			int maxAge =  Engine::GeneralState::statistics().getUniformDistValue(60,100);
			RandomAgent * agent = new RandomAgent(oss.str(),wealth,vision,metabolicRate,0,maxAge);
			addAgent(agent);
			agent->setRandomPosition();
			// when the agent is created this acction is regitered into the log files
	        log_INFO(logName.str(), getWallTime() << " new agent: " << agent);
		}
	}
}

void RandomWorld::step() {  // each step of the world is registered into the log files
	std::stringstream logName;
	logName << "simulation_" << getId();
	log_INFO(logName.str(), getWallTime() << " executing step: " << _step);
	
	if (_step%_config->getSerializeResolution() == 0) {
		_scheduler->serializeRasters(_step);
		_scheduler->serializeAgents(_step);
		log_DEBUG(logName.str(), getWallTime() << " step: " << step_ << " serialization done");
	}
	stepEnvironment();
	log_DEBUG(logName.str(), getWallTime() << " step: " << _step << " has executed step enviroment");
	
	_scheduler->executeAgents();
	_scheduler->removeAgents();
	log_INFO(logName.str(), getWallTime() << " finished step: " << _step);
}

void RandomWorld::stepEnvironment() {
	// this for loop checks that the value of the raster is correct and then updates it
	Engine::DynamicRaster sugar =  getDynamicRaster("sugar");
	int w = getBoundaries().right() + 1;
	int h = getBoundaries().bottom() + 1;
	for(auto index : getBoundaries()) {
		int oldSugar = getValue("sugar",index);
		if ((index._x *50/w) + (index._y * 50/h) < 15) {
			sugar.setValue(index,0);
		}
		else if ((index._x * 50/w) + (index._y * 50/h) >= 82) {
			sugar.setValue(index,0);
		}
		else if (((index._x * 50/w) + (index._y * 50/h) >= 15 and (index._x * 50/w) + (index._y * 50/h) < 28) and ((oldSugar + 1) > 1)) {
			sugar.setValue(index,1);
		}
		else if (((index._x * 50/w) + (index._y * 50/h) >= 69 and (index._x * 50/w) + (index._y * 50/h) < 82) and ((oldSugar + 1) > 1)) {
			sugar.setValue(index,1);
		}
		else if (((index._x * 50/w) - (index._y * 50/h) <= -40) and ((oldSugar + 1) > 1)) {
			sugar.setValue(index,1);
		}
		else if (((index._x * 50/w) + (index._y * 50/h) >= 28 and (index._x * 50/w) + (index._y * 50/h) < 35) and ((oldSugar + 1) > 2)) {
			sugar.setValue(index,2);
		}
		else if (((index._x * 50/w) + (index._y * 50/h) >= 64 and (index._x * 50/w) + (index._y * 50/h) < 69) and ((oldSugar + 1) > 2)) {
			sugar.setValue(index,2);
		}
		else if (((index._x * 50/w) - (index._y * 50/h) >= 44) and ((oldSugar + 1) > 2)) {
			sugar.setValue(index,2);
		}
		else if (((index._x * 50/w) - (index._y * 50/h) >= -40 and (index._x * 50/w) - (index._y * 50/h) < -34) and ((oldSugar + 1) > 2)) {
			sugar.setValue(index,2);
		}
		else if (((index._x * 50/w) - (index._y * 50/h) >= -1 and (index._x * 50/w) - (index._y * 50/h) < 12) and ((oldSugar + 1) > 2)) {
			sugar.setValue(index,2);
		}
		else if (((index._x * 50/w) + (index._y * 50/h) >= 35 and (index._x * 50/w) + (index._y * 50/h) < 42) and ((oldSugar + 1) > 3)) {
			sugar.setValue(index,3);
		}
		else if (((index._x * 50/w) + (index._y * 50/h) >= 58 and (index._x * 50/w) + (index._y * 50/h) < 64) and ((oldSugar + 1) > 3)) {
			sugar.setValue(index,3);
		}
		else if (((index._x * 50/w) - (index._y * 50/h) >= 38 and (index._x * 50/w) - (index._y * 50/h) < 44) and ((oldSugar + 1) > 3)) {
			sugar.setValue(index,3);
		}
		else if (((index._x * 50/w) - (index._y * 50/h) >= 12 and (index._x * 50/w) - (index._y * 50/h) < 22) and ((oldSugar + 1) > 3)) {
			sugar.setValue(index,3);
		}
		else if (((index._x * 50/w) - (index._y * 50/h) >= -11 and (index._x * 50/w) - (index._y * 50/h) < -1) and ((oldSugar + 1) > 3)) {
			sugar.setValue(index,3);
		}
		else if (((index._x * 50/w) - (index._y * 50/h) >= -34 and (index._x * 50/w) - (index._y * 50/h) < -27) and ((oldSugar + 1) > 3)) {
			sugar.setValue(index,3);
		}
		else if ((oldSugar + 1) > 4) setValue("sugar",index,4);
		else  setValue("sugar",index,oldSugar + 1);
	}
}

} // namespace Examples

