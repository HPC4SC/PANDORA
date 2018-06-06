
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples 
{

class RandomWorldConfig : public Engine::Config {
		
	int _numBugs;
	int _bugMaxConsumptionRate;
	int _maxFoodProduction;
	
public:
	RandomWorldConfig( const std::string & xmlFile );
	virtual ~RandomWorldConfig();

	void loadParams();

	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

