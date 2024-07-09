#pragma once

#define SCREEN_WIDTH TFT_WIDTH //240    // tft.width()
#define SCREEN_HEIGHT TFT_HEIGHT //320   // tft.height()

#define PIXEL_SIZE_DEF 2    // in meters
#define MAX_ZOOM 7

#define DEVMODE_NAV 1
#define DEVMODE_MOVE 2
#define DEVMODE_ZOOM 3
#define DEVMODE_MENU 4

// Buttons GPIO nums
#define UP_BUTTON 03 // ok
#define DOWN_BUTTON 39 //!
#define LEFT_BUTTON 15 // ok
#define RIGHT_BUTTON 37 //!

#define SELECT_BUTTON 38
#define TFT_OFF_BUTTON 05
//#define MENU_BUTTON 05