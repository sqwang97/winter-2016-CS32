#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <list>
#include <stack>
#include <queue>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

double radius (int x1, int x2, int y1, int y2)
{
    return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}

bool StudentWorld::checkPosValid(int x, int y, int height, Actor* self) const
{
    if (noDirt(x, y, height) && !isOccupiedByBoulder(x, y, self) && x >= 0 && x <= 60 && y >= 0 && y <= 60)
        return true;
    return false;
}

int StudentWorld::init()
{
    m_time = 0;
    
    int m_level = getLevel();
    m_boulder = min(m_level/2 + 2, 6);
    m_gold = max(5 - m_level/2, 2);
    m_oil = min(2 + m_level, 20);
    m_chanceToAddPorS = m_level*25 + 300;
    int temp = 2+ m_level*1.5;
    m_targetNumOfProtestor = min(15, temp);
    m_addNewProtestor = max(25, 200 - m_level);
    m_probOfHardCore = min(90, m_level * 10 + 30);
    
    //initialize the oil field
    for (int i = 0; i < VIEW_WIDTH; i++){
        for (int r = 0; r < VIEW_HEIGHT; r++){
            if (r < 4){
                m_dirt[i][r] = new Dirt(this, i,r);
                m_isDirt[i][r] = true;
            }
            else if (r >= 60)
                m_isDirt[i][r] = false;
            else if (i < 30 || i > 33){
                m_dirt[i][r] = new Dirt(this, i,r);
                m_isDirt[i][r] = true;
            }
            else
                m_isDirt[i][r] = false;
        }
    }
    
    //initialize FrackMan
    m_player = new FrackMan(this);
    
    //initialize boulder
    for (int i = 0; i < m_boulder; i++){
        int x = rand()%61;
        int y = 20+rand()%37;
        if (checkValid(x,y)){
            m_record.push_back(new Boulder(this, x, y));
            removeDirt(x, y);
        }
        else
            i--;
    }
    //initialize gold
    for (int i = 0; i < m_gold; i++){
        int x = rand()%61;
        int y = rand()%57;
        if (checkValid(x,y))
            m_record.push_back(new Gold(this, x, y));
        else
            i--;
    }
    //initialzie oil barrel
    for (int i = 0; i < m_oil; i++){
        int x = rand()%61;
        int y = rand()%57;
        if (checkValid(x,y))
            m_record.push_back(new Oil(this, x, y));
        else
            i--;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_time++;
    
    /// Update the Game Status Line
    updateDisplayText();
    
    ///Add New Actors
    addNewActors();

    /// Give each Actor a chance to do something
    //FrackMan
    m_player->doSomething();
    removeDirt(m_player->getX(), m_player->getY());
    
    ///init direction Map
    int legalMove = -1;
    posDir(m_dirMap, legalMove);
    
    //Every other actors
    list<Actor*>::iterator it;
    it = m_record.begin();
    
    while (it != m_record.end()){
        if ((*it)->isAlive())
            (*it)->doSomething();
        
        if ((*it)->isWhat() == 'B'){
            if(radius((*it)->getX()+2, m_player->getX()+2, (*it)->getY()+2, m_player->getY()+2) <= 3)
                m_player->setDead();
        }
        
        //check status of FrackMan
        if (!m_player->isAlive()){
            decLives();
            playSound(SOUND_PLAYER_GIVE_UP);
            return GWSTATUS_PLAYER_DIED;
        }
        else if (getOilsLeft() == 0){
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
        it++;
    }
    
    ///Remove Newly Dead Actors
    it = m_record.begin();
    while (it != m_record.end()){
        if (!(*it)->isAlive()){
            if ((*it)->isWhat() == 'T' || (*it)->isWhat() == 'H')
                m_targetNumOfProtestor++;
            delete (*it);
            it = m_record.erase(it);
        }
        it++;
    }
    
    /// return the proper result
    if (!m_player->isAlive()){
        decLives();
        playSound(SOUND_PLAYER_GIVE_UP);
        return GWSTATUS_PLAYER_DIED;
    }
    // If the player has collected all of the Barrels on the level, then
    // return the result that the player finished the level
    if (getOilsLeft() == 0)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    //Player hasn't complete current level and hasn't died
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //delete dirt
    for (int i = 0; i < VIEW_WIDTH; i++){
        for (int r = 0; r < VIEW_HEIGHT; r++){
            if (m_isDirt[i][r]){
                delete m_dirt[i][r];
                m_isDirt[i][r] = false;
            }
        }
    }
    //delete FrackMan
    delete m_player;
    //delete Items
    list<Actor*>::iterator it;
    it = m_record.begin();
    
    while (it != m_record.end()){
        delete *it;
        it = m_record.erase(it);
    }
}

void StudentWorld::addNewSquirt(Actor* squirt)
{
    m_record.push_back(squirt);
}

void StudentWorld::addAGold()
{
    m_record.push_back(new Gold(this, m_player->getX(), m_player->getY(), true, true, false));
}

void StudentWorld::revealHidden(int x, int y)
{
    list<Actor*>::iterator it;
    it = m_record.begin();
    
    while (it != m_record.end()){
        if (radius((*it)->getX()+2, x+2, (*it)->getY()+2, y+2) <= 12 && !(*it)->isVisible())
            (*it)->setVisible(true);
        it++;
    }
}

void StudentWorld::attackProtestor(Actor* squirt, int x, int y)
{
    list<Actor*>::iterator it;
    it = m_record.begin();
    
    bool attacked = false;
    while (it != m_record.end()){
        if (!(*it)->isTotallyAnnoyed()){
            if (radius((*it)->getX()+2, x+2, (*it)->getY()+2, y+2) <= 3){
                (*it)->decHealth();
                if ((*it)->isTotallyAnnoyed() && (*it)->isWhat() == 'T')
                    increaseScore(100);
                else if ((*it)->isTotallyAnnoyed() && (*it)->isWhat() == 'H')
                    increaseScore(250);
                attacked = true;
            }
        }
        it++;
    }
    
    if (attacked)
        squirt->setDead();
}

bool StudentWorld::isGoldHere(Actor* protest, int x, int y)
{
    list<Actor*>::iterator it;
    it = m_record.begin();
    
    while (it != m_record.end()){
        if ((*it)->isWhat() == 'G'){
            if (radius((*it)->getX(), x, (*it)->getY(), y) <= 3){
                (*it)->setDead();
                return true;
            }
        }
        it++;
    }
    return false;
}

bool StudentWorld::isOccupiedByBoulder(int x, int y, Actor* self) const
{
    list<Actor*>::const_iterator it;
    it = m_record.begin();
    
    while (it != m_record.end()){
        if ((*it)->isWhat() == 'B' && (*it) != self)
            if (radius((*it)->getX()+2, x+2, (*it)->getY()+2, y+2) <= 3)
                return true;
        it++;
    }
    return false;
}

void StudentWorld::posDir(int map[][VIEW_HEIGHT], int& legalMove, int sx, int sy, int ex, int ey)
{
    for (int i = 0; i < VIEW_WIDTH; i++)
        for (int r = 0; r < VIEW_HEIGHT; r++)
            map[i][r] = -1;
    
    queue<Coord> Coordqueue;
    Coord start(sx, sy);
    Coordqueue.push(start);
    Coord curr(0,0);
    
    if (ex == -1 && ey == -1)
        map[sx][sy] = RIGHT;
    while (!Coordqueue.empty()){
        curr = Coordqueue.front();
        //cout << curr.x() << " " << curr.y() << endl;
        Coordqueue.pop();
        if (curr.x() == ex && curr.y() == ey){
            legalMove = curr.getDistance();
            return;
        }
        
        if (checkPosValid(curr.x()-1, curr.y()) && map[curr.x()-1][curr.y()] == -1){
            Coordqueue.push(Coord(curr.x()-1, curr.y(),curr.getDistance()+1));
            map[curr.x()-1][curr.y()] = RIGHT;
        }
        if (checkPosValid(curr.x(), curr.y()-1) && map[curr.x()][curr.y()-1] == -1){
            Coordqueue.push(Coord(curr.x(), curr.y()-1, curr.getDistance()+1));
            map[curr.x()][curr.y()-1] = UP;
        }
        if (checkPosValid(curr.x()+1, curr.y()) && map[curr.x()+1][curr.y()] == -1){
            Coordqueue.push(Coord(curr.x()+1, curr.y(), curr.getDistance()+1));
            map[curr.x()+1][curr.y()] = LEFT;
        }
        if (checkPosValid(curr.x(), curr.y()+1) && map[curr.x()][curr.y()+1] == -1){
            Coordqueue.push(Coord(curr.x(), curr.y()+1, curr.getDistance()+1));
            map[curr.x()][curr.y()+1] = DOWN;
        }
    }
}

////Helper function
bool StudentWorld::checkValid(int x, int y) const
{
    if (!isDirt(x, y))
        return false;
    
    if (m_record.empty())
        return true;
    
    list<Actor*>::const_iterator it;
    it = m_record.begin();
    
    while (it != m_record.end()){
        if (radius((*it)->getX(), x, (*it)->getY(), y) <= 6)
            return false;
        it++;
    }
    return true;
}

bool StudentWorld::noDirt(int x, int y, int height) const
{
    for (int i = 0; i < 4; i++)
        for (int r = 0; r < height; r++)
            if (m_isDirt[x+i][y+r])
                return false;
    return true;
}

bool StudentWorld::isDirt(int x, int y) const
{
    for (int i = 0; i < 4; i++)
        for (int r = 0; r < 4; r++)
            if (!m_isDirt[x+i][y+r])
                return false;
    return true;
}


void StudentWorld::removeDirt(int x, int y)
{
    bool dig = false;
    for (int i = 0; i < 4; i++){
        for (int r = 0; r < 4; r++){
            if (m_isDirt[x+i][y+r]){
                dig = true;
                delete m_dirt[x+i][y+r];
                m_isDirt[x+i][y+r] = false;
            }
        }
    }
    if (dig && m_time != 0)
        playSound(SOUND_DIG);
}

void StudentWorld::updateDisplayText()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = m_player->getHealth();
    int squirts = m_player->getWater();
    int gold = m_player->getGold();
    int sonar = m_player->getSonar();
    int oilsLeft = getOilsLeft();
    string s = conversion(score, level, lives, health, squirts, gold, sonar, oilsLeft);
    setGameStatText(s);
}

void StudentWorld::addNewActors()
{
    if (m_time == 1){
        int temp = 1+rand()%100;
        if (temp <= m_probOfHardCore)
            m_record.push_back(new HardCoreProtestor(this));
        else
            m_record.push_back(new RegularProtestor(this));
        m_targetNumOfProtestor--;
    }
    else if (m_time % m_addNewProtestor == 0 && m_targetNumOfProtestor > 0){
        int temp = 1+rand()%100;
        if (temp <= m_probOfHardCore)
            m_record.push_back(new HardCoreProtestor(this));
        else
            m_record.push_back(new RegularProtestor(this));
        m_targetNumOfProtestor--;
    }
    
    if ((rand()% m_chanceToAddPorS) == 1){
        if ((rand()%5) == 1){
            m_record.push_back(new Sonar(this));
            return;
        }
        bool push = false;
        while (push != true){
            int x = rand()%61;
            int y = rand()%61;
            if (checkPosValid(x, y)){
                m_record.push_back(new WaterPool(this, x, y));
                push = true;
                return;
            }
        }
    }
}

string StudentWorld::conversion(int score, int level, int lives, int health, int squirts, int gold, int sonar, int oilsLeft)
{
    string result = "Scr: ";
    stack<char> cStack;
    int count = 0;
    while (score/10 != 0){
        cStack.push((score%10)+48);
        score = score/10;
        count++;
    }
    cStack.push(score+48);
    count++;
    for (int i = count; i < 6; i++)
        result += '0';
    while(!cStack.empty()){
        result += cStack.top();
        cStack.pop();
    }
    
    result += "  Lvl: " + convertIntToString(level);
    
    char a;
    result += "  Lives: ";
    a = lives+48;
    result += a;
    
    result += "  Hlth: " + convertIntToString(health) + "0%";
    result += "  Wtr: " + convertIntToString(squirts);
    result += "  Gld: " + convertIntToString(gold);
    result += "  Sonar: " + convertIntToString(sonar);
    result += "  Oil Left: " + convertIntToString(oilsLeft);
    return result;
}

string StudentWorld::convertIntToString(int n)
{
    string result;
    char a;
    if (n/10 != 0){
        a = (n/10)+48;
        result += a;
        a = (n%10)+48;
        result += a;
    }
    else{
        a = n +48;
        result += " ";
        result += a;
    }
    return result;
}