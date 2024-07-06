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
#define UP_BUTTON 03
#define DOWN_BUTTON 04
#define LEFT_BUTTON 15
#define RIGHT_BUTTON 16
#define SELECT_BUTTON 07
#define TFT_OFF_BUTTON 05
#define MENU_BUTTON 5

#define TFT_BLK_PIN 21
#define TFT_CS 10


#define SD_MISO_PIN 13
#define SD_CS_PIN 14

#define GPS_RX 17
#define GPS_TX 18
// #define GPS_CE 14