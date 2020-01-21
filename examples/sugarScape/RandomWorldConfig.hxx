
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class RandomWorldConfig : public Engine::Config
{

    int _numAgents;
    int _minWealth;
    int _maxWealth;
    int _minVision;
    int _maxVision;
    int _minMr;
    int _maxMr;
    int _minMAge;
    int _maxMAge;

public:
	// creates a RandomWorldConfig instance
	RandomWorldConfig( const std::string & xmlFile );
	// destyroys a RandomWorldConifg instance
	virtual ~RandomWorldConfig();

    void loadParams();


	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

