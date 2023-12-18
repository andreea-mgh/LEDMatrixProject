#include "Arduino.h"
#include "LedControl.h"
#include "Monster.h"

Monster::Monster() {}

Monster::Monster(LedControl* lc, int x = 0, int y = 0) {
  this->x = x;
  this->y = y;
  ledMatrix = lc;
  this->lastMoveTime = millis();
}

void Monster::draw(unsigned long blinkRate) {
  if(millis() % blinkRate < blinkRate / 2){
    ledMatrix->setLed(0, y - cameraY, x - cameraX, false);
  } else {
    ledMatrix->setLed(0, y - cameraY, x - cameraX, true);
  }
}

void Monster::go(int target_x, int target_y) {

  if(target_x > x && gameMap[y][x+1] == 0) {
    x++;
  }
  else if(target_x < x && gameMap[y][x-1] == 0) {
    this->lastMoveTime = millis();
    x--;
  }
  if(target_y > y && gameMap[y+1][x] == 0) {
    this->lastMoveTime = millis();
    y++;
  }
  else if(target_y < y && gameMap[y-1][x] == 0) {
    this->lastMoveTime = millis();
    y--;
  }
}

int Monster::getX() {
  return x;
}

int Monster::getY() {
  return y;
}