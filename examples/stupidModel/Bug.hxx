
#ifndef __Bug_hxx__
#define __Bug_hxx__

#include <ConcreteAgent.hxx>
#include <Action.hxx>

#include <string>

namespace Examples 
{

class Bug : public Examples::ConcreteAgent
{
	
	int _size; // atribut que representa la mida del Bug 
	int _maxConsumptionRate; // atribut que representa la quantitat maxima de menjar que pot consumir un Bug
	int _survivalProbability = 95; // atribut que representa la probabilitat de sobreviure una mort sobtada

public:
	
	// crea un agent del tipus Bug amb els seus atributs inicialitzats amb els valors d'entrada
	Bug( const std::string & id , const int &maxConsumptionRate, const int &size);
	// destrueix un agent del tipus Bug
	virtual ~Bug();
	
	// selecciona quines accions i en quin ordre executa cada Agent de tipus Bug
	void selectActions();
	// selecciona quins atributs es vol registrar al executar la simulacio
	void registerAttributes();
	// dona valor als atributs registrats
	void serialize();

	// setter de l'atribut _size de la classe
	void setSize(const int &size);
	// getter de l'atribut _size de la classe
	int getSize() const;
	// getter de l'atribut _maxConsumptionRate
	int getMaxConsumptionRate() const;
	// getter de l'atribut _survivalProbability
	int getSurvivalProbability() const;
	// metode que fa que crea un nou agent fill
	void reproduce(const std::string &childId);


};

} // namespace Examples

#endif // __GubAgent_hxx__

