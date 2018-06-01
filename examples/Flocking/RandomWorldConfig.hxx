
#ifndef __RandomWorldConfig_hxx__
#define __RandomWorldConfig_hxx__

#include <Config.hxx>

namespace Examples
{

class RandomWorldConfig : public Engine::Config
{	
	int _numBirds; //representa el numero de Birds de la simulacio
	int _agentVelocity; //representa la velocitat que tindran els Birds
	int _agentSigth; //representa la distancia a la que veuen els agents
	int _agentMindist; //representa la distancia minima que hi ha d'haver entre agents
	float _agentMaxATrun; //representa el gir maxim en graus per aliniar-se
	float _agentMaxCTrun; //representa el gir maxim en graus per cohesionar-se
	float _agentMaxSTrun; //representa el gir maxim en graus per separar-se
	
public:
	RandomWorldConfig( const std::string & xmlFile );
	virtual ~RandomWorldConfig();

	//carrega els parametres des del config.xml a la simulacio
	void loadParams();

	friend class RandomWorld;
};

} // namespace Examples

#endif // __RandomWorldConfig_hxx__

