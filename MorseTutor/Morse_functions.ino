void playCharacter(char x)
{

  // Orginised is a cock ass manor, quicker to search through.
  switch (x) {
    case 'E':
      dit(); printLetter(x);  break;
    case 'I':
      dit(); dit(); printLetter(x); break;
    case 'S':
      dit(); dit(); dit(); printLetter(x); break;
    case 'A':
      dit(); dah(); printLetter(x); break;
    case 'U':
      dit(); dit(); dah(); printLetter(x); break;
    case 'T':
      dah(); printLetter(x); break;
    case 'M':
      dah(); dah(); printLetter(x); break;
    case 'O':
      dah(); dah(); dah(); printLetter(x); break;
    case 'N':
      dah(); dit(); printLetter(x); break;
    case 'D':
      dah(); dit(); dit(); printLetter(x); break;
    case 'F':
      dit(); dit(); dah(); dit(); printLetter(x); break;
    case 'P':
      dit(); dah(); dah(); dit(); printLetter(x); break;
    case 'J':
      dit(); dah(); dah(); dah(); printLetter(x); break;
    case 'L':
      dit(); dah(); dit(); dit(); printLetter(x); break;
    case 'Q':
      dah(); dah(); dit(); dah(); printLetter(x); break;
    case 'X':
      dah(); dit(); dit(); dah(); printLetter(x); break;
    case 'B':
      dah(); dit(); dit(); dit(); printLetter(x); break;
    case 'Y':
      dah(); dit(); dah(); dah(); printLetter(x); break;
    case 'W':
      dit(); dah(); dah(); printLetter(x); break;
    case 'R':
      dit(); dah(); dit(); printLetter(x); break;
    case 'H':
      dit(); dit(); dit(); dit(); printLetter(x); break;
    case 'Z':
      dit(); dit(); dah(); dah(); printLetter(x); break;
    case 'G':
      dah(); dah(); dit(); printLetter(x); break;
    case 'K':
      dah(); dit(); dah(); printLetter(x); break;
    case 'V':
      dit(); dit(); dit(); dah(); printLetter(x); break;
    case 'C':
      dah(); dit(); dah(); dit(); printLetter(x); break;
    case '1':
      dit(); dah(); dah(); dah(); dah(); printLetter(x); break;
    case '2':
      dit(); dit(); ; dah(); dah(); dah();  printLetter(x); break;
    case '3':
      dit(); dit(); dit(); dah(); dah();  printLetter(x); break;
    case '4':
      dit();  dit(); dit(); dit(); dah();  printLetter(x); break;
    case '5':
      dit(); dit(); dit(); dit(); dit(); printLetter(x); break;
    case '6':
      dah(); dit(); dit(); dit(); dit(); printLetter(x); break;
    case '7':
      dah(); dah(); dit(); dit(); dit(); printLetter(x); break;
    case '8':
      dah(); dah(); dah(); dit(); dit(); printLetter(x); break;
    case '9':
      dah(); dah(); dah(); dah(); dit(); printLetter(x); break;
    case '0':
      dah(); dah(); dah(); dah(); dah(); printLetter(x); break;
    case '/':
      dah(); dit(); dit(); dah(); dit(); printLetter(x); break;
    case '.':
      dit(); dah(); dit(); dah(); dit(); dah(); printLetter(x); break;
    case ',':
      dah(); dah(); dit(); dit(); dah(); dah(); printLetter(x); break;
    case '?':
      dit(); dit(); dah(); dah(); dit(); dit(); printLetter(x); break;
    case '`':
      dit(); dah(); dah(); dah(); dah(); dit(); printLetter(x); break;
    case '!':
      dah(); dit(); dah(); dit(); dah(); dah(); printLetter(x); break;
    case '(':
      dah(); dit(); dah(); dah(); dit(); printLetter(x); break;
    case ')':
      dah(); dit(); dah(); dah(); dit(); dah(); printLetter(x); break;
    case '&':
      dit(); dah(); dit(); dit(); dit(); printLetter(x); break;
    case ':':
      dah(); dah(); dah(); dit(); dit(); dit(); printLetter(x); break;
    case ';':
      dah(); dit(); dah(); dit(); dah(); dit(); printLetter(x); break;
    case '=':
      dah(); dit(); dit(); dit(); dah(); printLetter(x); break;
    case '+':
      dit(); dah(); dit(); dah(); dit(); printLetter(x); break;
    case '-':
      dah(); dit(); dit(); dit(); dah(); printLetter(x); break;
    case '"':
      dit(); dah(); dit(); dit(); dah(); dit(); printLetter(x); break;
    case '@':
      dit(); dah(); dah(); dit(); dah(); dit(); printLetter(x); break;
  }
}

void dit()
{
  tone(tonePin, buzz, dotPeriod);
  delay(relaxTime / 2);
}

void dah()
{
  tone(tonePin, buzz, dahPeriod);
  delay(relaxTime);
}

void randCallsign() {

  uint8_t pattern = random(1, 5);
  uint8_t randLetter[5] = { random(0, 25), random(0, 25), random(0, 25), random(0, 25), random(0, 25)};
  uint8_t randNumber = random(26, 35);

  // patterns of callsigns to generate X0XXX XX0XXX X0XX
  // patterns of suffix to chose. /m /p & null
  // Choose a letter, a number and 3 letters to create a fake callsign.
  switch (pattern) {
    case 1:
      for (int i = 0; i < 3; i++) {
        playCharacter(charsArray[randLetter[i]]);
        if (i == 0)
          playCharacter(charsArray[randNumber]);
      }
      delay(relaxTime);
#ifdef DEBUG
      Serial.print (' ');
#endif
      ColPos++;
      delay(wordSpace);

    case 2:
      for (int i = 0; i < 4; i++) {
        playCharacter(charsArray[randLetter[i]]);
        if (i == 2)
          playCharacter(charsArray[randNumber]);
      }
      delay(relaxTime);
#ifdef DEBUG
      Serial.print (' ');
#endif
      ColPos++;
      delay(wordSpace);

    case 3:
      for (int i = 0; i < 2; i++) {
        playCharacter(charsArray[randLetter[i]]);
        if (i == 0)
          playCharacter(charsArray[randNumber]);
      }
      delay(relaxTime);
#ifdef DEBUG
      Serial.print (' ');
#endif
      ColPos++;
      delay(wordSpace);

    case 4:
      for (int i = 0; i < 3; i++) {
        playCharacter(charsArray[randLetter[i]]);
        if (i == 0)
          playCharacter(charsArray[randNumber]);
      }
      playCharacter ('/');
      playCharacter ('M');
      delay(relaxTime);
#ifdef DEBUG
      Serial.print (' ');
#endif
      ColPos++;
      delay(wordSpace);

    case 5:
      for (int i = 0; i < 2; i++) {
        playCharacter(charsArray[randLetter[i]]);
        if (i == 0)
          playCharacter(charsArray[randNumber]);
      }
      playCharacter ('/');
      playCharacter ('P');
      delay(relaxTime);
#ifdef DEBUG
      Serial.print (' ');
#endif
      ColPos++;
      delay(wordSpace);
  }
}

void randChar(uint8_t _mode) {
  wordSize++;
  switch (_mode) {
    case MODE_MIXED:
      playCharacter(charsArray[random(0, 53)]);
      break;

    case MODE_NUMBERS:
      playCharacter(charsArray[random(0, 10)]);
      break;

    case MODE_LETTERS:
      playCharacter(charsArray[random(0, 26)]);
      break;

    case MODE_SPECIAL:
      playCharacter(charsArray[random(0, 17)]);
      break;
  }
  if (wordSize >= charGroup) {
#ifdef DEBUG
    Serial.print (' ');
#endif
    ColPos++;
    delay(wordSpace);
    wordSize = 0 ;
  }
}
