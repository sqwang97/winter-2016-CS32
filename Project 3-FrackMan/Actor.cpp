#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <queue>
using namespace std;

double radius (int x1, int x2, int y1, int y2);

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
bool Actor::moveAStep(Direction dir)
{
    if (dir == left && getWorld()->checkPosValid(getX()-1, getY())){
        moveTo(getX()-1, getY());
        return true;
    }
    else if (dir == right && getWorld()->checkPosValid(getX()+1, getY())){
        moveTo(getX()+1, getY());
        return true;
    }
    else if (dir == up && getWorld()->checkPosValid(getX(), getY()+1)){
        moveTo(getX(), getY()+1);
        return true;
    }
    else if (dir == down && getWorld()->checkPosValid(getX(), getY()-1)){
        moveTo(getX(), getY()-1);
        return true;
    }
    return false;
}

///Frackman
void FrackMan::doSomething()
{
    if (getHealth() == 0){
        setDead();
        return;
    }
    
    int key(0);
    if (getWorld()->getKey(key)){
        switch(key){
            case KEY_PRESS_LEFT:
                if (getDirection() != left)
                    setDirection(left);
                else if (getX()-1 < 0)
                    moveTo(getX(), getY());
                else if (!getWorld()->isOccupiedByBoulder(getX()-1, getY()))
                    moveTo(getX()-1, getY());
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() != right)
                    setDirection(right);
                else if (getX()+1 > 60)
                    moveTo(getX(), getY());
                else if (!getWorld()->isOccupiedByBoulder(getX()+1, getY()))
                    moveTo(getX()+1, getY());
                break;
            case KEY_PRESS_UP:
                if (getDirection() != up)
                    setDirection(up);
                else if (getY()+1 > 60)
                    moveTo(getX(), getY());
                else if (!getWorld()->isOccupiedByBoulder(getX(), getY()+1))
                    moveTo(getX(), getY()+1);
                break;
            case KEY_PRESS_DOWN:
                if (getDirection() != down)
                    setDirection(down);
                else if (getY()-1 < 0)
                    moveTo(getX(), getY());
                else if (!getWorld()->isOccupiedByBoulder(getX(), getY()-1))
                    moveTo(getX(), getY()-1);
                break;
            case 'Z':
            case 'z':
                if (getSonar() >= 1){
                    decreaseSonar();
                    getWorld()->revealHidden(getX(), getY());
                    getWorld()->playSound(SOUND_SONAR);
                }
                break;
            case KEY_PRESS_SPACE:
                if (getWater() >= 1){
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    Actor* squirt = createNewSquirt(getX(), getY(), getDirection());
                    if (squirt != nullptr)
                        getWorld()->addNewSquirt(squirt);
                    decreaseWater();
                }
                else if (getWater() >= 1){
                    decreaseWater();
                }
                break;
            case KEY_PRESS_TAB:
                if(getGold() >= 1){
                    getWorld()->addAGold();
                    decreaseGold();
                }
                break;
            case KEY_PRESS_ESCAPE:
                getWorld()->getPlayer()->setDead();
                break;
            /*
            case 'u':
                getWorld()->advanceToNextLevel();
                break;
            */
        }
    }
}

void FrackMan :: decHealth()
{
    Actor::decHealth();
    if (getHealth() <= 0)
        setDead();
    else
        getWorld()->playSound(SOUND_PLAYER_ANNOYED);
}

Actor* FrackMan::createNewSquirt(int x, int y, Direction dir)
{
    Actor* squirt = nullptr;
    if (dir == up && getWorld()->checkPosValid(x, y+4))
        squirt = new Squirt(getWorld(), x, y+4, dir);
    else if (dir == down && getWorld()->checkPosValid(x, y-4))
        squirt = new Squirt(getWorld(), x, y-4, dir);
    else if (dir == left && getWorld()->checkPosValid(x-4, y))
        squirt = new Squirt(getWorld(), x-4, y, dir);
    else if (dir == right && getWorld()->checkPosValid(x+4, y))
        squirt = new Squirt(getWorld(), x+4, y, dir);
    return squirt;
}

///Boulder
void Boulder::doSomething()
{
    if (!isAlive())
        return;
    if (getWorld()->checkPosValid(getX(), getY()-1, 1, this) && m_time != 0)
        m_time--;
    else if (m_time == 0 && getWorld()->checkPosValid(getX(), getY()-1, 4, this)){
        moveTo(getX(), getY()-1);
    }
    else if (m_time == 0 && !getWorld()->checkPosValid(getX(),getY()-1, 4, this))
        setDead();
    
    if (m_time == 0)
        getWorld()->playSound(SOUND_FALLING_ROCK);
}

///Squirt
void Squirt::doSomething()
{
    if (!isAlive())
        return;
    
    if (isFullyTravelled()){
        setDead();
        return;
    }
    
    if (isNew()){
        setUsed();
        return;
    }
    
    if (moveAStep(getDirection()))
        decDistance();
    else
        setDead();
    
    getWorld()->attackProtestor(this, getX(), getY());
    return;
}
///PickableItem
double PickableItem::getRadius() const
{
    return radius(getWorld()->getPlayer()->getX()+2, getX()+2, getWorld()->getPlayer()->getY()+2, getY()+2);
}

bool PickableItem::turnVisible()
{
    if (isVisible() == false && getRadius() <= 4){
        setVisible(true);
        return true;
    }
    return false;
}

bool PickableItem::isPickedbyFrack()
{
    if (getRadius() <= 3 && isPickableByFrack()){
        if(isWhat() != 'O')
            getWorld()->playSound(SOUND_GOT_GOODIE);
        else
            getWorld()->playSound(SOUND_FOUND_OIL);
        setDead();
        getWorld()->increaseScore(m_score);
        return true;
    }
    return false;
}

void PickableItem::doSomething()
{
    if (!isAlive())
        return;
    
    if (turnVisible())
        return;
    
    if (isPickedbyFrack()){
        if (isWhat() == 'G')
            getWorld()->getPlayer()->increaseGold();
        else if (isWhat() == 'O')
            getWorld()->decreaseOil();
        else if (isWhat() == 'S')
            getWorld()->getPlayer()->addSonar();
        else if (isWhat() == 'P')
            getWorld()->getPlayer()->increaseWater();
        return;
    }
}

///Gold
void Gold::doSomething()
{
    PickableItem::doSomething();
    
    if (isTemp() && getTime() > 0)
        decTime();
    else if (isTemp())
        setDead();
}

///Sonar
Sonar::Sonar(StudentWorld* world, int imageID, int startX, int startY, int score)
:PickableItem(world, imageID, startX, startY, score)
{
    int m_level = getWorld()->getLevel();
    int time = min(100, 300 - 10* m_level);
    setTime(time);
}

void Sonar::doSomething()
{
    PickableItem::doSomething();
    
    if (getTime() == 0)
        setDead();
    else
        decTime();
}

///Protestor
void RegularProtestor::setRestingTime()
{
    int m_level = getWorld()->getLevel();
    m_RestingTime = max(0, 3 - m_level/4);
}

void RegularProtestor::setElapseTime()
{
    int m_level = getWorld()->getLevel();
    m_elapseTime = max(50, 100 - m_level*10);
}

void RegularProtestor::numSquaresToMoveInCurrentDirection()
{
    m_numToMove = 8 + rand()%52;
}

void RegularProtestor::decHealth()
{
    Actor::decHealth();
    
    if (getHealth() <= 0){
        setStateToLeave();
        setAnnoyance();
        setElapseTimeToZero();
    }
    else{
        setElapseTime();
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    }
}

void RegularProtestor::setStateToLeave()
{
    m_leave_the_field = true;
    if (!hasAGold())
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
}

void RegularProtestor::doSomething()
{
    if (isAttacked())
        return;
    
    if (!isAlive())
        return;
    
    if (isRestingTick())
        return;
    
    setRestingTime();
    incNonRestingTime();

    //In leaving status
    if (isInLeaving())
        return;
    
    ///Document the freezing time
    if (isFreezing())
        return;
    
    //non-resting tick
    if (hasAGold()){
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        getWorld()->increaseScore(25);
        setStateToLeave();
        return;
    }
    //non-leaving
    nonLeavingStatus();
}

bool RegularProtestor::isAttacked()
{
    if (getWorld()->isOccupiedByBoulder(getX(), getY()) && !isTotallyAnnoyed()){
        getWorld()->increaseScore(500);
        setStateToLeave();
        setAnnoyance();
        return true;
    }
    return false;
}

bool RegularProtestor::isRestingTick()
{
    if (m_RestingTime != 0 && m_elapseTime != 0){
        m_RestingTime--;
        m_elapseTime--;
        return true;
    }
    else if (m_RestingTime != 0 && m_elapseTime == 0){
        m_RestingTime--;
        return true;
    }
    else if (m_RestingTime == 0 && m_elapseTime != 0){
        m_elapseTime--;
        return true;
    }
    return false;
}

bool RegularProtestor::isInLeaving()
{
    if (m_leave_the_field && getX() == m_startX && getY() == m_startY){
        setDead();
        return true;
    }
    else if (m_leave_the_field){
        automaticMovements(getWorld()->getDir(getX(), getY()));
        return true;
    }
    return false;
}

bool RegularProtestor::isFreezing()
{
    if (m_freezingTime != 0){
        m_freezingTime--;
        return true;
    }

    return false;
}

void RegularProtestor::automaticMovements(int dir)
{
    //cout << dir << endl;
    switch (dir) {
        case UP:
            if (getDirection() != up)
                setDirection(up);
            break;
        case DOWN:
            if (getDirection() != down)
                setDirection(down);
            break;
        case LEFT:
            if (getDirection() != left)
                setDirection(left);
            break;
        case RIGHT:
            if (getDirection() != right)
                setDirection(right);
            break;
    }
    moveAStep(getDirection());
}

void RegularProtestor::nonLeavingStatus()
{
    //cout << "nonRestingTick: " << m_nonRestingTime << "|| lastPerpMove: " << m_lastPerpMoveTime << endl;
    
    double m_radius = radius(getX()+2,getWorld()->getPlayer()->getX()+2,getY()+2,getWorld()->getPlayer()->getY()+2);
        
    if (m_radius <= 4){
        if (!isFacingFrackMan())
            turnToFrackMan();
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->getPlayer()->decHealth();
        setFreezingTime();
        return;
    }
    /// turn to the frackman when the radius is within 4 and attack it within the same tick, document it
    
    if (turnToFrackMan()){
        moveAStep(getDirection());
        setNumSquaresToZero();
        return;
    }
    
    if (m_numToMove <= 0)
        generateNewDirPath();
    if (m_nonRestingTime-m_lastPerpMoveTime >= 200 && makePerpTurn(getDirection())){
        numSquaresToMoveInCurrentDirection();
        m_lastPerpMoveTime = m_nonRestingTime;
    }
    
    m_numToMove--;
    if (!moveAStep(getDirection()))
        generateNewDirPath();
}

bool RegularProtestor::isFacingFrackMan() const
{
    int x1 = getX();
    int x2 = getWorld()->getPlayer()->getX();
    int y1 = getY();
    int y2 = getWorld()->getPlayer()->getY();
    Direction dir = getDirection();
    
    if (dir == left && (x2 <= x1) && abs(y1-y2) < 4)
        return true;
    else if (dir == right && (x2 >= x1) && abs(y1-y2) < 4)
        return true;
    else if (dir == up && (y1 <= y2) && abs(x1-x2) < 4)
        return true;
    else if (dir == down && (y1 >= y2) && abs(x1-x2) < 4)
        return true;
    return false;
}

bool RegularProtestor::turnToFrackMan()
{
    int x1 = getX();
    int x2 = getWorld()->getPlayer()->getX();
    int y1 = getY();
    int y2 = getWorld()->getPlayer()->getY();
    
    if ((x2 < x1) && y1 == y2){  //Player at left
        for (int i = 0; i < x1-x2; i++){
            if (!getWorld()->checkPosValid(x1-i, y1))
                return false;
        }
        setDirection(left);
        return true;
    }
    else if ((x2 > x1) && y1 == y2){ //Player at right
        for (int i = 0; i < x2-x1; i++){
            if (!getWorld()->checkPosValid(x1+i, y1))
                return false;
        }
        setDirection(right);
        return true;
    }
    else if ((y1 < y2) && x1 == x2){ //Player at up
        for (int i = 0; i < y2-y1; i++){
            if (!getWorld()->checkPosValid(x1, y1+i))
                return false;
        }
        setDirection(up);
        return true;
    }
    else if ((y1 > y2) && x1 == x2){ //Player at down
        for (int i = 0; i < y1-y2; i++){
            if (!getWorld()->checkPosValid(x1, y1-i))
                return false;
        }
        setDirection(down);
        return true;
    }
    
    return false;
}

bool RegularProtestor::makePerpTurn(Direction dir)
{
    if (dir == left || dir == right){
        if (getWorld()->checkPosValid(getX(), getY()+1) && !getWorld()->checkPosValid(getX(), getY()-1))
            setDirection(up);
        else if (getWorld()->checkPosValid(getX(), getY()-1)
                 && !getWorld()->checkPosValid(getX(), getY()+1))
            setDirection(down);
        else if (getWorld()->checkPosValid(getX(), getY()-1)
                 && getWorld()->checkPosValid(getX(), getY()+1)){
            int random = rand()%2;
            if (random == 0)
                setDirection(down);
            else
                setDirection(up);
        }
        else
            return false;
    }
    
    if (dir == up || dir == down){
        if (getWorld()->checkPosValid(getX()-1, getY()) && !getWorld()->checkPosValid(getX()+1, getY()))
            setDirection(left);
        else if (getWorld()->checkPosValid(getX()+1, getY())
                 && !getWorld()->checkPosValid(getX()-1, getY()))
            setDirection(right);
        else if (getWorld()->checkPosValid(getX()+1, getY())
                 && getWorld()->checkPosValid(getX()-1, getY())){
            int random = rand()%2;
            if (random == 0)
                setDirection(left);
            else
                setDirection(right);
        }
        else
            return false;
    }
    return true;
}

bool RegularProtestor::hasAGold()
{
    if(getWorld()->isGoldHere(this, getX(), getY()))
        return true;
    return false;
}

void RegularProtestor::generateNewDirPath()
{
    int x = getX();
    int y = getY();
    Direction direct = getDirection();
    
    bool pathExist = false;
    bool moveUp(true), moveDown(true), moveLeft(true), moveRight(true);
    
    while (pathExist != true){
        int dir = rand()%4;
        switch (dir) {
            case UP:
                if (moveUp != false && getWorld()->checkPosValid(x, y+1)){
                    if(direct == left || direct == right)
                        m_lastPerpMoveTime = m_nonRestingTime;
                    setDirection(up);
                    numSquaresToMoveInCurrentDirection();
                    return;
                }
                else
                    moveUp = false;
                break;
            case DOWN:
                if (moveDown != false && getWorld()->checkPosValid(x, y-1)){
                    if(direct == left || direct == right)
                        m_lastPerpMoveTime = m_nonRestingTime;
                    setDirection(down);
                    numSquaresToMoveInCurrentDirection();
                    return;
                }
                else
                    moveDown = false;
                break;
            case LEFT:
                if (moveLeft != false && getWorld()->checkPosValid(x-1, y)){
                    if(direct == up || direct == down)
                        m_lastPerpMoveTime = m_nonRestingTime;
                    setDirection(left);
                    numSquaresToMoveInCurrentDirection();
                    return;
                }
                else
                    moveLeft = false;
                break;
            case RIGHT:
                if (moveRight != false && getWorld()->checkPosValid(x+1, y)){
                    if(direct == up || direct == down)
                        m_lastPerpMoveTime = m_nonRestingTime;
                    setDirection(right);
                    numSquaresToMoveInCurrentDirection();
                    return;
                }
                else
                    moveRight = false;
                break;
        }
    }
}

///HardCore Protestor
void HardCoreProtestor::doSomething()
{
    if (isAttacked())
        return;
    
    if (!isAlive())
        return;
    
    if (isRestingTick())
        return;

    setRestingTime();
    incNonRestingTime();
    
    //In leaving status
    if (isInLeaving())
        return;
    
    if (isFreezing())
        return;

    //non-resting tick
    if (hasAGold()){
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        getWorld()->increaseScore(50);
        setElapseTime();
        return;
    }
    
    int x1 = getX();
    int x2 = getWorld()->getPlayer()->getX();
    int y1 = getY();
    int y2 = getWorld()->getPlayer()->getY();
    double m_radius = radius(x1+2, x2+2, y1+2, y2+2);
    //int moves = getLegalMoves(x2, y2, x1, y1);
    //cout << moves  << endl;
    
    if (m_radius > 4 && getLegalMoves(x2, y2, x1, y1) <= getDistance()){
        //cout << "getHere || " << getProtestDirection(x1, y1) << endl;
        automaticMovements(getProtestDirection(x1, y1));
        return;
    }
    //non-leaving
    nonLeavingStatus();
}

void HardCoreProtestor::setDistance()
{
    m_distance = 16 + getWorld()->getLevel() * 2;
}

int HardCoreProtestor::getLegalMoves(int sx, int sy, int ex, int ey)
{
    int legalMoves = -1;
    getWorld()->posDir(m_protestorMap, legalMoves, sx, sy, ex, ey);
    return legalMoves;
}
