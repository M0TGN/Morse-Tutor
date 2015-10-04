#include <EEPROMex.h>
#include <EEPROMVar.h>

#define DEBUG

char letters[] = 
{ 
  'A', 'B', 'C', 'D', 'E','F',
  'G', 'H', 'I', 'J', 'K', 'L',
  'M', 'N', 'O', 'P', 'Q', 'R',
  'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z',
};

char numbers [] = 
{
  '0', '1', '2', '3', '4',
  '5', '6', '7', '8', '9',
};

char special [] = 
{
  '.', ',', '?', '`', '!',
  '/', '-','(', ')', '&',
  ':', ';', '=', '+', '-',
  '"', '@',
};
uint16_t lettersAddress, numbersAddress, specialAddress;

void setup() {
  Serial.begin(115200);
  getAddresses();
  updateValues();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void getAddresses()
{
  lettersAddress = EEPROM.getAddress(sizeof(char)*26);
  numbersAddress = EEPROM.getAddress(sizeof(char)*10);
  specialAddress = EEPROM.getAddress(sizeof(char)*17);

  #ifdef DEBUG
  Serial.print(F("lettersAddress --> "));
  Serial.println(lettersAddress);
  Serial.print(F("numbersAddress --> "));
  Serial.println(numbersAddress);
  Serial.print(F("specialAddress --> "));
  Serial.println(specialAddress);
  #endif
}

void updateValues()
{
  char outputLetters[26];
  char outputNumbers[10];
  char outputSpecial[17];
  
  EEPROM.updateBlock<char>(lettersAddress, letters, 26);
  EEPROM.updateBlock<char>(numbersAddress, numbers, 10);
  EEPROM.updateBlock<char>(specialAddress, special, 17);

  EEPROM.readBlock<char>(lettersAddress, outputLetters, 26);
  EEPROM.readBlock<char>(numbersAddress, outputNumbers, 10);
  EEPROM.readBlock<char>(specialAddress, outputSpecial, 17);
  Serial.println("Values");
  Serial.println(outputLetters);
  Serial.println(outputNumbers);
  Serial.println(outputSpecial);
}
