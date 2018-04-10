#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Field.h"
#include "Actor.h"
#include <string>
#include <list>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp


class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir)
    : GameWorld(assetDir), m_tickCount(2000), antCount{0,0,0,0}, m_winningAntNumber(-999)
    {}
    ~StudentWorld(){}
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    
    
    int getCurrentTicks();
    void increaseAntCountForColony(int t);
    void updateDisplayText();
    bool theSimulationIsOver();
    void clearThereIs();
    int weHaveWinningAnt();
    std::list<Actor*>& accessActorsList(int x, int y);
    
    std::list<Actor*> objectInTheSpotThatCanBeBitten(int x, int y)
    {
        std::list<Actor*> get = actors[x][y];
        std::list<Actor*>::iterator it = get.begin();
        while(it != get.end())
        {
            if (!(*it)->canBeBitten())
                it = get.erase(it);
            else
                ++it;
        }
        return get;
    }
    
    std::list<Actor*> objectInTheSpotThatCanBePoinsoned(int x, int y)
    {
        std::list<Actor*> get = actors[x][y];
        std::list<Actor*>::iterator it = get.begin();
        while(it != get.end())
        {
            if (!(*it)->canBePoisoned())
                it = get.erase(it);
            else
                ++it;
        }
        return get;
    }
    
    std::list<Actor*> objectInTheSpotThatCanBeStunned(int x, int y)
    {
        std::list<Actor*> get = actors[x][y];
        std::list<Actor*>::iterator it = get.begin();
        while(it != get.end())
        {
            if (!(*it)->canBeStunned())
                it = get.erase(it);
            else
                ++it;
        }
        return get;
    }
    
    std::list<Actor*> getFoodObject(int x, int y)
    {
        std::list<Actor*> get = actors[x][y];
        std::list<Actor*>::iterator it = get.begin();
        while(it != get.end())
        {
            if (!(*it)->isFood())
                it = get.erase(it);
            else
                ++it;
        }
        return get;
    }
    
    std::list<Actor*> getPheromone(int x, int y)
    {
        std::list<Actor*> get = actors[x][y];
        std::list<Actor*>::iterator it = get.begin();
        while(it != get.end())
        {
            if ((*it)->getPheromoneNo() < 0 && (*it)->getPheromoneNo() > 3)
                it = get.erase(it);
            else
                ++it;
        }
        return get;
    }
    
    void setNumberOfColony(unsigned long n)
    {
        m_numberOfColony = n;
    }
    
    unsigned long getNumberOfColony()
    {
        return m_numberOfColony;
    }
    
private:
    std::list<Actor*> actors[VIEW_HEIGHT][VIEW_WIDTH];
    std::list<Actor*> newActorStored;
    int antCount[4];
    Compiler* compilerForEntrant[4];
    unsigned long m_numberOfColony;
    int m_winningAntNumber;
    int m_tickCount;
};

#endif // STUDENTWORLD_H_
