
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class RandomWorldConfig : public Engine::Config
{
    private:
    
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

        const int& getNumAgents() const { return _numAgents; }
        const int& getMinWealth() const { return _minWealth; }
        const int& getMaxWealth() const { return _maxWealth; }
        const int& getMinVision() const { return _minVision; }
        const int& getMaxVision() const { return _maxVision; }
        const int& getMinMr() const { return _minMr; }
        const int& getMaxMr() const { return _maxMr; }
        const int& getMinMAge() const { return _minMAge; }
        const int& getMaxMAge() const { return _maxMAge; }

    friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

