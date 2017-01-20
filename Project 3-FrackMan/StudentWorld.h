#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class FrackMan;
class Dirt;
class Coord;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
    : GameWorld(assetDir)
    {}
    
    ~StudentWorld()
    {}
    
    virtual int init();

    virtual int move();
    
    virtual void cleanUp();
    
    bool isOccupiedByBoulder(int x, int y, Actor* self=nullptr) const;
    
    bool checkPosValid(int x, int y, int height = 4, Actor* self = nullptr) const;
    
    void revealHidden(int x, int y); //use by sonar
    
    void attackProtestor(Actor* squirt, int x, int y); //use by squirt
    
    bool isGoldHere(Actor* protest, int x, int y); //use by protestor
    
    void posDir(int Map[][VIEW_HEIGHT], int& legalMove, int sx = 60, int sy = 60, int ex = -1, int ey = -1);
//Accessor
    int getOilsLeft() const
    {return m_oil;}
    
    int getBoulderLeft() const
    {return m_boulder;}
    
    int getGold() const
    {return m_gold;}
    
    FrackMan* getPlayer() const
    {return m_player;}

    int getDir(int x, int y) const
    {return m_dirMap[x][y];}
//Mutators
    void decreaseOil()
    {m_oil--;}
    
    void addNewSquirt(Actor* squirt);
    
    void addAGold();
    
private:
    //objects
    std::list<Actor*> m_record;
    FrackMan* m_player;
    Dirt* m_dirt[VIEW_WIDTH][VIEW_HEIGHT];
    int m_dirMap[VIEW_WIDTH][VIEW_HEIGHT];
    //status
    bool m_isDirt[VIEW_WIDTH][VIEW_HEIGHT];
    int m_boulder;
    int m_gold;
    int m_oil;
    int m_time;
    int m_addNewProtestor;
    int m_targetNumOfProtestor;
    int m_chanceToAddPorS;
    int m_probOfHardCore;
    //helper functions
    bool checkValid(int m_x, int m_y) const; //used when placing items
    bool isDirt(int x, int y) const; //use in init()
    bool noDirt(int x, int y, int height) const;
    void addNewActors();
    void removeDirt(int m_x, int m_y);
    void updateDisplayText();
    std::string conversion(int score, int level, int lives, int health, int squirts, int gold, int sonar, int oilsLeft);
    std::string convertIntToString (int n);
};

#endif // STUDENTWORLD_H_
