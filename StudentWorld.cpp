#include "StudentWorld.h"
#include <string>
#include "Compiler.h"
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

int StudentWorld::getCurrentTicks()
{
    return m_tickCount;
}

std::list<Actor*>& StudentWorld::accessActorsList(int x, int y)
{
    return actors[x][y];
}

void StudentWorld::increaseAntCountForColony(int t)
{
    antCount[t]++;
}

int StudentWorld::init()
{
    std::string fieldFileName;
    Field f;
    std::string error;
    std::string fieldFile = getFieldFilename();
    if (f.loadField(fieldFile, error) != Field::LoadResult::load_success)
    {
        setError(fieldFile + " " + error);
        return GWSTATUS_LEVEL_ERROR;
    }
    // something bad happened!
    // otherwise the load was successful and you can access the
    // contents of the field
    
    
    Anthill *ah0, *ah1, *ah2, *ah3;
    
    std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
    setNumberOfColony(fileNames.size());
    for (int l= 0; l < getNumberOfColony(); l++)
        compilerForEntrant[l] = new Compiler;

    // compile the source file... If the compile function returns
    // false, there was a syntax error during compilation!
    for (int j = 0; j < getNumberOfColony(); j++)
    {
        if ( ! compilerForEntrant[j]->compile(fileNames[j], error) ) {
        // entrant 0’s source code had a syntax error!
        // send this error to our framework to warn the user.
        // do it JUST like this!
            setError(fileNames[j] + " " + error);
        // return an error to tell our simulation framework
        // that something went wrong, and it’ll inform the user
        return GWSTATUS_LEVEL_ERROR;
        }
    }
    
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            Field::FieldItem item = f.getContentsOf(i, j);
            switch (item) {
                case Field::FieldItem::rock:
                    actors[i][j].push_back(new Pebble(i, j, this));
                    break;
                case  Field::FieldItem::grasshopper:
                    actors[i][j].push_back(new BabyGrasshopper(i, j, this));
                    break;
                case Field::FieldItem::food:
                    actors[i][j].push_back(new Food(i, j, this));
                    break;
                case Field::FieldItem::anthill0:
                    ah0 = new Anthill(i, j, this, 0, compilerForEntrant[0]);
                    actors[i][j].push_back(ah0);
                    break;
                case Field::FieldItem::anthill1:
                    ah1 = new Anthill(i, j, this, 1, compilerForEntrant[1]);
                    actors[i][j].push_back(ah1);
                    break;
                case Field::FieldItem::anthill2:
                    ah2 = new Anthill(i, j, this, 2, compilerForEntrant[2]);
                    actors[i][j].push_back(ah2);
                    break;
                case Field::FieldItem::anthill3:
                    ah3 = new Anthill(i, j, this, 3, compilerForEntrant[3]);
                    actors[i][j].push_back(ah3);
                    break;
                case Field::FieldItem::water:
                    actors[i][j].push_back(new Water(i, j, this));
                    break;
                case Field::FieldItem::poison:
                    actors[i][j].push_back(new Poison(i, j, this));
                    break;
                default:
                    break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}



int StudentWorld::move(){
    
    if(!theSimulationIsOver())
    {
        // Give each actor a chance to do something
        for (int x = 0; x<VIEW_WIDTH; ++x)
            for (int y = 0; y <VIEW_HEIGHT; ++y)
                for (std::list<Actor*>::iterator i = actors[x][y].begin(), ie = actors[x][y].end(); i != ie; ++i) {
                    (*i)->doSomething();
                }
        
        // after each actor moves, store the moved actors into a list, and erase the moved actors from the old position
        std::list<Actor*> movedActors;
        for (int x = 0; x<VIEW_WIDTH; ++x)
            for (int y = 0; y <VIEW_HEIGHT; ++y)
                for (std::list<Actor*>::iterator i = actors[x][y].begin(); i != actors[x][y].end(); ) {
                    int newX = (*i)->getX(), newY = (*i)->getY();
                    if (newX != x || newY != y) {
                        movedActors.push_back(*i);
                        i = actors[x][y].erase(i);
                    } else ++i;
                }
        //fprintf(stderr, "There are %zu moved actors.\n", movedActors.size());
        
        
        // add the moved actors into the 2D liked list to avoid moved actors being moved multiple times
        for (std::list<Actor*>::iterator i = movedActors.begin(), ie = movedActors.end(); i != ie; ++i) {
            int newX = (*i)->getX(), newY = (*i)->getY();
            actors[newX][newY].push_back(*i);
            //fprintf(stderr, "Moving %p to (%d,%d).\n", *i, newX, newY);
        }
        
        // decrement tick count
        m_tickCount--;
        
        // updateTheDataStructureThatTracksWhereEachActorIs(q,oldX,oldY);
        // Remove newly-dead actors after each tick
        for (int x = 0; x < VIEW_WIDTH; x++){
            for (int y = 0; y < VIEW_HEIGHT; y++)
            {
                std::list<Actor*>::iterator it = actors[x][y].begin();
                while(it != actors[x][y].end())
                {
                    if((*it)->isDead())
                    {
                        //fprintf(stderr, "Removing dead actor %p.\n", *it);
                        delete *it;
                        it = actors[x][y].erase(it);
                    }
                    else it++;
                }
            }
        }
        
        // if there is new actors being created, i.e. ant, adult grasshopper
        std::list<Actor*>::iterator it = newActorStored.begin();
        while(it != newActorStored.end())
        {
            int x = (*it)->getX(), y = (*it)->getY();
            actors[x][y].push_back(*it);
        }
        
        // after adding all newly created actors, remove the list so that the empty list can be used for the next tick
        newActorStored.clear();
        
        // Update the simulation Status Line
        updateDisplayText(); // update the ticks/ant stats text at screen top
        weHaveWinningAnt();
        return GWSTATUS_CONTINUE_GAME;
    }
    
    
//     If the simulation’s over (ticks == 2000) then see if we have a winner
        if (theSimulationIsOver())
        {
            int k = weHaveWinningAnt();
            if (k >= 0)
            {
                setWinner(compilerForEntrant[k]->getColonyName());
                return GWSTATUS_PLAYER_WON;
            }
            else
                return GWSTATUS_NO_WINNER;
        }
        // the simulation is not yet over, continue!
        return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::weHaveWinningAnt()
{
    return m_winningAntNumber;
}

void StudentWorld::cleanUp()
{
    for (int x = 0; x < VIEW_WIDTH; x++)
        for (int y = 0; y < VIEW_HEIGHT; y++)
        {
            std::list<Actor*>::iterator it = actors[x][y].begin();
            while(it != actors[x][y].end())
            {
                delete *it;
                ++it;
            }
            actors[x][y].clear();
        }
    for (int i = 0; i < getNumberOfColony(); ++i)
        delete compilerForEntrant[i];
    newActorStored.clear();
}


bool StudentWorld::theSimulationIsOver()
{
    return getCurrentTicks() <= 0;
}

void StudentWorld::updateDisplayText()
{
    int ticks = m_tickCount;
    for (int i = 0; i < getNumberOfColony(); i++){
        if (antCount[i] > 5 && (m_winningAntNumber < 0 || m_winningAntNumber >= 0 && antCount[i] > antCount[m_winningAntNumber]))
        {
            m_winningAntNumber = i;
        }
    }
    
    std::ostringstream oss;
    oss << "Ticks:" << std::setw(5) << ticks;
    //fprintf(stderr, "updateDisplayText: winning ant %d\n", m_winningAntNumber);
    for(int i = 0; i < getNumberOfColony(); i ++)
    {
        if (i) oss << "  "; else oss << " - ";
        if(m_winningAntNumber != i)
            oss << compilerForEntrant[i]->getColonyName() << ": " << setfill('0') << std::setw(2) << antCount[i];
        else
            oss << compilerForEntrant[i]->getColonyName() << "*: " << setfill('0') << std::setw(2)<<antCount[i];
    }
    setGameStatText(oss.str());
}



// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
