//////////////////////////////////////////////////////////////////////////////////////////////
// LED 16x16 Display Module
// Copyright 2023 The Tech Dungeon
//
// www.techdungeon.xyz
// support@techdungeon.xyz
// https://youtube.com/thetechdungeon
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

This is sample code for our 16x16 RGB LED project. We will be selling these on our website, 
etsy page, and at Retro shows.

The display comes pre-programmed with tons of sprites in static and animated forms. The
code for that binary is not being released as it has our custom art and effects that we use
in the product we sell.

However, you can take this code and make your own sprites for the display if you are so
inclined.  The code will show you the basics of getting the display enumarated and displaying
16x16 sprites on it. 

On our website www.techdungeon.xyz you can find our latest binary we have released 
for the display.  You can use this to restore your display back to the "factory" settings you
got it from us. This is useful if you play around with your own code and want to restore 
the display back to our binary. 

Any new updates to the code will be released here and new binaries with new art from us will
always be availble to people who have the display for free on our website. Be aware
if we come out with a new display product in the future with a higher resolution, that product
will have its own github page, and binaries. But any updates we make to this 16x16 sample code
will always be available here for people to use on that hardware.

If you are not a coder you can also download a Flash Download Tool to reflash a
released bin from us without needing Visual Studio Code, or Anrdroid Studio. Be aware that
if you want to write your own code for the display you will need Visual Studio Code, or 
Arduino Studio.  But the Flash download tool is perfect for those who just want to update to 
a newer release. We will add notes on how to use the Flash Download tool to our website.

==============================================================================================
NOTE if you modify the code or upload your own code to the device, we can't guarantee or 
support proper operation of the device. We can offer info on how to reflash back to our
binary, but if you do some damage to the device, or connected devices to it, we are not liable.
Any modifications you take are at your own risk. Please read the warnings in the code about
current limits imposed in the code, and do NOT modify them!
==============================================================================================

The code for this project is kept as simple as possible. Many people I talked to want to hack
around with art, but are new to the C language. So I have kept things as simple as possible.

For creating images I recommend the website https://www.piskelapp.com/ . Resize the canvas
to 16x16 and you can drag the file in this archive named led-display-pallete.gpl onto the 
sprite editor to load a palette that matches the order and color in the code.

You can export your sprite in Piskel to a C file and then you can replace the RGB values
with the proper single byte index for that color as noted in our C source.  Be sure to 
set the color format in preferences to hex so it outputs the sprite as a hex array.

After converting the array to a 256 byte array you can place it into the sprites.h file
and reference it in the spriteslist array.  You can then compile it and download your
binary to the display.

PLEASE DO NOT MODIFY THE CURRENT LIMIT SET IN THE CODE. YOU COULD DAMAGE YOUR DISPLAY
OR PC/USB PORT!!!!!  You run your own code at your own risk.

To build the code you will need.

1) Arduino IDE or Visual Studio Code
2) ESP8266 Support added to the above
3) The FastLED library added also

I will try to clarify these steps if people need the help.
