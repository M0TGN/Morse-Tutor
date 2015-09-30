void DisplayMode (uint8_t _wpm, float _delay , uint8_t _mode, uint8_t _xpos, uint8_t _ypos)
{
  // Display Speed
  lcd.setCursor(0, 4);
  lcd.print(_wpm);
  lcd.print("WPM ");

  //Delay
  lcd.setCursor(7, 4);
  lcd.print(_delay);
  lcd.print("s");

  //Mode
  lcd.setCursor(13, 4);
  switch (_mode)
  {
    case MODE_CALLSIGNS:
      lcd.print ("  Calls");
      break;
    case MODE_MIXED:
      lcd.print ("  Mixed");
      break;
    case MODE_NUMBERS:
      lcd.print ("Numbers");
      break;
    case MODE_LETTERS:
      lcd.print ("Letters");
      break;
  }
  lcd.setCursor(_xpos, _ypos);                      // Returns cursor to position it started from
}

void printLetter(char x)
{
  /*
   * Prints letter to LCD screen, updates the footer and return cursor to correct position
   */
  if (ColPos == 18) {
    Serial.println();
    RowPos++;                                           // Increment row number
    ColPos = 0;                                         // Zero out the column
  }
  ColPos++;
  if (RowPos == 3) {
    RowPos = 0;
    lcd.clear(); // clear the screen
  }
  // Write the footer. Remember to send the cursor back to correct position.
  DisplayMode (uint8_t(ActualWPM), float(gapSpace / 100 / 6.7), mode, ColPos, RowPos);
  lcd.print(x);                                         // Write data to the screen.
  
#ifdef DEBUG
  Serial.print (x); // Write something
#endif
  delay(gapSpace);

}

