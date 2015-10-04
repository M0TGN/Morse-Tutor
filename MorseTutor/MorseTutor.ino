//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                               LIBRARIES                                                //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <EEPROMex.h>
#include <EEPROMVar.h>
#include <Wire.h>
#include <LCD.h>                                 //For LCD display
#include <LiquidCrystal_I2C.h>                   //For LCD display

#define DEBUG

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                            PIN DEFINITIONS                                             //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define gapPin        A3                        // Variable resistor to control Gap time
#define spdPin        A2                        // Variable resistor to control Speed time
#define tonePin        3                        // Speaker is connected to pin 3
#define CALLSIGNPIN    8                        // If LOW then mode is CALLSIGNS
#define MIXEDPIN       9                        // If LOW then mode is MIXED
#define NUMBERSPIN    10                        // If LOW then mode is NUMBERS
#define LETTERSPIN    11                        // If LOW then mode is LETTERS
#define SPECIALPIN    12                        // If LOW then mode is SPECIAL CHARACTERS

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                           MORSE DEFINITIONS                                            //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define gapSpace (analogRead(gapPin)*3)
#define dotPeriod (analogRead(spdPin)/5+30)
#define dahPeriod (dotPeriod*3)
#define ActualWPM (1038/dotPeriod)
#define relaxTime (dahPeriod * 1.3)               // Relax time to be used between charactors
#define letterSpace (dotPeriod*1)
#define wordSpace (dotPeriod*7)
#define Delay (gapSpace / 100 / 6.7)
#define buzz (760)                                // Tone value set here. 800 = 800Hz

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                            MODE DEFINITIONS                                            //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define MODE_CALLSIGNS      8
#define MODE_MIXED          9
#define MODE_NUMBERS       10
#define MODE_LETTERS       11
#define MODE_SPECIAL       12

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                VARIABLES                                               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
float VerNo = 1.1;                                // Current Version Number.
uint8_t charGroup = 5;                            // How many characters to generate per group
uint8_t wordSize = 0;                             // Keeps track of word size
uint8_t ColPos = 0;                               // Current X position on LCD
uint8_t RowPos = 0;                               // Current Y position on LCD.
int mode;                                         // Mode of operation
String BlankLine = "                    ";
uint16_t lettersAddress, numbersAddress, specialAddress;
byte modeSelection[5];
char charsArray[53];

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                            OBJECT INITIATIONS                                          //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); //Initiate LiquidCrystal object for LCD. (0x27 is the I2C bus address for an unmodified I2C backpack)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                            SET UP INTERRUPTS                                           //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Software of reset the Arduino by calling the resetFunc()
void(* resetFunc) (void) = 0;


void initPinChangeInterrupt(void) {
  cli();
  pinMode(CALLSIGNPIN, INPUT_PULLUP);
  pinMode(MIXEDPIN, INPUT_PULLUP);
  pinMode(NUMBERSPIN, INPUT_PULLUP);
  pinMode(LETTERSPIN, INPUT_PULLUP);
  pinMode(SPECIALPIN, INPUT_PULLUP);// Enable pullups on D8 - D12
  PCICR = 0x01;                                     // Enable pin change interrupts on Port B
  PCMSK0 = 0x1F;  // Enable specific interrupts for pins D8 - D12
  sei();
}

ISR(PCINT0_vect) {
  for (int i = 0; i < sizeof(modeSelection); i++)
  {
    if (digitalRead(modeSelection[i]) == LOW)
    {
      if (mode != modeSelection[i]) {
        mode = modeSelection[i];
        resetFunc();
      }
      // Save this mode
      mode = modeSelection[i];

    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                  SETUP                                                 //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void setup() {
  getAddresses();                               // Get addresses to read characters from EEPROM as required
  initPinChangeInterrupt();

#ifdef DEBUG
  Serial.begin(115200);
#endif

  modeSelection[0] = MODE_CALLSIGNS;   //  Call signs
  modeSelection[1] = MODE_MIXED;   //  Mixed
  modeSelection[2] = MODE_NUMBERS;  // Numbers
  modeSelection[3] = MODE_LETTERS;  // Letters
  modeSelection[4] = MODE_SPECIAL;  //Special characters

  for (int i = 0; i < sizeof(modeSelection); i++)
  {
    if (digitalRead(modeSelection[i]) == LOW) {
      mode = modeSelection[i];
    };
  }

  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  delay(50);
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print(F("Morse Tutor"));
  lcd.setCursor(2, 2);
  lcd.print(F("Version no: v"));
  lcd.print(VerNo);
  delay(1000);
  lcd.clear();

  randomSeed(analogRead(A0));

  //When switching mode, read the correct set of data from EEPROM
  switch (mode) {
    case MODE_CALLSIGNS:
      clearArray(charsArray);
      EEPROM.readBlock<char>(lettersAddress, charsArray, 36);
#ifdef DEBUG
      Serial.println(charsArray);
#endif
      playModeTone(mode);
      break;

    case MODE_MIXED:
      clearArray(charsArray);
      EEPROM.readBlock<char>(lettersAddress, charsArray, 53);
#ifdef DEBUG
      Serial.println(charsArray);
#endif
      playModeTone(mode);
      break;

    case MODE_NUMBERS:
      clearArray(charsArray);
      EEPROM.readBlock<char>(numbersAddress, charsArray, 10);
#ifdef DEBUG
      Serial.println(charsArray);
#endif
      playModeTone(mode);
      break;

    case MODE_LETTERS:
      clearArray(charsArray);
      EEPROM.readBlock<char>(lettersAddress, charsArray, 26);
#ifdef DEBUG
      Serial.println(charsArray);
#endif
      playModeTone(mode);
      break;

    case MODE_SPECIAL:
      clearArray(charsArray);
      EEPROM.readBlock<char>(specialAddress, charsArray, 17);
#ifdef DEBUG
      Serial.println(charsArray);
#endif
      playModeTone(mode);
      break;

    default:
      break;
  }

#ifdef DEBUG
  // Welcome message
  Serial.println();
  Serial.print (F("Starting up    Speed :"));
  Serial.print (ActualWPM);
  Serial.print (F("WPM    Tone :"));
  Serial.print (buzz);
  Serial.print (F("Hz    Mode :"));
  switch (mode) {
    case MODE_CALLSIGNS:
      Serial.println (F("CALLSIGNS"));
      break;

    case MODE_MIXED:
      Serial.println (F("MIXED"));
      break;

    case MODE_NUMBERS:
      Serial.println (F("NUMBERS"));
      break;

    case MODE_LETTERS:
      Serial.println (F("LETTERS"));
      break;

    case MODE_SPECIAL:
      Serial.println (F("SPECIAL"));
      break;

    default:
      break;
  }
#endif

  lcd.print(BlankLine);
  lcd.setCursor(0, 1);
  lcd.print(BlankLine);
  lcd.setCursor(0, 2);
  lcd.print(BlankLine);

  // Call Display mode, set the cursor to 0,0
  DisplayMode (ActualWPM, Delay, mode, 0, 0);

  Serial.print ("\n"); // Write a new line for the code
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                     LOOP                                               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void loop() {
  switch (mode) {
    case MODE_CALLSIGNS:
      randCallsign();
      break;
    default:
      randChar(mode);
      break;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                  FUNCTIONS                                             //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//******************//
// EEPROM Functions //
//******************//
void getAddresses()
{
  lettersAddress = EEPROM.getAddress(sizeof(char) * 26);
  numbersAddress = EEPROM.getAddress(sizeof(char) * 10);
  specialAddress = EEPROM.getAddress(sizeof(char) * 17);

#ifdef DEBUG
  Serial.print(F("lettersAddress --> "));
  Serial.println(lettersAddress);
  Serial.print(F("numbersAddress --> "));
  Serial.println(numbersAddress);
  Serial.print(F("specialAddress --> "));
  Serial.println(specialAddress);
#endif
}

//*****************//
// Misc. Functions //
//*****************//
void playModeTone(uint8_t _mode) {
  switch (_mode) {
    case MODE_MIXED:
      tone(tonePin, 900, 50);
      delay(600);
      dah(); dah(); //playCharacter('M');
      tone(tonePin, 900, 50);
      delay(200);
      break;

    case MODE_NUMBERS:
      tone(tonePin, 900, 50);
      delay(600);
      dah(); dit(); //playCharacter('N');
      tone(tonePin, 900, 50);
      delay(200);
      break;

    case MODE_LETTERS:
      tone(tonePin, 900, 50);
      delay(600);
      dit(); dah(); dit(); dit(); //playCharacter('L');
      tone(tonePin, 900, 50);
      delay(200);
      break;

    case MODE_SPECIAL:
      tone(tonePin, 900, 50);
      delay(600);
      dit(); dit(); dit(); //playCharacter('S');
      tone(tonePin, 900, 50);
      delay(200);
      break;

    case MODE_CALLSIGNS:
      tone(tonePin, 900, 50);
      delay(600);
      dah(); dit(); dah(); dit(); //playCharacter('C');
      tone(tonePin, 900, 50);
      delay(200);
      break;
  }
}

void clearArray(char dataArray[]) {
  for (int i = 0; i < sizeof(dataArray); i++) {
    dataArray[i] = 0;
  }
}
