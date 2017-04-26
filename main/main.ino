//---------------- GLOBALS ----------------

#define _DEBUG 1
/* 0: semmi
   1: soros
   2: bt
   4: flash
   összegekkel több is megy egyszerre
*/
uint8_t debugMode = 3;
/* 0: semmi
   1: mozgás
   2: szenzorok
   4: állapot és paraméterek
   8: térkép
   16: pálya
   összegekkel több is megy egyszerre
*/
uint8_t outputMode = 8;

//map size (32 for competition)
#define mapsize 5

#define infraPin 22
#define batteryPin A14
#define led0 13
#define gombPin 21
#define led1 2
#define led2 23

//timer
IntervalTimer myTimer;
//timer frequency and interval (microsec)
#define timerFrequency 250
const int32_t myinterval = 1000000 / timerFrequency;

//Setting pwm output parameters
#define pwmRes 10
const int32_t pwmMax = pow(2, pwmRes);
const int32_t pwmMid = pwmMax / 2;

//setpoint for pid control
volatile int32_t setPoint = 0;

//infra sensor values and TOF
volatile int32_t infra[5] = {0, 0, 0, 0, 0};
volatile int32_t pastinfra[5] = {0, 0, 0, 0, 0};
volatile int32_t infra_deriv[5] = {0, 0, 0, 0, 0};
volatile int32_t TOFread = 0;

//overflower variable
uint16_t overFloop = 0;
uint16_t overFirpt = 0;

//Variable to read command from serial
String serialCommand = "";
//Paramteres
volatile char state = 'S';
volatile int32_t param1 = 0, param2 = 0, param3 = 0, param4 = 0;

//time measure
elapsedMillis elapsedTime = 0;
elapsedMillis delayTimer = 0;
elapsedMicros _elapsedMicro = 0;

//Variable for serial output
String serialop = "";

bool jobboldali = false;
bool baloldali = false;

//Variables for position in a 2D matrix
//0;0 a kiindulási pont
volatile int8_t posX = 0;
volatile int8_t posY = 0;
volatile int8_t savedPosX = 0;
volatile int8_t savedPosY = 0;
//A cella közepéről induljon
bool midzone = true;

//0 az előre, -3től 4-ig irányt mutat
/*    7  0  1
      6     2
      5  4  3
*/
volatile uint8_t orientation = 0;
volatile int16_t relativeAngleError = 0;


//---------------- INCLUDES ----------------

#include "encoderReader.h"
#include "motorControl.h"
#include "infra.h"
#include "motorAutomation.h"
#include "position2D.h"
#include "myFunctions.h"
#include "walls.h"
#include "states.h"
#include "debug.h"



//---------------- SETUP ----------------
void setup()
{
  //Initialize Serial comm
#if _DEBUG
  if (debugMode % 2)
    Serial.begin(115200);
  if ((debugMode >> 1) % 2)
    Serial3.begin(115200);
#endif
  //Initialize I2C (for TOF and MPU)
  //Wire.begin();
  //Analog frekvencia
  analogWriteFrequency(motorLeft, 35156.25);
  analogWriteFrequency(motorRight, 35156.25);
  //Analog 10 biten
  analogWriteResolution(pwmRes);
  //Initialize Motors
  SetupMotors();
  //Start timer
  myTimer.begin(onTimerTick, myinterval);
  //infra
  pinMode(infraPin, OUTPUT);
  digitalWrite(infraPin, 0);
  //button
  pinMode(gombPin, INPUT_PULLUP);
  //leds
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  //Initialize TOF sensor
  //SetupTOF();
  //first infra value
  ReadInfra();
  ReadInfra();
}

//---------------- LOOP ----------------
void loop()
{
  delayTimer = 0;
#if DEBUG
  serialToValue();
  displayData();

  digitalWrite(led2, baloldali);
  digitalWrite(led1, jobboldali);
#endif
  checkBattery();
  if (state != 'O')
    ReadInfra();
  if (!digitalRead(gombPin))
    state = 'T';
  overFloop++;
  while (delayTimer < 5);
}

void checkBattery()
{
  if (analogRead(batteryPin) < 800)
  {
    digitalWrite(led0, 1);
    if (analogRead(batteryPin) < 780)
      state = 'O';
  }
  else
    digitalWrite(led0, 0);
}

//---------------- TIMER INTERRUPT ----------------
void onTimerTick()
{
  jobboldali = false;
  baloldali = false;
  switch (state)
  {
    case 'T':
      stateT();
      break;
    case 'W':
      stateW();
      break;
    case 'C':
      stateC();
      break;
    case 'R':
      stateR();
      break;
    case 'V':
      stateV();
      break;
    case 'S':
      stateS();
      break;
    case 'I':
      stateI();
      break;
    case 'D':
      stateD();
      break;
    case 'O':
      stateS();
      break;
    default:
      state = 'E';
  }
  overFirpt++;
}

