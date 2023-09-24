#include <VL6180X.h>
#include <Wire.h>
#include <EEPROM.h>
#include "BotPins.h"
#include "BotZlgoVariables.h"
#include "BotVariables.h"
#include "BotTof.h"
#include "BotMotors.h"
#include "BotTurns.h"
#include "BotWallPid.h"
#include "BotSpeed.h"
#include "BotMoves.h"
#include "BotZlgo.h"
#include "BotMazeSolve.h"

void setup()
{   stbyHigh();
    tofSetup();
    motorDiver();
    motorInterrupt();
    pinMode(buzzer, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(PB12), countLeftOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(PB13), countLeftOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(PB14), countRightOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(PB15), countRightOut1, RISING);
    Serial2.begin(9600);
}


void loop()
{
    delay(1000);
    searchStates();
    //eepromClear();
}
