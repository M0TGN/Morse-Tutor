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
*/
// Set up Morse objects here
// Spacing and timings to create the perfect code.

// #define dotPeriod 100 // For when you have no Variable resistors.
#define gapSpace (analogRead(3)*3)
//#define gapSpace (60) // For when we have no Variable resitor
#define dotPeriod (analogRead(2)/5+30)
#define dahPeriod (dotPeriod*3)
#define ActualWPM (1038/dotPeriod)
#define relaxTime (dahPeriod * 1.3) // Relax time to be used between charactors
#define letterSpace (dotPeriod*1)
#define wordSpace (dotPeriod*7)
#define tonePin 3 // Speaker is connected to pin 3
#define buzz (760) // Tine value set here. 800 = 800Hz
String VerNo="V1.01"; // Current Version Number.
int CharGroup = 5; // How many charactors to generate per group
int i= 0; // i is used a a counter for the char generation. Set to zero.
int ColPos=0; // Current X position on LCD
int RowPos=0; // Current Y position on LCD
byte ModeSelection[4];  // holds the port numbers of the push buttons.
int RndCallsign = 0; // Local int used for random callsign pattern
String InfoMsg = " "; // An emty string for emptyness 
int ModePin=0; //What mode have we selected. lets start assuming we know nothing.
int ModeCalls = 8;
int ModeMixed = 9;
int ModeNumbers = 10;
int ModeLetters =11;
String BlankLine ="                  ";

#include <Wire.h> 
#include <LiquidCrystal_I2C.h> //Get this here https://arduino-info.wikispaces.com/LCD-Blue-I2C

// initialize the library with the numbers of the interface pins
// LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // My I2C screen is on 0x3F - yours might not be. Check and adjust.

// Array for Mixed
    char MIXED_CHARACTERS[] = {
        // Numbers
        '0', '1', '2', '3', '4',
        '5', '6', '7', '8', '9',
        // Letters 
        'A', 'B', 'C', 'D', 'E', 
        'F', 'G', 'H', 'I', 'J', 
        'K', 'L', 'M', 'N', 'O', 
        'P', 'Q', 'R', 'S', 'T', 
        'U', 'V', 'W', 'X', 'Y', 'Z', 
    };

// Array for numbers only.
    char NUM_CHARACTERS[] = {
        // Numbers
        '0', '1', '2', '3', '4',
        '5', '6', '7', '8', '9',
    };

// Array for Letters only.
    char LETTERS_CHARACTERS[] = {
        // Letters 
        'A', 'B', 'C', 'D', 'E', 
        'F', 'G', 'H', 'I', 'J', 
        'K', 'L', 'M', 'N', 'O', 
        'P', 'Q', 'R', 'S', 'T', 
        'U', 'V', 'W', 'X', 'Y', 'Z', 
    };


void setup()
{
  lcd.begin(20, 4);      // Cuzz we have a 20x4 display
  lcd.clear();           // Empty the display - who knows whats on there right now eh ?
lcd.setCursor(7,0);
lcd.print("M0TGN");  // Im so vain, i proberbly think this code is about me...
 lcd.setCursor(5,1);
lcd.print("Morse Tutor");
 lcd.setCursor(7,2);
lcd.print(VerNo);
 
  ModeSelection[0] = ModeCalls;   //  Call signs
  ModeSelection[1] = ModeMixed;   //  Mixed
  ModeSelection[2] = ModeNumbers;  // Numbers
  ModeSelection[3] = ModeLetters;  // Letters  

// initialize the button ports
  for (int i = 0; i < sizeof(ModeSelection); i++)
  {

   pinMode(ModeSelection[i], INPUT);      // set port as input
   digitalWrite(ModeSelection[i], HIGH);  // enable pull-up resistor
    
  } 
  
// Write out the text to teh Serial Screen. Start up comms here  
Serial.begin(9600);

// Make as random as I can - by reading a float voltage off an unused Analogue Pin
randomSeed(analogRead(A0));  

// Welcome message
Serial.println();
Serial.print ("Starting up    Speed :");
Serial.print (ActualWPM);
Serial.print ("WPM    Tone :");
Serial.print (buzz);
Serial.print ("Hz    Mode :");



// Loop through all our pins, see which is pulled LOW
  for (int i = 0; i < sizeof(ModeSelection); i++)
  {
    if (digitalRead(ModeSelection[i])==LOW){
    ModePin=ModeSelection[i];
    };
  } ;     
 

// Write out friendly text depending on which pin is LOW 
    switch (ModePin){
          case 8:
          Serial.print ("Callsigns"); 
          break;
          case 9:
          Serial.print ("Mixed");
          break;
          case 10:
          Serial.print ("Numbers");
          break;
          case 11:
          Serial.print ("Letters");
          break;

    
    
  } 
// Some musical interlude. 

PlaySong(ModePin);
  


lcd.setCursor(0,0);
lcd.print(BlankLine);
lcd.setCursor(0,1);
lcd.print(BlankLine);
lcd.setCursor(0,2);
lcd.print(BlankLine);

// Call Display mode, set the cursor to 0,0  
DisplayMode (String(ActualWPM), float(gapSpace/100/6.7), ModePin, 0, 0);

Serial.print ("\n"); // Write a new line for the code
delay(2500); // Added tension for dramatic effect.

// Ready now. lets create some tones !

}

void(* resetFunc) (void) = 0; //A Software reset function - called when mode is changed.


void loop()
{
ModeSelect(); // Monitor the mode pins.
}

void RandChars(int x)
{
i++;
switch (x){
  case 9:
        playLetter (MIXED_CHARACTERS[random(0,35)]);
      break;
  case 10:
  
        playLetter (NUM_CHARACTERS[random(0,10)]); 
       
     break;
  case 11:
        playLetter (LETTERS_CHARACTERS[random(0,25)]);  
     break;
delay(relaxTime); // And relax.
}

// Have we displayed 5 chars ? break it up, insert a space and a pause.
if (i>=CharGroup) {
 Serial.print (' ');
  ColPos++;
 delay(wordSpace);
i=0 ;
}; 
}



void printLetter(char x) 
{
/*
Assuming we have an 20x4 LCD we can only show 3 groups on 
the same line. So we will use 18 as a check number for number 
of written chars to screen.

We save the current position of the cusor so we can retrun back here after writing out
the footer.
*/

  
  if (ColPos==18){
    Serial.println();
    // Increment row number
    RowPos++;
    // Cols = zero baby.
    ColPos=0;
  }
  ColPos++;
  Serial.print (x); // Write somthing out
if (RowPos==3){
  RowPos=0;
  lcd.clear(); // clear the screen
}
  // Write the footer out. remember to send the cursor back to correct position.
  DisplayMode (String(ActualWPM), float(gapSpace/100/6.7), ModePin, ColPos, RowPos);
    lcd.print(x); // Write somthing to the screen.
    
    
  // TODO :: This delay to be adjusted by an Analogue Read.
  //delay(wordSpace+gapSpace);
  delay(gapSpace);
  
}

void playLetter(char x)
{
  
  // Orginised is a cock ass manor, quicker to search through.
	switch (x){
  		case 'E':
			dit(); printLetter(x);  return;
		case 'I':
			dit(); dit(); printLetter(x); return;
		case 'S':
			dit(); dit(); dit(); printLetter(x);return;
		case 'A':
			dit(); dah(); printLetter(x);return;
		case 'U':
			dit(); dit(); dah(); printLetter(x);return;
		case 'T':
			dah(); printLetter(x);return;
		case 'M':
			dah(); dah(); printLetter(x);return;
		case 'O':
			dah(); dah(); dah(); printLetter(x);return;
		case 'N':
			dah(); dit(); printLetter(x);return;
		case 'D':
			dah(); dit(); dit(); printLetter(x);return;
		case 'F':
			dit();dit(); dah(); dit(); printLetter(x);return;
		case 'P':
			dit(); dah(); dah(); dit(); printLetter(x); return;
		case 'J':
			dit();dah();dah();dah(); printLetter(x);return;
		case 'L':
			dit(); dah(); dit(); dit(); printLetter(x);return;
		case 'Q':
			dah(); dah(); dit(); dah(); printLetter(x);return;
		case 'X':
			dah(); dit(); dit(); dah(); printLetter(x); return;
		case 'B':
			dah(); dit(); dit();dit(); printLetter(x);return;
		case 'Y':
			dah(); dit(); dah(); dah(); printLetter(x); return;
		case 'W':
			dit(); dah(); dah(); printLetter(x); return;
		case 'R':
			dit(); dah(); dit(); printLetter(x);return;
		case 'H':
			dit(); dit();dit();dit(); printLetter(x);return;
		case 'Z':
			dit();dit(); dah();dah();printLetter(x); return;
		case 'G':
			dah(); dah(); dit(); printLetter(x);return;
		case 'K':
			dah(); dit();dah(); printLetter(x); return;
		case 'V':
			dit(); dit(); dit(); dah();printLetter(x); return;
		case 'C':
			dah(); dit(); dah();dit();printLetter(x);return;
		case '1':
			 dit(); dah(); dah(); dah(); dah(); printLetter(x);return;
		case '2':
			dit();dit(); ; dah(); dah(); dah();  printLetter(x);return;
		case '3':
			dit(); dit();dit(); dah(); dah();  printLetter(x);return;
		case '4':
			dit();  dit();dit();dit(); dah();  printLetter(x);return;
		case '5':
			 dit(); dit();dit(); dit(); dit();printLetter(x);return;
		case '6':
			  dah(); dit();dit(); dit(); dit();printLetter(x);return;
		case '7':
			 dah(); dah();dit(); dit(); dit();printLetter(x);return;
		case '8':
			  dah(); dah(); dah(); dit(); dit();printLetter(x);return;
		case '9':
			  dah(); dah(); dah(); dah(); dit();printLetter(x);return;
		case '0':
			 dah();dah();dah();dah();dah();printLetter(x);return;
		case ' ':
			delay(wordSpace);
    case '/':
       dah();dit();dit();dah();dit(); printLetter(x);return;

}

}

void dit()
{ 
  tone(tonePin, buzz, dotPeriod);
  //delay(dotPeriod);
  delay(relaxTime /2);

}

void dah()
{

  tone(tonePin, buzz, dahPeriod);
    //delay(dotPeriod);
  delay(relaxTime); 
}


void RandCallsign()
{
  
int RndCallsign = random(1,5);

// TODO :: Not happy with this function ! Its crap.


 // patterns of callsigns to generate X0XXX XX0XXX X0XX 
 // patterns of suffix to chose. /m /p & null
 // Choose a letter, a number and 3 letters to create a fake callsign.
switch (RndCallsign){ 
 case 1:
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (NUM_CHARACTERS[random(0,10)]); 
  
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   delay(relaxTime);
   Serial.print (' ');
     ColPos++;
   delay(wordSpace);
 
 case 2:
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (NUM_CHARACTERS[random(0,10)]); 
  
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   delay(relaxTime);
   Serial.print (' ');
    ColPos++;
   delay(wordSpace);
   
 case 3:
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (NUM_CHARACTERS[random(0,10)]); 
  
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   delay(relaxTime);
   Serial.print (' ');
    ColPos++;
   delay(wordSpace);
   
   
    case 4:
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (NUM_CHARACTERS[random(0,10)]); 
  
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter ('/');
   playLetter ('M');
   delay(relaxTime);
   Serial.print (' ');
    ColPos++;
   delay(wordSpace);
 
  
   

   
 case 5:
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (NUM_CHARACTERS[random(0,10)]); 
  
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter (LETTERS_CHARACTERS[random(0,25)]);
   playLetter ('/');
   playLetter ('P');
   delay(relaxTime);
   Serial.print (' ');
     ColPos++;
   delay(wordSpace);
   
   
   
} 




}

void DisplayMode (String strWPM, float strDelay ,int intMode, int XPos, int YPos)
{
  // Display Speed
  lcd.setCursor(0,4);
  InfoMsg = strWPM + "WPM ";
  lcd.print(InfoMsg);
  
  //Delay
  lcd.setCursor(7,4);
  lcd.print(strDelay);
  lcd.print("s");
  //Mode
  lcd.setCursor(13,4);
  switch (intMode)
  {
          case 8:
          lcd.print ("  Calls"); 
          break;
          case 9:
          lcd.print ("  Mixed");
          break;
          case 10:
          lcd.print ("Numbers");
          break;
          case 11:
          lcd.print ("Letters");
          break;      
}




// return the cursor back to where it once was.

  lcd.setCursor(XPos,YPos);

  
}

void ModeSelect()
{

  // Check all our mode pins
    for (int i = 0; i < sizeof(ModeSelection); i++)
  {
    if (digitalRead(ModeSelection[i])==LOW)
        {

         // This little pin is LOW, so save this pin and check if the last 
         // Saved pin was the same, if its not - weve changed mode
         // So reset with new mode.
          
          if (ModePin != ModeSelection[i]) {
            ModePin=ModeSelection[i];
            
            resetFunc();
            
          };
          // Save this mode
          ModePin=ModeSelection[i];
          
        };
  } ; 
 
 if (ModePin ==8)
 {
   // Are we 8 ? are doing cals ?
   RandCallsign();
   
 }
    else{   
 RandChars(ModePin);  
    }; 
  
}; 

void PlaySong(byte m)
{
// TODO :: Change song to high beep and a letter denoting mode - so M, L, N & C 
  switch (m) 
  {
    case 8:

tone(tonePin, 900, 50);
delay(900);
dah(); dit();dah(); dit();
tone(tonePin, 900, 50);
break;   

    case 9:
tone(tonePin, 900, 50);
delay(900);
dah(); dah();
tone(tonePin, 900, 50);
delay(500);
break;    
    case 10:
tone(tonePin, 900, 50);
delay(900);
dah(); dit();
tone(tonePin, 900, 50);
delay(500);
break;    
case 11:
tone(tonePin, 900, 50);
delay(900);
dit(); dah();dit(); dit();
tone(tonePin, 900, 50);
delay(500);
break;

    
  }; 
  
  
}

 

