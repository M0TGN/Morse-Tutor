/********************************************************************

      M O R S E   C O D E   P R A C T I C E    G E N E R A T O R

/********************************************************************

Author       : Dan Trudgian (M0TGN) http://www.m0tgn.com
Date         : 14th September 2015
Version      : 1.0
Description  :

This sketch is designed to emulate the Datong D70 practice oscillator.
It generated either letters, number or a mixture of both selected from a switch
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

A0 = Used for float voltage for random seed
A1 = Not used
A2 = Used for Speed variations
A3 = Used as Gap Variations
A3 = Not Used
A4 = Reserved for I2C LCD
A5 = Reserved for I2C LCD

Future enhancements to include
  1. LCD display - Completed 17/9/2015
  2. Callsign training - Complete 15/09/2015
  3. Code review
  
Known Issues
1. When generating call signs choosing a different mode will not immediately work
2. Call signs can be longer than 20 chars per line.
3. WPM calculation can be slightly off at mid point
4. 


To make this work well you will need :

2 x Variable Resistors. 1K LIN
1 x 4 way switch.
1 x Speaker

To make this gucci you can add a screen
I use an I2C screen, hence teh included Library
The library I use is here :
https://arduino-info.wikispaces.com/LCD-Blue-I2C

You will also need to know the address.
Use this link to discover the address of the I2C device :
http://playground.arduino.cc/Main/I2cScanner

  
/********************************************************************
