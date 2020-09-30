#ifndef __Passanger_hxx__
#define __Passanger_hxx__

#include <HumanBeeing.hxx>
#include <Train.hxx>

#include <Point2D.hxx>

#include <list>

namespace Examples
{

class Passanger : public Examples::HumanBeeing
{

    private:
        float _move;
		bool _willSeat;
		Train* _train;
		bool _exiting;
        bool _entering;
        std::list<Engine::Point2D<int>> _targetPath;
        Engine::Point2D<int> _targetPosition;
		bool _outDoorSelected = false;

    public:
        Passanger(const std::string& id, const bool& sick, const int& encounterRadius, const int& phoneT1, 
	    const int& phoneT2, const bool& phoneApp, const int& signalRadius, const float& move, const bool& wilSeat, Train* train);

        ~Passanger();

        void selectActions();

		void goingToLeave();

		void setPath();

		Engine::Point2D<int> nextPosition();

		void popCurrentPosition();

		bool getExiting();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Passanger( void * );
	void * fillPackage() const;
	void freePackage(void* package) const;
	bool hasTheSameAttributes(const Examples::HumanBeeing&) const;
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

}

#endif