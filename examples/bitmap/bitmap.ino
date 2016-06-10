#include <SPI.h>
#include <HGS2561281.h>
#include "rose.h"

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
  
  lcd.drawPixels(0, 0, rose_width, rose_height, rose_pixels);
}

void loop(){
}