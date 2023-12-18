#ifndef Monster_h
#define Monster_h
#include "Arduino.h"
#include "LedControl.h"

extern int cameraX;
extern int cameraY;
extern byte gameMap[16][16];

class Monster {
  public:
    Monster();
    Monster(LedControl* lc, int x = 0, int y = 0);
    void draw(unsigned long blinkRate);
    int getX();
    int getY();
    void go(int target_x, int target_y);
    unsigned long lastMoveTime;
  private:
    int x;
    int y;
    LedControl* ledMatrix;
};

#endif