//////////////////////////////////////////////////////////////////////////////////////////////
// LED 16x16 Display Module
// Copyright 2023 The Tech Dungeon
//
// This code is licensed under the GPL
//
// We believe in open development. The GPL has rights and obligations.
// You must abide by the terms of the GPL v3 license as noted in the LICENSE file
// If you modify this code you must make your source code available to end users upon request.
//
// Please read the LICENSE file as it defines your specific rights regarding the use of this
// source code, as well as condiditions under which the rights are given to you
//////////////////////////////////////////////////////////////////////////////////////////////

#include <FastLED.h>
#include "sprites.h"

#define LED_PIN     2                           // Data pin for controlling LED's
#define LED_XWIDTH  16                          // X Width of Pixels, 16 in our case
#define LED_YWIDTH  16                          // Y Width of Pixels, 16 in our case
#define NUM_LEDS    LED_XWIDTH * LED_YWIDTH     // Total number of pixels
#define BRIGHTNESS  32                          // Brightness setting. We keep this sane to reduce current draw
#define PAUSETIME   5000                        // Pause time between sprites in milliseconds
#define TEMPERATURE_1 Tungsten100W              // Color Temperature to use with FastLED library

CRGB leds[NUM_LEDS];                            // Define the array for our LED's

int total_sprites = 0;                          // Used to store total number of sprites in our array
sprites cur_sprite;                             // Current sprite being displayed

//////////////////////////////////////////////////////////////////////////////////////////////
// colors is an array of color values for our pixels. We have 10 defined.  You can
// tweak the color values to get a "better blue" or whatever.  If you add more colors it is 
// recommended to start them after the white. So start at 0x0a and go from there.
//////////////////////////////////////////////////////////////////////////////////////////////

CRGB colors[]={
  CHSV(0x00,0x00,0x00),   // 0x00 BLACK
  CHSV(0x00,0xff,0xFF),   // 0x01 Red
  CHSV(0x60,0xff,0xFF),   // 0x02 Green
  CHSV(0xA0,0xFF,0xFF),   // 0x03 Blue
  CHSV(0x80,0xff,0xFF),   // 0x04 Cyan
  CHSV(0x40,0xff,0xFF),   // 0x05 Yellow
  CHSV(0xc0,0xff,0xFF),   // 0x06 Purple
  CHSV(0x20,0xff,0xFF),   // 0x07 Orange
  CHSV(0x00,0x00,0x46),   // 0x08 Gray
  CHSV(0x00,0x00,0xFF)    // 0x09 White
};

//////////////////////////////////////////////////////////////////////////////////////////////
// Initial Setup of FastLED library
// Counting of total sprites
// Clearing the display
//////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.setTemperature( TEMPERATURE_1 ); // first temperature

  //////////////////////////////////////////////////////////////////////////////////////////////
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //
  // DO NOT CHANGE THE VALUES IN THE LINE BELOW THIS COMMENT UNLESS YOU KNOW WHAT YOU ARE DOING. 
  // THE CURRENT VALUES SET THE FASTLED LIBRARY TO LIMIT TOTAL AMPS USED BY THE 256 LED'S TO 
  // 1000 MILLIAMPS AT 5V. CHANGING THESE VALUES COULD DAMAGE THE MICROCONTROLLER OR YOUR 
  // PC'S USB PORT, YOUR MOTHERBOARD, YOUR USB-C POWER DEVICE, ETC.
  //
  // WE CAN NOT BE RESPONSIBLE FOR DAMAGE YOU MIGHT DO TO THE DISPLAY OR YOUR PC IF YOU MODIFY 
  // THE CODE AND IN PARTICULAR THE FastLED.setMaxPowerInVoltsAndMilliamps(5,1000) line!!!!!! 
  //
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //////////////////////////////////////////////////////////////////////////////////////////////

  FastLED.setMaxPowerInVoltsAndMilliamps(5,1000); 

  // Clear display
  FastLED.clear();
  FastLED.show();

  total_sprites = count_sprites();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// count_sprites loops through array checking for NULL to denote the end
//////////////////////////////////////////////////////////////////////////////////////////////

int count_sprites() {
  int x;
  for(x=0;spritelist[x].type!=0;x++);
  return(x);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// We call this function to translate a location on the 16x16 grid from the top/down 
// left/right we expect, to the actual pattern the LED's are wired in on the board. 
//////////////////////////////////////////////////////////////////////////////////////////////

unsigned char getOrder(unsigned char pos) {
  return(order[pos]);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Main program loop. This goes through all sprites and displays one and then pauses the
// PAUSETIME before moving on to the next sprite.
//////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  for(;;) {
    for(int x=0;x<total_sprites;x++) {
      show_sprite(x);
      delay(PAUSETIME);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// show_sprite shows the current sprite on the screen
//////////////////////////////////////////////////////////////////////////////////////////////

void show_sprite(int spriteNum) {
  // Run through all LED's and set their color to the given data for that frame
  for(int x=0;x<NUM_LEDS;x++) {
    leds[order[x]] = colors[spritelist[spriteNum].sprite[x]];
  }

  // Show sprite
  FastLED.show();
}

