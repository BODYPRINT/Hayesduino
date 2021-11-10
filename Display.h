#ifndef _DISPLAY_H
#define _DISPLAY_H

//Display code disabled as incomplete
//Can be enabled for XC4630 displays
//to test current gui.
//enable below with USE_DISPLAY 1
// https://www.jaycar.com.au/240x320-lcd-touch-screen-for-arduino/p/XC4630

#define USE_DISPLAY 0

#if USE_DISPLAY

//#include "Arduino.h"
#include "Ethernet.h"
#include "EEPROM.h"
#include "Global.h"
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
//#include "TouchScreen_PM.h"

#define DEBUG_SCREEN  1

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define XP      8
#define XM      A2
#define YP      A3
#define YM      9
#define DEV_ID  33328 // 0x8230

//Menu constants
#define MENU_MAIN       10
#define MENU_SETUP      11
#define MENU_IPADDRESS  12

//Button constants
#define BTN_SETUP       0
#define BTN_IPADDRESS   0

void DisplaySetup();
void DisplayMenu();
//void DisplayPrint(const char *btext);
int readTouch(void);
void processTouch();
int getPressure();
int getTouchStage();
void DisplaySwap(bool LED);
void Debug(bool force);
bool NeedTouch();
#endif
#endif
