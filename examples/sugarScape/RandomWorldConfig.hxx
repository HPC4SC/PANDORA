
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class RandomWorldConfig : public Engine::Config
{	
public:
	// creates a RandomWorldConfig instance
	RandomWorldConfig( const std::string & xmlFile );
	// destyroys a RandomWorldConifg instance
	virtual ~RandomWorldConfig();


	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

