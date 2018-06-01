
#ifndef __Bird_hxx__
#define __Bird_hxx__

#include <Agent.hxx>
#include <Action.hxx>
#include <GeneralState.hxx>

#include <string>
#include <vector>

namespace Examples
{

class Bird : public Engine::Agent
{
	int _velocity; //representa la velocitat a la que es mouen els agents
	int _sigth; //representa la distancia a la que els agents detecten altres agents
	int _mindist; //representa la distancia minima entre agents
	
	float _heading; //representa la direccio cap a la que es mouen els agents
	float _max_A_turn; //representa el gir maxim en graus per aliniar-se
	float _max_C_turn; //representa el gir maxim en graus per cohesionar-se
	float _max_S_turn; //representa el gir maxim en graus per separar-se

public:
	
	
	//es crea un objecte Bird amb els seus atributs inicialitzats amb els paramentres
	Bird(const std::string & id, const int &velocity, const int &sigth, const int &mindist, const float &max_A_turn, const float &max_C_turn, const float &max_S_turn);
	// es destrueix l'objecte Bird
	virtual ~Bird();
	
	//es passen les accions al scheduler en l'ordre que volem que s'executin
	void selectActions();
	//comprovem la correctesa de les variables
	void updateState();
	
	//setters i getters de tots els atributs de la classe
	void setVelocity(int v);
	int getVelocity() const;
	void setSigth(int s);
	int getSigth() const;
	void setMindist(int d);
	int getMindist();
	void setHeading(float h);
	float getHeading();
	void setMaxATurn(float maxTurn);
	float getMaxATurn() const;
	void setMaxCTurn(float maxTurn);
	float getMaxCTurn() const;
	void setMaxSTurn(float maxTurn);
	float getMaxSTurn();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
    Bird( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __Bird_hxx__

