#include <SPI.h>
#include <HGS2561281.h>
#include "font.h"

// Instance to our LCD
// You will need to replace the arguments
// with the correct pin numbers
HGS2561281 lcd(RESET, CS1, CS2, DC, WR);

void setup(){
  
  // This may be needed by Energia...
  pinMode(CSLK_PIN, OUTPUT);
  pinMode(SDIN_PIN, OUTPUT);

  // Initialise the display
  lcd.init();
  
  // Clear pixels
  lcd.clear();
  
  // Now we can draw things!
  
  lcd.drawString(0, 0, mono_8_font, mono_8_width, "Hello World!");
}

void loop(){
}