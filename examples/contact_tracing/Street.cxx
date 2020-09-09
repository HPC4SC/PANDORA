#include <Street.hxx>

namespace Examples
{

Street::Street(Engine::Config* config, Engine::Scheduler* scheduler) : World(config, scheduler, false) {}

Street::~Street() {}

}