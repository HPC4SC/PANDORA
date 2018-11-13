#include <MoveAction.hxx>


#include <RandomAgent.hxx>
#include <GeneralState.hxx>

namespace Examples
{

    MoveAction::MoveAction()
    {
    }

    MoveAction::~MoveAction()
    {
    }

    void MoveAction::execute( Engine::Agent & agent ) {
        Engine::World * world = agent.getWorld();
        RandomAgent & randomAgent = (RandomAgent&)agent;
        randomAgent.incCurrentAge();
        Engine::Point2D<int> newPosition = agent.getPosition();
        int new_x = newPosition._x;
        int new_y = newPosition._y;
        // the agent look for the best position to move
        selectBestPos(new_x,new_y,randomAgent.getVision(),world);
        newPosition._x = new_x;
        newPosition._y = new_y;
        // the agent consumes the sugar avaliable in that cell
        int sugarAvaliable = world->getValue("sugar", newPosition);
        world->setValue("sugar", newPosition, 0);
        int currentWealth = randomAgent.getWealth();
        int metabolicRate = randomAgent.getMetabolicRate();
        randomAgent.setWealth(currentWealth + sugarAvaliable - metabolicRate);
        // checks that the position in't occupied
        if(world->checkPosition(newPosition)) {
            agent.setPosition(newPosition);
        }
    }

    void MoveAction::selectBestPos(int &new_x, int &new_y, const int &vision, Engine::World * world) {
        Engine::Point2D<int> candidate_x, candidate_y, max_x, max_y, ini;
        ini._x = new_x;
        ini._y = new_y;
        int maxSugar_x = 0;
        int maxSugar_y = 0;
        Engine::Rectangle<int> r = world->getBoundaries();
        candidate_x._y = new_y;
        int nearest_i = ini._x - vision;
        for (int i = ini._x - vision; i < ini._x + vision; i++) { // checks the x axys cells
            if (inside(i,new_y,r)) {
                candidate_x._x = i;
                int candidateSugar = world->getValue("sugar",candidate_x);
                findMaxSugar(maxSugar_x,candidateSugar,nearest_i,i,new_x,candidate_x,max_x);
            }
                // as the space is toroidal the agent cap warp to the other side of the world
            else if (i < r.top()) {
                candidate_x._x = r.bottom() - abs(i - ini._x);
                int candidateSugar = world->getValue("sugar",candidate_x);
                findMaxSugar(maxSugar_x,candidateSugar,nearest_i,r.bottom(),new_x,candidate_x,max_x);
            }
            else if (i > r.bottom()) {
                candidate_x._x = r.top() + abs(i - ini._x);
                int candidateSugar = world->getValue("sugar",candidate_x);
                findMaxSugar(maxSugar_x,candidateSugar,nearest_i,r.top(),new_x,candidate_x,max_x);
            }
        }
        candidate_y._x = new_x;
        int nearest_j = ini._y - vision;
        for (int j = ini._y - vision; j < ini._y + vision; ++j) { // checks the y axys cells
            if (inside(new_x,j,r)) {
                candidate_y._y = j;
                int candidateSugar = world->getValue("sugar",candidate_y);
                findMaxSugar(maxSugar_y,candidateSugar,nearest_j,j,new_y,candidate_y,max_y);
            }
                // as the space is toroidal the agent cap warp to the other side of the world
            else if (j > r.right()) {
                candidate_y._y = r.left() + abs(j - ini._y);
                int candidateSugar = world->getValue("sugar",candidate_y);
                findMaxSugar(maxSugar_y,candidateSugar,nearest_j,r.left(),new_y,candidate_y,max_y);
            }
            else if (j < r.left()) {
                candidate_y._y = r.right() - abs(j - ini._y);
                int candidateSugar = world->getValue("sugar",candidate_y);
                findMaxSugar(maxSugar_y,candidateSugar,nearest_j,r.right(),new_y,candidate_y,max_y);
            }
        }
        // compares the two best cells and select that one
        if (maxSugar_x > maxSugar_y) {
            new_x = max_x._x;
            new_y = max_x._y;
        }
        else if (maxSugar_x < maxSugar_y) {
            new_x = max_y._x;
            new_y = max_y._y;
        }
        else if (ini.distance(max_x) < ini.distance(max_y)) {
            new_x = max_x._x;
            new_y = max_x._y;
        }
        else if (ini.distance(max_x) > ini.distance(max_y)) {
            new_x = max_y._x;
            new_y = max_y._y;
        }
        else { // if they are equaly good it choses ramdonly with a 50% chance each
            if (Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) {
                new_x = max_x._x;
                new_y = max_x._y;
            }
            else {
                new_x = max_y._x;
                new_y = max_y._y;
            }
        }
    }

    void MoveAction::findMaxSugar(int &maxSugar, const int &candidateSugar, int &nearest, const int &i, const int &pos, const Engine::Point2D<int> &candidate, Engine::Point2D<int> &max) {
        if (maxSugar < candidateSugar) { // checks if the quanity of sugar is grater
            maxSugar = candidateSugar;
            max = candidate;
            nearest = i;
        }
        else if (maxSugar == candidateSugar and (abs(i - pos) < abs(nearest - pos))) { // checks if its too far
            max = candidate;
            nearest = i;
        }
            // if they are equaly good it choses ramdonly with a 50% chance each
        else if (maxSugar == candidateSugar and (abs(i - pos) == abs(nearest - pos)) and Engine::GeneralState::statistics().getUniformDistValue(0,1) == 0) {
            max = candidate;
            nearest = i;
        }
    }

    bool MoveAction::inside(int i, int j, Engine::Rectangle<int> r) {
        return ((j >= r.top() and j <= r.bottom()) and (i >= r.left() and i <= r.right()));
    }

    std::string MoveAction::describe() const
    {
        return "MoveAction";
    }

} // namespace Examples