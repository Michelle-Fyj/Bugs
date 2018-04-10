#include "Actor.h"
#include "StudentWorld.h"

#include <list>
#include <vector>
#include <cmath>
#include <cassert>
#include "Compiler.h"
#include <utility>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;


void Actor::doSomething() {return;}
bool Actor::isDead() {return false;}


void Actor::setDead(){return;}
void Actor::setStunned(){return;}
void Actor::setBitten(){return;}
bool Actor::isStunned(){return false;}
void Actor::gainHitPoint(){return;}
void Actor::loseHitPoint(){return;}
void Actor::changeHowManyHitPoint(int n){return;}
bool Actor::canBeBitten(){return false;}
bool Actor::canBePoisoned(){return false;}
bool Actor::canBeStunned(){return false;}
bool Actor::canRetaliate(){return false;}

bool Actor::isDanger(){return false;}
bool Actor::isFood(){return false;}
bool Actor::isPoision(){return false;}
bool Actor::isWater(){return false;}
int Actor::getAntNo(){return -999;}
int Actor::getPheromoneNo(){return -999;}
int Actor::getAntHillNo(){return -999;}
bool Actor::isPebble(){return false;}
bool Actor::thereIsPebble(int x, int y)
{
    std::list<Actor*> getAllObject = getStudentWorld()->accessActorsList(x, y);
    std::list<Actor*>::iterator it = getAllObject.begin();
    while(it!= getAllObject.end())
    {
        if((*it)->isPebble())
        {
            return true;
        }
        it++;
    }
    return false;
}





//////////////////////EnergyBasedClass////////////////////////////////////


bool EnergyBasedClass::isDead()
{
    if (m_hit_point <= 0)
        return true;
    return false;
}
int EnergyBasedClass::getHitPoint(){return m_hit_point;}
//void EnergyBasedClass::produceAnt(){ m_hit_point -= 1500;}
void EnergyBasedClass::setDead(){m_hit_point = 0;}
void EnergyBasedClass::gainHitPoint(){m_hit_point++;}
void EnergyBasedClass::loseHitPoint() {m_hit_point--;}
void EnergyBasedClass::changeHowManyHitPoint(int n){m_hit_point += n;}


//////////////////////Insect////////////////////////////////////

bool Insect::isStunned(){return m_isStunned != 0;}
bool Insect::canBeBitten(){return true;}
void Insect::deadBodyTurnToFood(int x, int y)
{
    std::list<Actor*> thereIsFood = getStudentWorld()->getFoodObject(x, y);
    if(thereIsFood.empty()) // if originally there is no food in the spot
    {
        std::list<Actor*> p = getStudentWorld()->accessActorsList(x, y);
        p.push_back(new Food(x, y, getStudentWorld(), 100));
    }
    else    // if there is already food in the spot
    {
        std::list<Actor*>::iterator it = thereIsFood.begin();
        (*it)->changeHowManyHitPoint(100);
    }
};

void Insect::setStunned(){if (!m_isStunnedHere) {m_isStunned = 2;m_isStunnedHere=true;}}

//////////////////////Ant////////////////////////////////////

bool Ant::canBeBitten(){return true;}
bool Ant::canBePoisoned(){return true;}
bool Ant::canBeStunned(){return true;}
int Ant::getAntNo(){return m_antHillNo;}

void Ant::doSomething()
{
//    fprintf(stderr, "Ant %p: do something.\n", this);
    //step 1
    loseHitPoint();
    
    //step 2
    if(getHitPoint() <= 0)
    {
        setDead();
        deadBodyTurnToFood(getX(), getY());
        return;
    }
    
    Compiler::Command cmd;
//    fprintf(stderr, "Ant %p: m_ic = %d\n", this, m_ic);
    if (m_compiler->getCommand(m_ic++, cmd)) {
        for(int i = 0; i <10 ; i++)
            if(runCommand(cmd))
                return;
    } else {
//        fprintf(stderr, "Ant %p: invalid instruction: dying\n", this);
        setDead();
        deadBodyTurnToFood(getX(), getY());
        return;
    }
}

bool Ant::isBitten(){return m_isBitten;}
bool Ant::moved(){return m_moved;}
bool Ant::isPreviouslyBlocked(){return m_isPreviouslyBlocked;}




void Ant::moveTheAntForward()
{
    int dir = getDirection();
//    fprintf(stderr, "Ant %p: moving forward from (%d,%d) in direction %d.\n", this, getX(), getY(), dir);
    // none, up, right, down, left
    int newX, newY;
    switch (dir) {
        case up:
        {
            newX = getX();
            newY = getY() + 1;

            break;
        }
        case right:
        {
            newX = getX()+1;
            newY = getY();
            break;
        }
        case down:
        {
            newX = getX();
            newY = getY() - 1;
            break;
        }
        case left:
        {
            newX = getX()-1;
            newY = getY();
            break;
        }
        default: assert(false && "invalid direction");
    }
    if(!thereIsPebble(newX, newY))
    {
        moveTo(newX, newY);
        m_isBitten = false;
        m_moved = true;
    }
    else {
//        fprintf(stderr, "Ant %p: not moving to (%d,%d) because there is pebble.]\n", this, newX, newY);
        m_moved = false;}

//    fprintf(stderr, "Ant %p: moved: %d, now at (%d,%d).\n", this, m_moved, getX(), getY());
}

void Ant::eatFood()
{
    if(m_food >= 100)
    {
        changeHowManyHitPoint(100);
        m_food -= 100;
    }
    if (m_food > 0)
    {
        changeHowManyHitPoint(m_food);
        m_food = 0;
    }
}

void Ant::dropFood()
{
    if(m_food == 0)
        return;
    
    std::list<Actor*> thereIsFood = getStudentWorld()->getFoodObject(getX(), getY());
    if(thereIsFood.empty())
    {
        getStudentWorld()->accessActorsList(getX(), getY()).push_back(new Food(getX(), getY(), getStudentWorld(), m_food));
    }
    else
    {
        std::list<Actor*>::iterator getFoodObject = thereIsFood.begin();
        (*getFoodObject)->changeHowManyHitPoint(m_food);
    }
    m_food = 0;
}

void Ant::bite()
{
    std::list<Actor*> getAllEnermy = getStudentWorld()->objectInTheSpotThatCanBeBitten(getX(), getY());
    if(getAllEnermy.empty())
        return;
    std::list<Actor*>::iterator it = getAllEnermy.begin();
    if((*it)->canRetaliate())
        (*it)->setBitten();
    (*it)->changeHowManyHitPoint(-15);
}

void Ant::pickupFood()
{
    if(m_food >= 1800)
        return;
    
    std::list<Actor*> thereIsFood = getStudentWorld()->getFoodObject(getX(), getY());
    if(thereIsFood.empty())
        return;
    std::list<Actor*>::iterator food = thereIsFood.begin();
    int canConsumeAtMost = 1800 - m_food;
    int available = (*food)->getHitPoint();
    int willConsume = std::min(400, std::min(available, canConsumeAtMost));
    (*food)->changeHowManyHitPoint(-willConsume);
    m_food += willConsume;
}

void Ant::emitPheromone()
{
    std::list<Actor*> thereIsPheromone = getStudentWorld()->getPheromone(getX(), getY());
    if(thereIsPheromone.empty())
    {
        std::list<Actor*> p = getStudentWorld()->accessActorsList(getX(), getY());
        p.push_back(new Pheromone(getAntNo(), getX(), getY(), getStudentWorld()));
    }
    else
    {
        std::list<Actor*>::iterator getPheromoneObject = thereIsPheromone.begin();
        if((*getPheromoneObject)->getHitPoint() < 768-256)
            (*getPheromoneObject)->changeHowManyHitPoint(256);
        else
        {
            int topUp = 768 - (*getPheromoneObject)->getHitPoint();
            (*getPheromoneObject)->changeHowManyHitPoint(topUp);
        }
    }
}


void Ant::faceRandomDirection()
{

    setDirection(static_cast<GraphObject::Direction>(randInt(up, left)));
//    fprintf(stderr, "Ant %p: changing direction to %d\n", this,getDirection());
}



bool Ant::if_command(Compiler::Condition condition)
{
    switch (condition) {
        case Compiler::last_random_number_was_zero:
            return m_lastRandomNumberGenerated == 0;
        case Compiler::i_am_carrying_food:
            return m_food > 0;
        case Compiler::i_am_hungry:
            return (getHitPoint() <= 25);
        case Compiler::i_am_standing_with_an_enemy:
        {
            std::list<Actor*> insects = getStudentWorld()->objectInTheSpotThatCanBeBitten(getX(), getY());
            std::list<Actor*>::iterator it = insects.begin();
            while (it != insects.end())
            {
                if(getAntNo() == (*it)->getAntNo())
                    insects.erase(it);
                else it++;
            }
            return !insects.empty();
        }
        case Compiler::i_am_standing_on_food:
        {
            std::list<Actor*> thereIsFood = getStudentWorld()->getFoodObject(getX(), getY());
            return !thereIsFood.empty();
        }
        case Compiler::i_am_standing_on_my_anthill:
        {
            std::list<Actor*> getObjects = getStudentWorld()->accessActorsList(getX(), getY());
            std::list<Actor*>::iterator it = getObjects.begin();
            while (it != getObjects.end())
            {
                if(getAntNo() == (*it)->getAntHillNo())
                    return true;
                it++;
            }
            return false;
        }
        case Compiler::i_smell_pheromone_in_front_of_me:
        {
            int destX = getX();
            int destY = getY();
            getNextPosition(destX, destY);
            std::list<Actor*> getObject = getStudentWorld()->accessActorsList(destX, destY);
            std::list<Actor*>::iterator it = getObject.begin();
            while(it != getObject.end())
            {
                if(getAntNo() == (*it)->getPheromoneNo())
                    return true;
                it++;
            }
            return false;
        }
        case Compiler::i_smell_danger_in_front_of_me:
        {
            int destX = getX();
            int destY = getY();
            getNextPosition(destX, destY);
            std::list<Actor*> getObject = getStudentWorld()->accessActorsList(destX, destY);
            std::list<Actor*>::iterator it = getObject.begin();
            while(it != getObject.end())
            {
                if((*it)->isDanger())
                    return true;
                if(getAntNo() != (*it)->getAntNo())
                    return true;
                it++;
            }
            return false;
        }
        case Compiler::i_was_bit:
            return m_isBitten;
        case Compiler::i_was_blocked_from_moving:
            return isPreviouslyBlocked();
        default:
            assert(false && "invalid ant type");
    }
}

void Ant::getNextPosition(int &x, int &y)
{
    GraphObject::Direction dir = getDirection();
    switch (dir) {
        case up:
            y++;
            break;
        case right:
            x++;
            break;
        case down:
            y--;
            break;
        case left:
            x--;
            break;
        default:
            break;
    }
}

void Ant::rotateClockwise()
{
    int dir = getDirection();
    GraphObject::Direction finalDir;
    switch (dir) {
        case up:
            finalDir = right;
            break;
        case right:
            finalDir = down;
            break;
        case down:
            finalDir = left;
            break;
        case left:
            finalDir = up;
            break;
        default:
            break;
    }
    setDirection(finalDir);
}

void Ant::rotateCounterClockwise()
{
    int dir = getDirection();
    GraphObject::Direction finalDir;
    switch (dir) {
        case up:
            finalDir = left;
            break;
        case right:
            finalDir = up;
            break;
        case down:
            finalDir = right;
            break;
        case left:
            finalDir = down;
            break;
        default:
            break;
    }
    setDirection(finalDir);
}

bool Ant::runCommand(const Compiler::Command& c)
{
    switch (c.opcode) {
        case Compiler::moveForward:
            moveTheAntForward();
            return true;
        case Compiler::if_command:
        {
            if (if_command(static_cast<Compiler::Condition>(stoi(c.operand1))))
                m_ic = stoi(c.operand2);
            return false;
        }
        case Compiler::goto_command:
            m_ic = stoi(c.operand1);
            return false;
        case Compiler::generateRandomNumber:
            generateRandomNumber(stoi(c.operand1));
            return false;
        case Compiler::rotateClockwise:
            rotateClockwise();
            return true;
        case Compiler::eatFood:
            eatFood();
            return true;
        case Compiler::dropFood:
            dropFood();
            return true;
        case Compiler::bite:
            bite();
            return true;
        case Compiler::pickupFood:
            pickupFood();
            return true;
        case Compiler::emitPheromone:
            emitPheromone();
            return true;
        case Compiler::faceRandomDirection:
            faceRandomDirection();
            return true;
        case Compiler::rotateCounterClockwise:
            rotateCounterClockwise();
            return true;
        default:
            assert(false && "invalid ant type");
    }
}

void Ant::generateRandomNumber(int n)
{
    if (n == 0)
        m_lastRandomNumberGenerated = 0;
    m_lastRandomNumberGenerated = randInt(0, n-1);
}
    

    
    //////////////////////GrassHopper////////////////////////////////////


bool GrassHopper::isDanger()
{
    return true;
}
    bool GrassHopper::isSleeping()
    {
        return sleepCount > 0;
    }
    
    void GrassHopper::setSleepCountToTwo()
    {
        //fprintf(stderr, "Grasshopper: setting sleep count to two\n");
        sleepCount = 2;
    }
    
    void GrassHopper::decrementSleepCount()
    {
        sleepCount--;
    }
    
    void GrassHopper::resetDistance()
    {
        m_distance = randInt(2, 10);
    }
    
    int GrassHopper::getDistance()
    {
        return m_distance;
    }
    
    void GrassHopper::basicDoSomething()
    {
        //step 6
        std::list<Actor*> thereIsFood = getStudentWorld()->getFoodObject(getX(), getY());
        if(!thereIsFood.empty())
        {
//            fprintf(stderr, "Grasshopper %p at (%d,%d): there is food here.\n", this, getX(), getY());
            std::list<Actor*>::iterator food = thereIsFood.begin();
            int howMuchFood = (*food)->getHitPoint();
            if( howMuchFood > 200)
            {
                (*food)->changeHowManyHitPoint(-200);
                changeHowManyHitPoint(200);
            }
            else    // there is not enough food in the spot
            {
                changeHowManyHitPoint(howMuchFood);
                (*food)->setDead();
            }
            //step 7: grasshopper did eat
            int temp = randInt(0, 1);
            if(temp == 0)
            {
                setSleepCountToTwo();
                return;
            }
        }
        //step 8
         if(getDistance() == 0)
        {
            //        fprintf(stderr, "Baby grasshopper %p: changing direction\n", this);
            resetDistance();
            setDirection(static_cast<GraphObject::Direction>(randInt(up, left)));
        }
        
        //step 9
        int destX = getX();
        int destY = getY();
        switch (getDirection())
        {
            case GraphObject::up:
                destY++;
                break;
            case GraphObject::right:
                destX++;
                break;
            case GraphObject::down:
                destY--;
                break;
            case GraphObject::left:
                destX--;
                break;
            default:
                break;
        }
        
        //step 11
        if(!thereIsPebble(destX, destY))   // there is no pebble at front
        {
//                    fprintf(stderr, "Baby grasshopper %p: moving because there is no pebble\n", this);
            moveTo(destX, destY);
            m_distance--;
            setSleepCountToTwo();

        }
        //step 10
        else        // if there is pebble at front
        {
            //        fprintf(stderr, "Baby grasshopper %p: changing dir because there is a pebble\n", this);
            m_distance = 0;
            setSleepCountToTwo();
        }
    }
    
    /////////////////////AdultGrasshopper/////////////////////////////////////

bool AdultGrasshopper::canRetaliate(){return true;}
    void AdultGrasshopper::doSomething()
    {
//        fprintf(stderr, "Adult grasshopper %p: currently at (%d,%d), doing something\n", this, getX(), getY());

        //step 1
        loseHitPoint();
        
        //step 2
        if(getHitPoint() <= 0)
        {
            setDead();
            deadBodyTurnToFood(getX(), getY());
            return;
        }
        
        
        //step 3
        if(isSleeping())
        {
//            fprintf(stderr, "Adult Grasshopper %p is sleeping\n", this);
            decrementSleepCount();
            return;
        }
        if(isStunned())
        {
            //fprintf(stderr, "Adult Grasshopper %p is stunned\n", this);
            decrementStunCount();
            return;
        }
        
        //step 4: adult grasshopper is going to do something
        
        //step 5
        int tempStepFive = randInt(0, 2);
        if(tempStepFive == 0)
        {
            std::list<Actor*> getAllEnermy = getStudentWorld()->objectInTheSpotThatCanBeBitten(getX(), getY());
            if(!getAllEnermy.empty())
            {
                std::list<Actor*>::iterator it = getAllEnermy.begin();
                if((*it)->canRetaliate())
                    (*it)->setBitten();
                for (int i = 0; i < 50 ; i++)
                    (*it)->loseHitPoint();
                setSleepCountToTwo();
            }
        }
        
        //step 6
        int tempStepSix = randInt(0, 9);
        if(tempStepSix == 0)
        {
            std::vector<std::pair<int, int>> candidates;
            int oldX = getX(), oldY = getY();
            for (int x = oldX - 10; x <= oldX + 10; ++x)
                for (int y = oldY - 10; y <= oldY + 10; ++y)
                    if (1 <= x && x <= 62 && 1 <= y && y <= 62 && (oldX - x) * (oldX - x) + (oldY - y) * (oldY - y) <= 100 && !thereIsPebble(x, y))
                        candidates.push_back(std::pair<int, int>(x, y));
            if (!candidates.empty()) {
                std::pair<int, int> dest = candidates[randInt(0, (int)candidates.size() - 1)];
                moveTo(dest.first, dest.second);
                setSleepCountToTwo();
                return;
            }
        }
        
        //step 7
        GrassHopper::basicDoSomething();
    }
    
    bool AdultGrasshopper::canBeBitten(){return true;}
void AdultGrasshopper::setBitten(){m_bitten = true;}
bool AdultGrasshopper::isBitten(){return m_bitten;}
    /////////////////////BabyGrasshopper/////////////////////////////////////
    
    bool BabyGrasshopper::canBeBitten(){return true;}
    bool BabyGrasshopper::canBePoisoned(){return true;}
    bool BabyGrasshopper::canBeStunned(){return true;}
    
    void BabyGrasshopper::doSomething()
    {
        //fprintf(stderr, "Baby grasshopper %p: currently at (%d,%d), doing something\n", this, getX(), getY());
        //step 1
        loseHitPoint();
        
        //step 2
        if(getHitPoint() <= 0)
        {
            deadBodyTurnToFood(getX(), getY());
            setDead();
            return;
        }
        //step 3
        if(isSleeping())
        {
          //  fprintf(stderr, "Baby Grasshopper %p is sleeping\n", this);
            decrementSleepCount();
            return;
        }
        if(isStunned())
        {
            //fprintf(stderr, "Baby Grasshopper %p is stunned\n", this);
            decrementStunCount();
            return;
        }
        
        //step 4: baby grasshopper is going to do something
        
        //step 5
        if (getHitPoint() >= 1600)
        {
            setDead();
            std::list<Actor*>& p = getStudentWorld()->accessActorsList(getX(), getY());
            //fprintf(stderr, "Creating new Adult Grasshopper\n");
            p.push_back(new AdultGrasshopper(getX(), getY(), getStudentWorld()));
            return;
        }
        
        GrassHopper::basicDoSomething();
    }

    
    //////////////////////////////////////////////////////////
    
    bool Pebble::isPebble(){return true;}
    
    
    /////////////////////Water/////////////////////////////////////
    
    
bool Water::isDanger(){return true;}
    bool Water::isWater(){return true;}
    void Water::doSomething()
    {
        std::list<Actor*> allObjectsCanBeStunned = getStudentWorld()->objectInTheSpotThatCanBeStunned(getX(), getY());
        std::list<Actor*>::iterator it = allObjectsCanBeStunned.begin();
        while(it != allObjectsCanBeStunned.end())
        {
            (*it)->setStunned();
            it++;
        }
    }
    
    /////////////////////Poison/////////////////////////////////////
    
    bool Poison::isPoision(){return true;}
bool Poison::isDanger(){return true;}
    void Poison::doSomething()
    {
        std::list<Actor*> allObjectsCanBePoisoned = getStudentWorld()->objectInTheSpotThatCanBePoinsoned(getX(), getY());
        std::list<Actor*>::iterator it = allObjectsCanBePoisoned.begin();
        while(it != allObjectsCanBePoisoned.end())
        {
            (*it)->changeHowManyHitPoint(-150);
            it++;
        }
    }
    
    /////////////////////Food/////////////////////////////////////
    
bool Food::isFood(){return true;}
    
    /////////////////////Pheromone/////////////////////////////////////
    
    void Pheromone::doSomething()
    {
        loseHitPoint();
        if(getHitPoint() <= 0)
            setDead();
    }
    
    int Pheromone::getPheromoneNo(){return m_t;}
    
    //////////////////////Anthill////////////////////////////////////
    
    int Anthill::getAntHillNo()
    {
        return m_antHillNo;
    }
    
    void Anthill::doSomething()
    {
        //step 1
        loseHitPoint();
        
        //step 2
        if(getHitPoint() <= 0)
        {
            setDead();
            return;
        }
        
        //step 3
        std::list<Actor*> thereIsFood = getStudentWorld()->getFoodObject(getX(), getY());
        if(!thereIsFood.empty())
        {
            //fprintf(stderr, "Anthill %d has food.\n", getAntHillNo());
            std::list<Actor*>::iterator food = thereIsFood.begin();
            int howMuchFood = (*food)->getHitPoint();
            if( howMuchFood > 10000)
            {
                (*food)->changeHowManyHitPoint(-10000);
                changeHowManyHitPoint(10000);
            }
            else    // there is not enough food in the spot
            {
                changeHowManyHitPoint(howMuchFood);
                (*food)->setDead();
            }
            return;
        }
        
        // step 4
        if (getHitPoint() >= 2000)
        {
            giveBirth(getX(), getY(), getStudentWorld(), getAntHillNo(), m_pointerToMyCompilerObject);
            changeHowManyHitPoint(-1500);
            //       It asks StudentWorld to increase the count of the total number of ants that this
            //        colony has produced. This needs to be tracked in order to determine the winner ant colony.
        }
    }

    void Anthill::giveBirth(int x, int y, StudentWorld* SW, int whichAnthill, Compiler* compilerObject)
    {
        //fprintf(stderr, "Anthill %d is creating a new ant.\n", whichAnthill);
        Ant* newAnt = new Ant(whichAnthill, x, y, getStudentWorld(), compilerObject);
        //fprintf(stderr, "Anthill %d's new ant is %p.\n", whichAnthill, newAnt);
        getStudentWorld()->accessActorsList(x, y).push_back(newAnt);
        getStudentWorld()->increaseAntCountForColony(whichAnthill);
    }
    
    //////////////////////////////////////////////////////////
