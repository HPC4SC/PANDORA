
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class RandomWorldConfig : public Engine::Config
{	
public:
	RandomWorldConfig( const std::string & xmlFile );
	virtual ~RandomWorldConfig();


	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

