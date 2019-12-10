
#ifndef __MoveAction_hxx__
#define __MoveAction_hxx__

#include <Action.hxx>
#include <World.hxx>
#include <string>

namespace Engine
{
    class Agent;
}

namespace Examples
{

    class MoveAction : public Engine::Action
    {
    public:
        // creates an instance of a MoveAction
        MoveAction();
        // destroys an instance of a MoveAction
        virtual ~MoveAction();
        // code that each agent will execute when performing a MoveAction
        void execute( Engine::Agent & agent );
        /* for all of the cells within the agent's vision selects the one that
         * has the most sugar
         */
        void selectBestPos(int &new_x, int &new_y, const int &vision, Engine::World * world);
        //does the pertinent comparations to decide wich of the cells is better
        void findMaxSugar(int &maxSugar, const int &cadidateSugar, int &nearest, const int &i, const int &pos, const Engine::Point2D<int> &candidate, Engine::Point2D<int> &max);
        //selects the best position to move
        void selectBestPosition(const int &maxX_x, const int &maxX_y, const int &maxY_x, const int &maxY_y, const int &maxSugar_x, const int &maxSugar_y,const int &ini_x, const int &ini_y, int &new_x, int &new_y);
        // checks if the position (i,j) is inside the world
        bool inside(int i, int j, Engine::Rectangle<int> r);
        // auxiliar function
        std::string describe() const;
    };

} // namespace Examples

#endif // __MoveHomeAction_hxx__
