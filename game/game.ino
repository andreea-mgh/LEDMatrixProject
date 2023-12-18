#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "Player.h"
#include "Monster.h"
const byte dinPin = 13;
const byte clockPin = 12;
const byte loadPin = 11;
const byte matrixSize = 8;
LedControl ledMatrix(dinPin, clockPin, loadPin, 1);
const byte lcd_rs = 9;
const byte lcd_en = 8;
const byte lcd_d4 = 7;
const byte lcd_d5 = 6;
const byte lcd_d6 = 5;
const byte lcd_d7 = 4;
const byte lcd_bl = 10;
LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

byte lcdBrightness = 128;
byte lcdBrightnessMax = 248;
byte matrixBrightness = 2;
byte matrixBrightnessMax = 15;

const byte pinX = A0;
const byte pinY = A1;
const byte buttonPin = 2;

const byte buzzPin = 3;

// joystick parameters
const int joystickMinThreshold = 384;
const int joystickMaxThreshold = 640;

// movement speed
unsigned long menuMoveTime = 0;
const unsigned long menuMoveDelay = 250;
const unsigned long moveDelay = 250;
const unsigned long monsterMoveDelay = 400;


const int highScoreAddress = 10;
const int highScoreEntries = 3;
unsigned long levelStartTime = 0;
unsigned long currentScore = 0;
unsigned long highscores[3] = {0, 0, 0};
char playerName[highScoreEntries] = {'A', 'A', 'A'};
char highscoreNames[highScoreEntries][3] = { {'*', '*', '*'} , {'*', '*', '*'} , {'*', '*', '*'} };

bool buttonPressed = false;
unsigned long lastButtonPressTime = 0;
unsigned long debounceDelay = 100;
volatile bool buttonTrigger = false;

bool gameWon = false;
byte currentOption = 0;
byte currentMenu = 0;
byte letterIndex = 3;
bool resetConfirm = false;
byte gameStatus = 0;

// settings
const int playerBlinkRate = 300;
const int monsterBlinkRate = 100;

unsigned long flashStartTime = 0;
const unsigned long flashDuration = 1500;
bool flash = false;

int alertRange = 2;
byte dangerLevel = 0;

int cameraX = 0;
int cameraY = 0;

Player survivor(&ledMatrix, 1, 1);
Monster monsters[5];
int numMonsters = 0;

const byte mapSize = 16;
const byte numLevels = 5;
byte currentLevel = 0;
bool soundsEnabled = true;

const int winBonus = 1000;
const int levelCompleteBonus = 100;
const int lifeBonus = 100;
const unsigned long bonusTimeLimit[numLevels] = {60, 90, 120, 180, 240};


byte lifeChar[8] = {
	0b00000,
	0b00000,
	0b01010,
	0b11111,
	0b11111,
	0b01110,
	0b00100,
	0b00000
};

byte timeChar[8] = {
	0b00000,
	0b00000,
	0b01110,
	0b10101,
	0b10111,
	0b10001,
	0b01110,
	0b00000
};

byte flashChar[8] = {
	0b00000,
	0b00000,
	0b01110,
	0b10101,
	0b10101,
	0b01110,
	0b00000,
	0b00000
};

const uint64_t matrixImages[] = {
  0x6699997e9999a5c3
};

byte gameMap[mapSize][mapSize];

const byte gameLevels[numLevels][mapSize][mapSize] PROGMEM = {
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1 },
    { 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  },
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 2, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 0, 0, 1, 0, 0, 0, 2, 1, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  },
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 2, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 2, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 0, 0, 1, 0, 1, 1, 0, 2, 0, 0, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  },
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 2, 0, 0, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 2, 0, 0, 1, 1, 1, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 2, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  },
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 2, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  }
};

const uint64_t winAnimation[] PROGMEM = {
  0x0008142240800000,
  0x08142a55a2408000
};

const uint64_t loseAnimation[] PROGMEM = {
  0x0022140814220000,
  0x22552a142a552200
};

void loadLevel(int level = 0) {
  if(level == 0) survivor = Player(&ledMatrix, 2, 2);
  else survivor.moveTo(2, 2);
  cameraX = 0;
  cameraY = 0;
  flash = false;
  numMonsters = 0;
  ledMatrix.clearDisplay(0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write((byte)1);
  lcd.setCursor(0, 1);
  lcd.print(F("LV "));
  lcd.print(currentLevel + 1);
  for(int i = 15; i > 15 - survivor.getLives(); i--) {
    lcd.setCursor(i, 0);
    lcd.write(byte(0));
  }


  if(level < numLevels) {
    for(int i = 0; i < mapSize; i++) {
      for(int j = 0; j < mapSize; j++) {
        int tile = pgm_read_byte(&gameLevels[level][i][j]);
        if (tile == 0 || tile == 1) {
          gameMap[i][j] = tile;
        }
        else if(tile == 2) {
          gameMap[i][j] = 0;
          monsters[numMonsters] = Monster(&ledMatrix, j, i);
          numMonsters++;
        }
      }
    }
  }
  levelStartTime = millis();
}

void drawMap(int cameraX, int cameraY) {
  ledMatrix.clearDisplay(0);
  for (int i = cameraY; i < cameraY + matrixSize; i++) {
    for (int j = cameraX; j < cameraX + matrixSize; j++) {
      ledMatrix.setLed(0, i-cameraY, j-cameraX, gameMap[i][j]);
    }
  }
}

void drawMonsters(int cameraX, int cameraY) {
  dangerLevel = 0; // safe

  for(int i = 0; i < numMonsters; i++) {
    if(monsters[i].getX() >= cameraX && monsters[i].getX() < cameraX + matrixSize && monsters[i].getY() >= cameraY && monsters[i].getY() < cameraY + matrixSize) {
      dangerLevel = 2; // danger
      if(millis() - monsters[i].lastMoveTime > monsterMoveDelay) {
        monsters[i].go(survivor.getX(), survivor.getY());
        drawMap(cameraX, cameraY);
      }
      monsters[i].draw(monsterBlinkRate);
    }
  }
}

void checkMonsters(int x, int y) {
  for(int i = 0; i < numMonsters; i++) {
    if(abs(monsters[i].getX() - x) <= 1 && abs(monsters[i].getY() - y) <= 1) {
      survivor.removeLife();
      survivor.lastLifeLostTime = millis();
      lcd.setCursor(15 - survivor.getLives(), 0);
      lcd.write(' ');
      removeMonster(i);
    }
    if(abs(monsters[i].getX() - x) <= alertRange && abs(monsters[i].getY() - y) <= alertRange) {
      dangerLevel = 1; // alert
    }
  }
}

void removeMonster(int i) {
  monsters[i] = monsters[numMonsters - 1];
  numMonsters--;
}

void see(int cameraX, int cameraY, int x, int y) {
  for(int i=-1; i<=1; i++) {
    for(int j=-1; j<=1; j++) {
      if(x+i >= 0 && x+i < mapSize && y+j >= 0 && y+j < mapSize) {
        ledMatrix.setLed(0, y+j-cameraY, x+i-cameraX, gameMap[y+j][x+i]);
      }
    }
  }
}

void displayImage(uint64_t image) {
  // SOURCE: xantorohara.github.io/led-matrix-editor
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      ledMatrix.setLed(0, i, j, bitRead(row, j));
    }
  }
}

void resetHighscores() {
  char name[3] = { '*', '*', '*' };
  for(int i = 0; i < highScoreEntries; i++) {
    EEPROM.put(highScoreAddress + i * (3+sizeof(currentScore)), long(0));
    EEPROM.put(highScoreAddress + i * (3+sizeof(currentScore)) + sizeof(currentScore), name);
    highscores[i] = 0;
    highscoreNames[i][0] = '*';
    highscoreNames[i][1] = '*';
    highscoreNames[i][2] = '*';
  }
}

void addHighscore(int rank) {
  // for(int i = rank + 1; i < highScoreEntries; i++) {
  //   highscores[i] = highscores[i - 1];
  //   highscoreNames[i][0] = highscoreNames[i - 1][0];
  //   highscoreNames[i][1] = highscoreNames[i - 1][1];
  //   highscoreNames[i][2] = highscoreNames[i - 1][2];

  //   EEPROM.put(highScoreAddress + i * (3+sizeof(currentScore)), highscores[i]);
  //   EEPROM.put(highScoreAddress + i * (3+sizeof(currentScore)) + sizeof(currentScore), highscoreNames[i][0]);
  //   EEPROM.put(highScoreAddress + i * (3+sizeof(currentScore)) + sizeof(currentScore) + 1, highscoreNames[i][1]);
  //   EEPROM.put(highScoreAddress + i * (3+sizeof(currentScore)) + sizeof(currentScore) + 2, highscoreNames[i][2]);
  // }
  
  for(int i = highScoreEntries - 1; i > rank; i--) {
    highscores[i] = highscores[i - 1];
    highscoreNames[i][0] = highscoreNames[i - 1][0];
    highscoreNames[i][1] = highscoreNames[i - 1][1];
    highscoreNames[i][2] = highscoreNames[i - 1][2];

    EEPROM.put(highScoreAddress + i * (3 + sizeof(currentScore)), highscores[i]);
    EEPROM.put(highScoreAddress + i * (3 + sizeof(currentScore)) + sizeof(currentScore), highscoreNames[i][0]);
    EEPROM.put(highScoreAddress + i * (3 + sizeof(currentScore)) + sizeof(currentScore) + 1, highscoreNames[i][1]);
    EEPROM.put(highScoreAddress + i * (3 + sizeof(currentScore)) + sizeof(currentScore) + 2, highscoreNames[i][2]);
  }

  highscores[rank] = currentScore;
  highscoreNames[rank][0] = playerName[0];
  highscoreNames[rank][1] = playerName[1];
  highscoreNames[rank][2] = playerName[2];

  EEPROM.put(highScoreAddress + rank * (3 + sizeof(currentScore)), currentScore);
  EEPROM.put(highScoreAddress + rank * (3 + sizeof(currentScore)) + sizeof(currentScore), playerName[0]);
  EEPROM.put(highScoreAddress + rank * (3 + sizeof(currentScore)) + sizeof(currentScore) + 1, playerName[1]);
  EEPROM.put(highScoreAddress + rank * (3 + sizeof(currentScore)) + sizeof(currentScore) + 2, playerName[2]);
}

void buttonPress() {
  static unsigned long interruptTime = 0;
  interruptTime = micros();

  if (interruptTime - lastButtonPressTime > debounceDelay * 1000) {
    buttonPressed = !buttonPressed;
    if (buttonPressed) {
      buttonTrigger = true;
    }
  }
  lastButtonPressTime = interruptTime;
}

void setup() {
  Serial.begin(9600); 

  lcdBrightness = EEPROM.read(0);
  matrixBrightness = EEPROM.read(1);
  soundsEnabled = EEPROM.read(2);
  playerName[0] = EEPROM.read(3);
  playerName[1] = EEPROM.read(4);
  playerName[2] = EEPROM.read(5);

  // read highscores from EEPROM
  for(int i = 0; i < highScoreEntries; i++) {
    highscores[i] = EEPROM.get(highScoreAddress + i * (3+sizeof(currentScore)), highscores[i]);
    highscoreNames[i][0] = EEPROM.get(highScoreAddress + i * (3+sizeof(currentScore)) + sizeof(currentScore), highscoreNames[i][0]);
    highscoreNames[i][1] = EEPROM.get(highScoreAddress + i * (3+sizeof(currentScore)) + sizeof(currentScore) + 1, highscoreNames[i][1]);
    highscoreNames[i][2] = EEPROM.get(highScoreAddress + i * (3+sizeof(currentScore)) + sizeof(currentScore) + 2, highscoreNames[i][2]);
  }

  ledMatrix.shutdown(0, false);
  ledMatrix.setIntensity(0, matrixBrightness);
  ledMatrix.clearDisplay(0);

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPress, CHANGE);

  pinMode(lcd_bl, OUTPUT);
  
  lcd.begin(16, 2);

  lcd.createChar(0, lifeChar);
  lcd.createChar(1, timeChar);
  lcd.createChar(2, flashChar);

  randomSeed(analogRead(A5));

  noTone(buzzPin);
  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///SHADOW ESCAPE"));
  if(soundsEnabled) (buzzPin, 100);
  delay(100);
  analogWrite(lcd_bl, lcdBrightness);

  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///             "));
  if(soundsEnabled) noTone(buzzPin);
  delay(100);

  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///SHADOW ESCAPE"));
  if(soundsEnabled) tone(buzzPin, 100);
  delay(200);

  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///             "));
  if(soundsEnabled) noTone(buzzPin);
  delay(200);

  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///SHADOW ESCAPE"));
  if(soundsEnabled) tone(buzzPin, 100);
  delay(400);

  lcd.setCursor(0, 0);
  lcd.print(F("||||||||||||||||"));
  lcd.setCursor(0, 1);
  lcd.print(F("||||||||||||||||"));
  delay(1000);
  if(soundsEnabled) noTone(buzzPin);

  displayImage(matrixImages[0]);
}

void loop() {

  // MENU
  if(gameStatus == 0) {

    int xValue = analogRead(pinX);
    int yValue = analogRead(pinY);

    // MAIN MENU
    if(currentMenu == 0) {
      lcd.setCursor(0, 1);
      lcd.print(F("///SHADOW ESCAPE"));

      lcd.setCursor(0, 0);
      if(currentOption == 0) {
        lcd.print(F("   START       >"));
        if(buttonTrigger) {
          gameStatus = 1;
          currentLevel = 0;
          loadLevel(currentLevel);
          buttonTrigger = false;
        }
      }
      else if(currentOption == 1) {
        lcd.print(F("<  HIGHSCORES  >"));
        if(buttonTrigger) {
          lcd.clear();
          currentMenu = 1;
          currentOption = 0;
          buttonTrigger = false;
        }
      }
      else if(currentOption == 2) {
        lcd.print(F("<  SETTINGS    >"));
        if(buttonTrigger) {
          lcd.clear();
          currentMenu = 2;
          currentOption = 0;
          buttonTrigger = false;
        }
      }
      else if(currentOption == 3) {
        lcd.print(F("<  HOW TO PLAY >"));
        if(buttonTrigger) {
          lcd.clear();
          currentMenu = 3;
          currentOption = 0;
          buttonTrigger = false;
        }
      }
      else {
        lcd.print(F("<  ABOUT        "));
        if(buttonTrigger) {
          lcd.clear();
          currentMenu = 4;
          currentOption = 0;
          buttonTrigger = false;
        }
      }

      if (millis() - menuMoveTime > menuMoveDelay) {
        if (xValue < joystickMinThreshold && currentOption > 0) {
          lcd.clear();
          currentOption--;
          menuMoveTime = millis();
        }
        if (xValue > joystickMaxThreshold && currentOption < 4) {
          lcd.clear();
          currentOption++;
          menuMoveTime = millis();
        }
      }
    }

    // HIGHSCORES
    else if(currentMenu == 1) {
      if(currentOption < highScoreEntries) {
        lcd.setCursor(0, 0);
        lcd.write('#');
        lcd.print(currentOption + 1);
        lcd.print(' ');
        lcd.print(highscoreNames[currentOption][0]);
        lcd.print(highscoreNames[currentOption][1]);
        lcd.print(highscoreNames[currentOption][2]);
        lcd.setCursor(0, 1);
        lcd.print(F("SCORE: "));
        lcd.print(highscores[currentOption]);
      }
      else {
        lcd.setCursor(0, 0);
        lcd.print(F("<  BACK TO MENU "));
        if(buttonTrigger) {
          currentMenu = 0;
          currentOption = 0;
          buttonTrigger = false;
        }
      }
      if (millis() - menuMoveTime > menuMoveDelay) {
        if (xValue < joystickMinThreshold && currentOption > 0) {
          lcd.clear();
          currentOption--;
          menuMoveTime = millis();
        }
        if (xValue > joystickMaxThreshold && currentOption < highScoreEntries + 1) {
          lcd.clear();
          currentOption++;
          menuMoveTime = millis();
        }
      }
    }

    // SETTINGS
    else if(currentMenu == 2) {
      lcd.setCursor(0, 0);
      if(currentOption == 0) {
        lcd.print(F("   LCD         >"));
        lcd.setCursor(3, 1);
        lcd.print(lcdBrightness);

      }
      else if(currentOption == 1) {
        lcd.print(F("<  MATRIX      >"));
        lcd.setCursor(3, 1);
        lcd.print(matrixBrightness);
      }
      else if(currentOption == 2) {
        lcd.print(F("<  SOUNDS      >"));
        lcd.setCursor(3, 1);
        if(soundsEnabled) lcd.print(F("ON "));
        else lcd.print(F("OFF"));
      }
      else if(currentOption == 3) {
        lcd.print(F("<  PLAYER NAME >"));
        lcd.setCursor(3, 1);
        if(letterIndex == 0 && millis() % 500 < 250) lcd.print(F(" "));
        else lcd.print(playerName[0]);
        if(letterIndex == 1 && millis() % 500 < 250) lcd.print(F(" "));
        else lcd.print(playerName[1]);
        if(letterIndex == 2 && millis() % 500 < 250) lcd.print(F(" "));
        else lcd.print(playerName[2]);
        if(buttonTrigger) {
          if(letterIndex < 3) letterIndex = 3;
          else letterIndex = 0;
          buttonTrigger = false;
        }
      }
      else if(currentOption == 4) {
        lcd.print(F("< RESET SCORES >"));
        if(buttonTrigger) {
          if(resetConfirm) {
            resetHighscores();
            lcd.clear();
            resetConfirm = false;
          }
          else {
            resetConfirm = true;
            lcd.setCursor(0, 1);
            lcd.print(F("  ARE YOU SURE? "));
          }
          buttonTrigger = false;
        }
      }
      else {
        lcd.print(F("<  BACK         "));
        if(buttonTrigger) {
          currentMenu = 0;
          currentOption = 0;
          buttonTrigger = false;
          EEPROM.update(0, lcdBrightness);
          EEPROM.update(1, matrixBrightness);
          EEPROM.update(2, soundsEnabled);
          EEPROM.update(3, playerName[0]);
          EEPROM.update(4, playerName[1]);
          EEPROM.update(5, playerName[2]);
        }
      }

      if (millis() - menuMoveTime > menuMoveDelay) {
        // joy left
        if (xValue < joystickMinThreshold && currentOption > 0) {
          if(currentOption == 3 && letterIndex < 3 && letterIndex > 0) {
            letterIndex = (letterIndex - 1) % 3;
            menuMoveTime = millis();
          }
          else {
            lcd.clear();
            currentOption--;
            menuMoveTime = millis();
          }
        }
        // joy right
        if (xValue > joystickMaxThreshold && currentOption < 5) {
          if(currentOption == 3 && letterIndex < 3) {
            letterIndex = (letterIndex + 1) % 3;
            menuMoveTime = millis();
          }
          else {
            lcd.clear();
            currentOption++;
            menuMoveTime = millis();
          }
        }
        // joy down
        if (yValue < joystickMinThreshold) {
          if(currentOption == 0 && lcdBrightness < lcdBrightnessMax) {
            lcdBrightness+=8;
            analogWrite(lcd_bl, lcdBrightness);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(F("   LCD         >"));
            lcd.setCursor(3, 1);
            lcd.print(lcdBrightness);
            menuMoveTime = millis();
          }
          else if(currentOption == 1 && matrixBrightness < matrixBrightnessMax) {
            matrixBrightness++;
            ledMatrix.setIntensity(0, matrixBrightness);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(F("<  MATRIX      >"));
            lcd.setCursor(3, 1);
            lcd.print(matrixBrightness);
            menuMoveTime = millis();
          }
          else if(currentOption == 2) {
            soundsEnabled = true;
            lcd.setCursor(3, 1);
            lcd.print(F("ON "));
            menuMoveTime = millis();
          }
          else if(currentOption == 3) {
            if(letterIndex < 3) {
              if(playerName[letterIndex] < 'Z') playerName[letterIndex]++;
              else playerName[letterIndex] = 'A';
              menuMoveTime = millis();
            }
          }
        }
        // joy up
        if(yValue > joystickMaxThreshold) {
          if(currentOption == 0 && lcdBrightness > 0) {
            lcdBrightness-=8;
            analogWrite(lcd_bl, lcdBrightness);
            lcd.clear();
            lcd.setCursor(3, 0);
            lcd.print(F("   LCD         >"));
            lcd.setCursor(3, 1);
            lcd.print(lcdBrightness);
            menuMoveTime = millis();
          }
          else if(currentOption == 1 && matrixBrightness > 0) {
            matrixBrightness--;
            ledMatrix.setIntensity(0, matrixBrightness);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(F("<  MATRIX      >"));
            lcd.setCursor(3, 1);
            lcd.print(matrixBrightness);
            menuMoveTime = millis();
          }
          else if(currentOption == 2) {
            soundsEnabled = false;
            lcd.setCursor(3, 1);
            lcd.print(F("OFF"));
            menuMoveTime = millis();
          }
          else if(currentOption == 3) {
            if(letterIndex < 3) {
              if(playerName[letterIndex] > 'A') playerName[letterIndex]--;
              else playerName[letterIndex] = 'Z';
              menuMoveTime = millis();
            }
          }
        }
      }
    }

    // HOW TO PLAY
    else if(currentMenu == 3) {
      if(currentOption == 0) {
        lcd.setCursor(0, 0);
        lcd.print(F("MOVE USING THE  "));
        lcd.setCursor(0, 1);
        lcd.print(F("JOYSTICK        "));
      }
      else if(currentOption == 1) {
        lcd.setCursor(0, 0);
        lcd.print(F("PRESS THE BUTTON"));
        lcd.setCursor(0, 1);
        lcd.print(F("FOR FLASHLIGHT  "));
      }
      else if(currentOption == 2) {
        lcd.setCursor(0, 0);
        lcd.print(F("WHEN LIGHT IS ON"));
        lcd.setCursor(0, 1);
        lcd.print(F("THEY FOLLOW YOU "));
      }
      else if(currentOption == 3) {
        lcd.setCursor(0, 0);
        lcd.print(F("GET TO THE EXIT "));
        lcd.setCursor(0, 1);
        lcd.print(F("IN ONE PIECE    "));
      }
      else if(currentOption == 4) {
        lcd.setCursor(0, 0);
        lcd.print(F(" CLICK TO RETURN"));
        lcd.setCursor(0, 1);
        lcd.print(F("TO THE MAIN MENU"));
      }
      
      if(buttonTrigger) {
        if(currentOption < 4) {
          currentOption ++;
        }
        else {
          currentMenu = 0;
          currentOption = 0;
        }
        buttonTrigger = false;
      }
    }

    // ABOUT
    else if(currentMenu == 4) {
      if(currentOption == 0) {
        lcd.setCursor(0, 0);
        lcd.print(F("Shadow Escape  "));
        lcd.setCursor(0, 1);
        lcd.print(F("               "));
      }
      else if(currentOption == 1) {
        lcd.setCursor(0, 0);
        lcd.print(F("Author: Andreea"));
        lcd.setCursor(0, 1);
        lcd.print(F("     Megherlich"));
      }
      else if(currentOption == 2) {
        lcd.setCursor(0, 0);
        lcd.print(F("GitHub:         "));
        lcd.setCursor(0, 1);
        lcd.print(F("     andreea-mgh"));
      }
      else if(currentOption == 3) {
        lcd.setCursor(0, 0);
        lcd.print(F(" Introduction to"));
        lcd.setCursor(0, 1);
        lcd.print(F(" Robotics - 2023"));
      }
      else if(currentOption == 4) {
        lcd.setCursor(0, 0);
        lcd.print(F(" CLICK TO RETURN"));
        lcd.setCursor(0, 1);
        lcd.print(F("TO THE MAIN MENU"));
      }
      
      if(buttonTrigger) {
        if(currentOption < 4) {
          currentOption ++;
        }
        else {
          currentMenu = 0;
          currentOption = 0;
        }
        buttonTrigger = false;
      }
    }
  }



  // GAME
  else if(gameStatus == 1) {
    // sound events
    if(soundsEnabled) {
      if(millis() - levelStartTime < 100) tone(buzzPin, 440);
      else if(millis() - levelStartTime < 400) tone(buzzPin, 587);
      else if(dangerLevel == 2) tone(buzzPin, 880);
      else if(dangerLevel == 1) tone(buzzPin, 80);
      else if(millis() - survivor.lastLifeLostTime < 200) tone(buzzPin, 440);
      else if(millis() - survivor.lastMoveTime < 50) tone(buzzPin, 100);
      else noTone(buzzPin);
    }

    // player movement
    int xValue = analogRead(pinX);
    int yValue = analogRead(pinY);
    bool playerMoved = false;
    if (millis() - survivor.lastMoveTime > moveDelay) {
      // left
      if (xValue < joystickMinThreshold) {
        if (survivor.getX() > 0) {
          if (gameMap[survivor.getY()][survivor.getX() - 1] == 0) {
            playerMoved = true;
            survivor.moveLeft();
            survivor.lastMoveTime = millis();
          }
        }
      }
      // right
      if (xValue > joystickMaxThreshold) {
        if (survivor.getX() < mapSize) {

          if (gameMap[survivor.getY()][survivor.getX() + 1] == 0) {
            playerMoved = true;
            survivor.moveRight();
            survivor.lastMoveTime = millis();
          }
        }
      }
      // up
      if (yValue < joystickMinThreshold) {
        if (survivor.getY() > 0) {

          if (gameMap[survivor.getY() - 1][survivor.getX()] == 0) {
            playerMoved = true;
            survivor.moveUp();
            survivor.lastMoveTime = millis();
          }
        }
      }
      // down
      if (yValue > joystickMaxThreshold) {
        if (survivor.getY() < mapSize) {

          if (gameMap[survivor.getY() + 1][survivor.getX()] == 0) {
            playerMoved = true;
            survivor.moveDown();
            survivor.lastMoveTime = millis();
          }
        }
      }
    }

    // camera movement
    if(survivor.getX() - cameraX > 5 && cameraX < mapSize - matrixSize) {
      cameraX = survivor.getX() - 5;
    }
    else if(survivor.getY() - cameraY > 5 && cameraY < mapSize - matrixSize) {
      cameraY = survivor.getY() - 5;
    }
    else if(survivor.getX() - cameraX < 2 && cameraX > 0) {
      cameraX = survivor.getX() - 2;
    }
    else if(survivor.getY() - cameraY < 2 && cameraY > 0) {
      cameraY = survivor.getY() - 2;
    }

    // redraw stuff if player moved
    if(playerMoved) {
      ledMatrix.clearDisplay(0);
      if(flash) {
        drawMap(cameraX, cameraY);
        drawMonsters(cameraX, cameraY);
      }
      else {
        see(cameraX, cameraY, survivor.getX(), survivor.getY());
      }
    }

    // flash
    if(buttonTrigger) {
      drawMap(cameraX, cameraY);
      drawMonsters(cameraX, cameraY);
      flash = true;
      flashStartTime = millis();
      buttonTrigger = false;
    }
    if(flash) {
      drawMonsters(cameraX, cameraY);
      if(millis() - flashStartTime > flashDuration) {
        flash = false;
        ledMatrix.clearDisplay(0);
      }
    }
    else {
      dangerLevel = 0;
      checkMonsters(survivor.getX(), survivor.getY());
    }

    // win conditions + scoring
    if(survivor.getX() == mapSize - 2 && survivor.getY() == mapSize - 2) {
      currentScore += survivor.getLives() * lifeBonus;
      if((millis() - levelStartTime) / 1000 < bonusTimeLimit[currentLevel]) {
        unsigned long timeBonus = bonusTimeLimit[currentLevel] - (millis() - levelStartTime) / 1000;
        currentScore += timeBonus;
      }

      if(currentLevel < numLevels - 1) {
        currentScore += levelCompleteBonus * (currentLevel + 1);
        currentLevel ++;
        loadLevel(currentLevel);
      }
      else {

        gameStatus = 2;
        currentMenu = 0;
        gameWon = true;
      }
    }

    // lose conditions
    if(survivor.getLives() == 0) {
      gameStatus = 2;
      currentMenu = 0;
      gameWon = false;
    }
    
    // draw player
    survivor.draw(playerBlinkRate);

    // lcd
    lcd.setCursor(0, 0);
    lcd.write(byte(1));
    lcd.setCursor(2, 0);
    lcd.print((millis() - levelStartTime) / 60000);
    lcd.print(F(":"));
    lcd.print(((millis() - levelStartTime) / 1000) % 60 / 10);
    lcd.print(((millis() - levelStartTime) / 1000) % 60 % 10);

    lcd.setCursor(5, 1);
    if(dangerLevel == 2) {
      lcd.print(F("DANGER     "));
    }
    else if(dangerLevel == 1) {
      lcd.print(F("WARNING    "));
    }
    else {
      lcd.print("           ");
      lcd.setCursor(5, 1);
      lcd.print(currentScore);
    } 

    if(gameStatus == 2) {
      buttonTrigger = false;
      displayImage(matrixImages[0]);
      if(gameWon) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print(F("YOU ESCAPED."));
        lcd.setCursor(0, 1);
        currentScore += winBonus;
        lcd.print(F("/// SCORE: "));
        lcd.print(currentScore);
      }
      else {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print(F("YOU DIED."));
        lcd.setCursor(0, 1);
        lcd.print(F("/// SCORE: "));
        lcd.print(currentScore);
      }
      levelStartTime = millis();
    }

  }



  // GAME OVER
  else if(gameStatus == 2) {
    noTone(buzzPin);
    if(currentMenu == 0) {
      if(buttonTrigger) {
        buttonTrigger = false;
        currentMenu = 1;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F(" CLICK TO RETURN"));
        lcd.setCursor(0, 1);
        lcd.print(F("TO THE MAIN MENU"));

        // calculate high scores
        for(int i = 0; i < highScoreEntries; i++) {
          if(currentScore > highscores[i]) {
            addHighscore(i);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(F("YOU ARE IN THE"));
            lcd.setCursor(0, 1);
            lcd.print(F("TOP "));
            lcd.print(i + 1);
            break;
          }
        }

      }
    }

    else if (currentMenu == 1) {
      if(buttonTrigger) {
        buttonTrigger = false;
        currentMenu = 0;
        currentOption = 0;
        gameStatus = 0;
        currentLevel = 0;
        currentScore = 0;
      }
    }
    
  }

}