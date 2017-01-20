#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
const int UP    = 0;
const int DOWN  = 1;
const int LEFT  = 2;
const int RIGHT = 3;

class StudentWorld;

class Coord
{
public:
    Coord(int x, int y, int distance = 1):m_x(x), m_y(y), m_distance(distance){}
    
    int x() const
    {return m_x;}
    
    int y() const
    {return m_y;}
    
    int getDistance() const
    {return m_distance;}
    
private:
    int m_x;
    int m_y;
    int m_distance;
};

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, bool visible = true,
          Direction dir = right, unsigned int depth = 0, double size = 1.0, int health = 0)
    :GraphObject(imageID, startX ,startY, dir, size, depth)
    ,m_world(world), m_isAlive(true), m_health(health), m_annoy(false)
    {setVisible(visible);}
    
    virtual ~Actor()
    {}
    
    StudentWorld* getWorld() const
    {return m_world;}
    
    virtual void doSomething() = 0;
    
    virtual char isWhat() = 0;
    
 //Accessor
    int getHealth() const
    {return m_health;}
    
    bool isAlive() const
    {return m_isAlive;}
    
    bool isTotallyAnnoyed() const
    {return m_annoy;}
//Mutator
    virtual void decHealth()
    {m_health -= 2;}
    
    void setDead()
    {m_isAlive = false;}
    
    void setAnnoyance() //is totally annoyed
    {m_annoy = true;}
    
    bool moveAStep(Direction dir);
    
private:
    StudentWorld* m_world;
    bool m_isAlive;
    int m_health;
    bool m_annoy;
};

class FrackMan : public Actor
{
public:
    FrackMan(StudentWorld* world)
    :Actor(world, IID_PLAYER, 30, 60, true, right, 0, 1.0, 10), m_water(5),m_gold(0),m_sonar(1)
    {}
    
    virtual ~FrackMan()
    {}
    
    virtual void doSomething();
    
    virtual char isWhat()
    {return 'F';}
    
    virtual void decHealth();
//Accesor
    int getWater() const
    {return m_water;}
    
    int getGold() const
    {return m_gold;}
    
    int getSonar() const
    {return m_sonar;}
//Mutator
    void increaseWater()
    {m_water += 5;}
    
    void decreaseWater()
    {m_water--;}
    
    void increaseGold()
    {m_gold++;}
    
    void decreaseGold()
    {m_gold--;}
    
    void addSonar()
    {m_sonar++;}
    
    void decreaseSonar()
    {m_sonar--;}

private:
    int m_water;
    int m_gold;
    int m_sonar;
    //Helper function
    Actor* createNewSquirt(int x, int y, Direction dir);
};


class RegularProtestor : public Actor
{
public:
    RegularProtestor(StudentWorld* world, int imageID = IID_PROTESTER, int health = 5)
    :Actor(world, imageID, 60, 60, true, left, 0, 1.0 , health), m_startX(60), m_startY(60),
    m_leave_the_field(false), m_nonRestingTime(0), m_lastPerpMoveTime(0), m_elapseTime(0), m_freezingTime(0)
    {
        setRestingTime();
        numSquaresToMoveInCurrentDirection();
    }
    
    virtual ~RegularProtestor()
    {}
    
    virtual void doSomething();

    virtual char isWhat()
    {return 'T';}
    
    virtual void decHealth();
//Used in Hardcore Protestor
    bool isAttacked();
    
    bool isRestingTick();
    
    bool isFreezing();
    
    bool isInLeaving();
    
    void nonLeavingStatus();
    
    void automaticMovements(int dir);
    
    void setRestingTime();

    bool hasAGold();
//Mutator
    void incNonRestingTime()
    {m_nonRestingTime++;}

    void setNumSquaresToZero()
    {m_numToMove = 0;}
    
    void setElapseTime();
    
    void setElapseTimeToZero()
    {m_elapseTime = 0;}
    
private:
    int m_startX;
    int m_startY;
    //Time Variables
    int m_freezingTime;
    int m_RestingTime; //time between two moves
    int m_elapseTime; //time stopped when hitted by squirt
    int m_nonRestingTime; //non-resting tick
    int m_lastPerpMoveTime; //non-resting tick
    
    int m_numToMove;
    bool m_leave_the_field;
    //Helper function
    bool isFacingFrackMan() const;
    bool turnToFrackMan(); //can directly see Frackman and then turn to Frackman
    bool makePerpTurn(Direction dir); //can make a perpendicualr move or not
    void generateNewDirPath();
    void numSquaresToMoveInCurrentDirection();
    void setStateToLeave();
    void setFreezingTime()
    {m_freezingTime = 15;}
};

class HardCoreProtestor : public RegularProtestor
{
public:
    HardCoreProtestor(StudentWorld* world)
    :RegularProtestor(world, IID_HARD_CORE_PROTESTER, 20)
    {setDistance();}
    
    virtual ~HardCoreProtestor()
    {}
    
    virtual void doSomething();
    
    virtual char isWhat()
    {return 'H';}
    
    void setDistance();
    
    int getDistance() const
    {return m_distance;}
    
    int getProtestDirection(int x, int y) const
    {return m_protestorMap[x][y];}
    
    int getLegalMoves(int sx, int sy, int ex, int ey);
private:
    int m_distance;
    int m_protestorMap[VIEW_WIDTH][VIEW_HEIGHT];
};

class Dirt : public Actor
{
public:
    Dirt(StudentWorld* world, int startX, int startY)
    :Actor(world, IID_DIRT, startX, startY, true, right, 3, 0.25)
    {}
    
    virtual ~Dirt()
    {}
    
    virtual void doSomething()
    {}
    
    virtual char isWhat()
    {return 'D';}
};


class Boulder : public Actor
{
public:
    Boulder(StudentWorld* world, int startX, int startY)
    :Actor(world, IID_BOULDER, startX, startY, true, down, 1), m_time(30)
    {}
    
    virtual ~Boulder()
    {}
    
    virtual void doSomething();
    
    virtual char isWhat()
    {return 'B';}
    
private:
    int m_time;
};


class Squirt : public Actor
{
public:
    Squirt(StudentWorld* world, int startX, int startY, Direction dir)
    :Actor(world, IID_WATER_SPURT, startX, startY, true, dir, 1), m_distance(4),m_new(true)
    {}
    
    virtual ~Squirt()
    {}
    
    virtual void doSomething();
    
    virtual char isWhat()
    {return 'Q';}
//Accessor
    bool isFullyTravelled() const
    {return (m_distance == 0);}
    
    bool isNew() const
    {return m_new;}
//Muatator
    void decDistance()
    {m_distance--;}
    
    void setUsed()
    {m_new = false;}
    
private:
    int m_distance;
    bool m_new;
};


class PickableItem : public Actor
{
public:
    PickableItem(StudentWorld* world, int imageID, int startX, int startY, int score, bool visible = true, bool pickByFrack = true)
    :Actor(world, imageID, startX, startY, visible, right ,2), m_score(score), m_pickByFrack(pickByFrack)
    {}
    
    virtual ~PickableItem()
    {}
    
    virtual void doSomething();
    
    bool isPickedbyFrack();
    
    bool turnVisible();
    
    double getRadius() const;
//Mutator
    void setTime(int time)
    {m_time = time;}
    
    void decTime()
    {m_time--;}
//Accessor
    int getTime() const
    {return m_time;}
    
    bool isPickableByFrack() const
    {return m_pickByFrack;}
    
private:
    int m_score;
    int m_time;
    bool m_pickByFrack;
};


class Gold : public PickableItem
{
public:
    Gold(StudentWorld* world, int startX, int startY, bool temp = false, bool visible = false, bool pickByFrack = true)
    :PickableItem(world, IID_GOLD, startX, startY, 10, false, pickByFrack), m_tempState(temp)
    {setTime(100);}
    
    virtual ~Gold()
    {}
    
    virtual void doSomething();
    
    virtual char isWhat()
    {return 'G';}
//Accessor
    bool isTemp() const
    {return m_tempState;}
    
private:
    bool m_tempState;
};


class Oil : public PickableItem
{
public:
    Oil(StudentWorld* world, int startX, int startY)
    :PickableItem(world, IID_BARREL, startX, startY, 1000, false)
    {}
    
    virtual ~Oil()
    {}
    
    virtual char isWhat()
    {return 'O';}
};


class Sonar : public PickableItem
{
public:
    Sonar(StudentWorld* world, int imageID = IID_SONAR, int startX = 0, int startY = 60, int score = 75);
    
    virtual ~Sonar()
    {}
    
    virtual void doSomething();
    
    virtual char isWhat()
    {return 'S';}
};

class WaterPool : public Sonar
{
public:
    WaterPool(StudentWorld* world, int startX, int startY)
    :Sonar(world, IID_WATER_POOL, startX, startY, 100)
    {}
    
    virtual ~WaterPool()
    {}
    
    virtual char isWhat()
    {return 'P';}
};
#endif // ACTOR_H_
