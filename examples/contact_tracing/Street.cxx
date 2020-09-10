#include <Street.hxx>
#include <Walker.hxx>

#include <Scheduler.hxx>
#include <GeneralState.hxx>

namespace Examples
{

Street::Street(Engine::Config* config, Engine::Scheduler* scheduler) : World(config, scheduler, false) {}

Street::~Street() {}
	
void Street::createAgents() {
    createWalker();
}

void Street::createWalker() {
    std::ostringstream oss;
    oss << "Walker_" << _walkerId;
    _walkerId++;
    bool sick = false;
    if (Engine::GeneralState::statistics().getUniformDistValue() < _streetConfig._sickRate) sick = true;
    float drifting = Engine::GeneralState::statistics().getUniformDistValue() * _streetConfig._drifting;
    float stopping = Engine::GeneralState::statistics().getUniformDistValue() * _streetConfig._stopping;
    int stopTime = (int)Engine::GeneralState::statistics().getUniformDistValue() * _streetConfig._stopTime;
    int speed = (int) 0.2 * (2 * Engine::GeneralState::statistics().getUniformDistValue() - 1) + _streetConfig._speed;
    bool hasApp = Engine::GeneralState::statistics().getUniformDistValue() < _streetConfig._applicationRate;
    Walker* walker = new Walker(oss.str(),sick,_streetConfig._encounterRadius,_streetConfig._phoneThreshold1,
    _streetConfig._phoneThreshold2,hasApp,_streetConfig._signalRadius,_streetConfig._stopping,_streetConfig._stopTime,
    _step,_streetConfig._wander,this);
    addAgent(walker);
    Engine::Point2D<int> spawn;
    if (Engine::GeneralState::statistics().getUniformDistValue() < 0.5) {
        int randIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_topLimit.size() - 1);
        while (not this->checkPosition(_topLimit[randIndex])) randIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_topLimit.size() - 1);
        spawn = _topLimit[randIndex];
    }
    else {
        int randIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_botLimit.size() - 1);
        while (not this->checkPosition(_botLimit[randIndex])) randIndex = Engine::GeneralState::statistics().getUniformDistValue(0,_botLimit.size() - 1);
        spawn = _botLimit[randIndex];
    }
    walker->setPosition(spawn);
}

void Street::setupLimits() {
    for (unsigned int i = 0; i < _streetConfig.getSize().getWidth(); i++) {
        _topLimit.push_back(Engine::Point2D<int>(i,0));
        _botLimit.push_back(Engine::Point2D<int>(i,_streetConfig.getSize().getHeight() - 1));
    }
}

void Street::step() {
    if (_step%_config->getSerializeResolution() == 0) {
        _scheduler->serializeRasters(_step);
        _scheduler->serializeAgents(_step);
    }
    if (_step == 0) setupLimits();
    createAgents();
    _scheduler->updateEnvironmentState();
    _scheduler->executeAgents();
    _scheduler->removeAgents();
}

}