#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cassert>
#include "Compiler.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


class Compiler;
class StudentWorld;
class Command;


class Actor: public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, StudentWorld* SW, Direction dir = right, int depth = 2)
    : GraphObject(imageID, startX, startY), m_SW(SW){};
    virtual ~Actor(){}
    virtual void doSomething();
    virtual bool isDead();
    virtual int getHitPoint(){return -999;}
    virtual void gainHitPoint();
    virtual void loseHitPoint();
    virtual void changeHowManyHitPoint(int n);
    virtual void setDead();
    virtual void setStunned();
    virtual void setBitten();
    virtual bool isStunned();
    
    
    virtual bool canBeBitten();
    virtual bool canBePoisoned();
    virtual bool canBeStunned();
    virtual bool canRetaliate();
    
    virtual bool isFood();
    virtual bool isPoision();
    virtual bool isWater();
    virtual int getAntNo();
    virtual int getPheromoneNo();
    virtual bool isPebble();
    virtual int getAntHillNo();
    virtual bool isDanger();
    
    bool thereIsPebble(int x, int y);
    
    StudentWorld* getStudentWorld()const {return m_SW;}
    
private:
    StudentWorld* m_SW;
};

//////////////////////////////////////////////////////////

class EnergyBasedClass: public Actor
{
public:
    EnergyBasedClass(int imageID, int startX, int startY, StudentWorld* SW, Direction dir, int depth, int hit_point)
    : Actor(imageID, startX, startY, SW, dir, depth), m_hit_point(hit_point){}
    virtual ~EnergyBasedClass(){}
    
    
    virtual bool isDead();
    int getHitPoint();
    virtual void setDead();
    virtual void gainHitPoint();
    virtual void loseHitPoint();
    virtual void changeHowManyHitPoint(int n);
private:
    int m_hit_point;
};

//////////////////////////////////////////////////////////

class Insect: public  EnergyBasedClass
{
public:
    Insect(int imageID, int startX, int startY, StudentWorld* SW, int hit_point)
    : EnergyBasedClass(imageID, startX, startY, SW, static_cast<GraphObject::Direction>(randInt(up, left)), 1, hit_point), m_isStunned(0),m_isStunnedHere(false){}
    virtual ~Insect(){}
    
    virtual bool isStunned();
    virtual bool canBeBitten();
    void deadBodyTurnToFood(int x, int y);
    virtual void setStunned();
    void moveTo(int x, int y) {
        GraphObject::moveTo(x, y);
        m_isStunnedHere = false;
    }
    void decrementStunCount()
    {
        m_isStunned--;
    }
    
private:
    int m_isStunned; bool m_isStunnedHere;
};

//////////////////////////////////////////////////////////

class Ant: public Insect
{
public:
    Ant(int t, int startX, int startY, StudentWorld* SW, Compiler* compilerP)
    : Insect(getIIDForType(t), startX, startY, SW, 1500),
    m_antHillNo(t), m_food(0), m_isBitten(false), m_moved (false), m_isPreviouslyBlocked(false), m_compiler(compilerP), m_ic(0) {}
    static int getIIDForType(int t) {
        switch(t){
            case 0:
                return IID_ANT_TYPE0;
            case 1:
                return IID_ANT_TYPE1;
            case 2:
                return IID_ANT_TYPE2;
            case 3:
                return IID_ANT_TYPE3;
            default:
                assert(false && "invalid ant type");
        }
    }
    virtual ~Ant(){}
    virtual void doSomething();
    virtual int getAntNo();
    
    bool isBitten();
    bool moved();
    bool isPreviouslyBlocked();
    
    virtual bool canBeBitten();
    virtual bool canBePoisoned();
    virtual bool canBeStunned();
    

    
    //movement command
    void moveTheAntForward();
    void eatFood();
    void dropFood();
    void bite();
    void pickupFood();
    void emitPheromone();
    void faceRandomDirection();
    void rotateClockwise();
    void rotateCounterClockwise();
    void generateRandomNumber(int n);
    void getNextPosition(int &x, int &y);
    
    
    
    
    bool runCommand(const Compiler::Command& c);
    bool if_command(Compiler::Condition condition);
    
    
    
private:
    int m_food;
    int m_antHillNo;
    bool m_isBitten;
    bool m_moved;
    bool m_isPreviouslyBlocked;
    Compiler* m_compiler;
    int m_ic;
    int m_lastRandomNumberGenerated;
};

//////////////////////////////////////////////////////////

class GrassHopper: public Insect
{
public:
    GrassHopper(int imageId, int startX, int startY, StudentWorld* SW, int hit_point)
    : Insect(imageId, startX, startY, SW, hit_point), sleepCount(0),m_distance(randInt(2, 10)){}
    virtual ~GrassHopper(){}
    
    bool isSleeping();
    void setSleepCountToTwo();
    void decrementSleepCount();
    void basicDoSomething();
    void resetDistance();
    int getDistance();
    virtual bool isDanger();
    
    
private:
    int sleepCount;
    int m_distance;
};

//////////////////////////////////////////////////////////

class AdultGrasshopper: public GrassHopper
{
public:
    AdultGrasshopper(int startX, int startY, StudentWorld* SW)
    : GrassHopper(IID_ADULT_GRASSHOPPER, startX, startY, SW, 1600), m_bitten(false){}
    virtual ~AdultGrasshopper(){}
    virtual void doSomething();
    
    virtual bool canBeBitten();
    void setBitten();
    bool isBitten();
    virtual bool canRetaliate();
    
private:
    bool m_bitten;
};

//////////////////////////////////////////////////////////

class BabyGrasshopper: public GrassHopper
{
public:
    BabyGrasshopper(int startX, int startY, StudentWorld* SW)
    : GrassHopper(IID_BABY_GRASSHOPPER, startX, startY, SW, 500){}
    virtual ~BabyGrasshopper(){}
    virtual void doSomething();

    
    virtual bool canBeBitten();
    virtual bool canBePoisoned();
    virtual bool canBeStunned();
};

//////////////////////////////////////////////////////////

class Pebble: public Actor
{
public:
    Pebble(int startX, int startY, StudentWorld* SW)
    : Actor(IID_ROCK, startX, startY, SW, right, 1){};
    virtual ~Pebble(){}
    
    virtual bool isPebble();
};

//////////////////////////////////////////////////////////

class Water: public Actor
{
public:
    Water(int startX, int startY, StudentWorld* SW)
    :Actor(IID_WATER_POOL, startX, startY, SW){}
    virtual ~Water(){}
    virtual void doSomething();
    virtual bool isWater();
    virtual bool isDanger();

};

//////////////////////////////////////////////////////////

class Poison: public Actor
{
public:
    Poison(int startX, int startY, StudentWorld* SW)
    :Actor(IID_POISON, startX, startY, SW){}
    virtual ~Poison(){}
    virtual void doSomething();
    virtual bool isPoision();
    virtual bool isDanger();

};

//////////////////////////////////////////////////////////

class Food: public EnergyBasedClass
{
public:
    Food(int startX, int startY, StudentWorld* SW, int hit_point = 6000)      // when an actor dies, a 100 hit_point food object needs to be created
    : EnergyBasedClass(IID_FOOD, startX, startY, SW, right, 2, hit_point = 6000){}
    virtual ~Food(){}
    virtual bool isFood();

};

//////////////////////////////////////////////////////////


class Pheromone: public EnergyBasedClass
{
public:
    Pheromone(int t, int startX, int startY, StudentWorld* SW)
    : EnergyBasedClass(getIIDForType(t), startX, startY, SW, right, 2, 256), m_t(t){}
    static int getIIDForType(int t) {
        switch (t) {
            case 0:
                return IID_PHEROMONE_TYPE0;
            case 1:
                return IID_PHEROMONE_TYPE1;
            case 2:
                return IID_PHEROMONE_TYPE2;
            case 3:
                return IID_PHEROMONE_TYPE3;
            default:
                assert(false && "invalid ant type");
        }
    }
    virtual ~Pheromone(){}
    

    virtual void doSomething();
    virtual int getPheromoneNo();
    
private:
    int m_t;
};

//////////////////////////////////////////////////////////

class Anthill: public EnergyBasedClass
{
public:
    Anthill(int startX, int startY, StudentWorld* SW, int antHillNo, Compiler* compilerObject)
    : EnergyBasedClass(IID_ANT_HILL, startX, startY, SW, right, 2, 8999), m_antHillNo(antHillNo), m_pointerToMyCompilerObject(compilerObject){}
    virtual ~Anthill(){}
    virtual void doSomething();
    void giveBirth(int x, int y, StudentWorld* SW, int whichAnthill, Compiler* compilerObject);
    virtual int getAntHillNo();
    
private:
    int m_antHillNo;
    Compiler* m_pointerToMyCompilerObject;
};

//////////////////////////////////////////////////////////

#endif // ACTOR_H_

