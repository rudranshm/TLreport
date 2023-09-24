#include <VL6180X.h>
#include <Wire.h>
#include "BotPins.h"
#include "BotVariables.h"
#include "BotTof.h"
#include "BotMotors.h"
#include "BotTurns.h"
#include "BotWallPid.h"
#include "BotSpeed.h"



void setup()
{
    tofSetup();
    motorDiver();
    motorInterrupt();
    attachInterrupt(digitalPinToInterrupt(PB12), countLeftOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(PB13), countLeftOut2, RISING);
    attachInterrupt(digitalPinToInterrupt(PB14), countRightOut1, RISING);
    attachInterrupt(digitalPinToInterrupt(PB15), countRightOut2, RISING);
    Serial2.begin(9600);
    delay(1000);
}

void loop()
{
    if(Serial2.available())
  {
    Val=Serial2.read(); 
  if (Val[0]=='p')
  {
    wallP= float(Val[1:4]);
    Serial2.println(wallP);
  }
  else if (Val[0]=='d')
  {
    wallD= float(Val[1:4]);
    Serial2.println(wallD);
  }
}
