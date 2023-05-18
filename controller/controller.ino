/*
 *  Project     Xbox Adaptive Arduino Controller
 *  @author     Ryan Rambali
 *  @link       https://github.com/rambolps/Xbox-Adaptive-Arduino-Controller
 */

#include <XInput.h>
#include <EEPROM.h>


void setup() {
	XInput.begin();
  Serial.begin(9600);
}


void loop() {
	XInput.press(BUTTON_A);
	delay(1000);

	XInput.release(BUTTON_A);
	delay(1000);

  Serial.println(EEPROM.length());
  delay(1000);  
}
