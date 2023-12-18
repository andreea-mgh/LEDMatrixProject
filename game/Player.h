#ifndef Player_h
#define Player_h
#include "Arduino.h"
#include "LedControl.h"

extern int cameraX;
extern int cameraY;
extern byte gameMap[16][16];
// cannot import const values with extern

class Player {
  public:
    Player(LedControl* lc, int x = 0, int y = 0);
    void moveTo(int x, int y);
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void draw(unsigned long blinkRate);
    int getX();
    int getY();
    int getLives();
    void addLife();
    void removeLife();
    void setLives(int lives);
    unsigned long lastMoveTime;
    unsigned long lastLifeLostTime;
  private:
    int x;
    int y;
    int lives;
    LedControl* ledMatrix;
};

#endif