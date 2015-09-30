
//*******************************************************************//

      M O R S E   C O D E   P R A C T I C E    G E N E R A T O R

//*******************************************************************//

Author        : Dan Trudgian (M0TGN) http://www.m0tgn.com
Date          : 14th September 2015
Version       : 1.1
Modified      : Marzogh http://chipsncode.com
Date modified : 01st October 2015
Description   :

This sketch is designed to emulate the Datong D70 practice oscillator.
It generates either letters, numbers, special characters or a mixture of both selected by buttons/a switch
2 controls allowed the WPM to be altered and the gap between characters.

This sketch should achieve the following requirements :

  1. Generate either numbers, letters or mixed - switched selection.
  2. Variable speed based on Variable resistor (WPM) between 8 and 34 WPM
  3. Generate 5 chars groups
  4. Allow a variable gap based on Variable Resistor between 0.5 and 4 seconds

Pin Assignments :

Switch
D8 = Call signs
D9 = Mixed
D10 = Numbers
D11 = Letters
D12 = Special characters        *Added by Marzogh 01/10/2015*

A0 = Used for float voltage for random seed
A1 = Not used
A2 = Used for Speed variations
A3 = Used as Gap Variations
A3 = Not Used
A4 = Reserved for I2C LCD
A5 = Reserved for I2C LCD

Change log:
1. Added the ability for mode changes to happen on Pin Change interrupts                  *Added by Marzogh 01/10/2015*
2. Cleaned up and optimised the code - especially in the randCallsign function            *Added by Marzogh 01/10/2015*
3. Added the ability to read special characters/ punctuation                              *Added by Marzogh 01/10/2015*
4. Added an extra sketch to load data into EEPROM from where it is retrieved as required  *Added by Marzogh 01/10/2015*

  
Known Issues (Not checked/fixed from original version)
1. Call signs can be longer than 20 chars per line
2. WPM calculation can be slightly off at mid point 


To make this work you will need :

1 x ATMega328 with Arduino Uno bootloader
2 x Variable Resistors. 1K LIN
1 x 4 way switch (or) 4 buttons
1 x Speaker

You can optionally add a screen
I use an I2C 20x4 LCD display, hence the included Library
The library I use is here :
https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads

You will also need to know the address of the I2C LCD.
Use this link to discover the address of the I2C device :
http://playground.arduino.cc/Main/I2cScanner
