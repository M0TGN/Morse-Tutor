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
#define CALLSIGNPIN  PB0                        // If LOW then mode is CALLSIGNS
#define MIXEDPIN     PB1                        // If LOW then mode is MIXED
#define NUMBERSPIN   PB2                        // If LOW then mode is NUMBERS
#define LETTERSPIN   PB3                        // If LOW then mode is LETTERS
#define SPECIALPIN   PB4                        // If LOW then mode is SPECIAL CHARACTERS

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
#define buzz (760)                                // Tone value set here. 800 = 800Hz

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                            MODE DEFINITIONS                                            //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define MODE_CALLSIGNS      0
#define MODE_MIXED          1
#define MODE_NUMBERS        2
#define MODE_LETTERS        3
#define MODE_SPECIAL        4

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                VARIABLES                                               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
float VerNo = 1.0;                                // Current Version Number.
uint8_t charGroup = 5;                            // How many characters to generate per group
uint8_t wordSize = 0;                             // Keeps track of word size
uint8_t ColPos = 0;                               // Current X position on LCD
uint8_t RowPos = 0;                               // Current Y position on LCD.
int mode = 0;                                     // Mode of operation
String BlankLine = "                    ";
uint16_t lettersAddress, numbersAddress, specialAddress;
char charsArray[53];

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                            OBJECT INITIATIONS                                          //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); //Initiate LiquidCrystal object for LCD. (0x27 is the I2C bus address for an unmodified I2C backpack)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                            SET UP INTERRUPTS                                           //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void initPinChangeInterrupt(void) {
  DDRB &= 0x1F;                                         // Set Pins D8 - D12 as input
  PORTB |= ((1 << CALLSIGNPIN) | (1 << MIXEDPIN)
            | (1 << NUMBERSPIN) | (1 << LETTERSPIN) | (1 << SPECIALPIN));   // Enable pullups on D8 - D12
  PCICR |= (1 << PCIE0);                                // Enable pin change interrupts on Port B
  PCMSK0 |= ((1 << CALLSIGNPIN) | (1 << MIXEDPIN)
             | (1 << NUMBERSPIN) | (1 << LETTERSPIN) | (1 << SPECIALPIN));  // Enable specific interrupts for pins D8 - D12
  sei();
}

ISR(PCINT0_vect) {
  if (bit_is_set(PORTB, CALLSIGNPIN)) {
    mode = MODE_CALLSIGNS;
    clearArray(charsArray);
    EEPROM.readBlock<char>(lettersAddress, charsArray, 36);
    playModeTone(mode);
  }
  if (bit_is_set(PORTB, MIXEDPIN)) {
    mode = MODE_MIXED;
    clearArray(charsArray);
    EEPROM.readBlock<char>(lettersAddress, charsArray, 53);
    playModeTone(mode);
  }

  if (bit_is_set(PORTB, NUMBERSPIN)) {
    mode = MODE_NUMBERS;
    clearArray(charsArray);
    EEPROM.readBlock<char>(numbersAddress, charsArray, 10);
    playModeTone(mode);
  }

  if (bit_is_set(PORTB, LETTERSPIN)) {
    mode = MODE_LETTERS;
    clearArray(charsArray);
    EEPROM.readBlock<char>(lettersAddress, charsArray, 26);
    playModeTone(mode);
  }

  if (bit_is_set(PORTB, SPECIALPIN)) {
    mode = MODE_SPECIAL;
    clearArray(charsArray);
    EEPROM.readBlock<char>(specialAddress, charsArray, 17);
    playModeTone(mode);
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                  SETUP                                                 //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void setup() {
  getAddresses();                               // Get addresses to read characters from EEPROM as required

#ifdef DEBUG
  Serial.begin(115200);
#endif

  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(4, 2);
  lcd.print("Morse Tutor");
  lcd.setCursor(4, 3);
  lcd.print("Version no: ");
  lcd.print(VerNo);

  randomSeed(analogRead(A0));

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
  DisplayMode (uint8_t(ActualWPM), float(gapSpace / 100 / 6.7), mode, 0, 0);

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
      playCharacter('M');
      tone(tonePin, 900, 50);
      delay(200);
      break;

    case MODE_NUMBERS:
      tone(tonePin, 900, 50);
      delay(600);
      playCharacter('N');
      tone(tonePin, 900, 50);
      delay(200);
      break;

    case MODE_LETTERS:
      tone(tonePin, 900, 50);
      delay(600);
      playCharacter('L');
      tone(tonePin, 900, 50);
      delay(200);
      break;

    case MODE_SPECIAL:
      tone(tonePin, 900, 50);
      delay(600);
      playCharacter('S');
      tone(tonePin, 900, 50);
      delay(200);
      break;

    case MODE_CALLSIGNS:
      tone(tonePin, 900, 50);
      delay(600);
      playCharacter('C');
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
