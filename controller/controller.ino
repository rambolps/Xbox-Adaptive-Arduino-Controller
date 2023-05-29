/*
 *  Project     Xbox Adaptive Arduino Controller
 *  @author     Ryan Rambali
 *  @link       https://github.com/rambolps/Xbox-Adaptive-Arduino-Controller
 */

#include <XInput.h>
#include <EEPROM.h>

//Analog Pin Defintions
#define JLX 0
#define JLY 1
#define JRX 2
#define JRY 3
#define DPX 4
#define DPY 5

//Digital Pin Defintions
#define ioSelect 2
#define clockPulse 3
#define dataOut 4;

#define ledR 5
#define ledG 6
#define ledB 8

//Global Variables
byte inputButtons[20];
bool gameMode;


void setup() {
	//Start Xinput Protocal
  XInput.begin();

  //Start Serial Communication (DEBUG MODE)
  Serial.begin(9600);

  //Start Controller in Game Mode
  gameMode = true;

  //Set Button Pins to Input
  for(int i = 0; i < 15; i++){
    pinMode(i, INPUT_PULLUP);
  }

  //Set Pin Modes For Button Inputs
  pinMode(ioSelect, OUTPUT);
  pinMode(clockPulse, OUTPUT);
  pinMode(dataIn, INPUT);

}


void loop() {

  delay(100);

  for(int i= 0; i < 13; i++){
    readAndSetButtonInput(i);
  }
  
}

void resetEEPROM(){

  //set defualt values for the regular buttons
  for(int i = 0; i < 15; i++){
    EEPROM.update(i,i);
  }

  //set all the macro buuttons to 100
  //100 is the "empty data" value
  for (int i = 15; i < 40; i++){
    EEPROM.update(i,100);
  }
}


void readButtonInputs(){

  //enable parallel input
  digitalWrite(ioSelect, 0);

  //start clock pin low
  digitalWrite(clockPulse, 0);

  //set clock pin high (load data into SR)
  digitalWrite(clockPulse, 1);

  //enable serial output
  digitalWrite(ioSelect, 1);


  for(int i = 0; i < 20; i++){
    //set value of input to the array
    inputButtons[i] = digitalRead(dataIn);

    //load next bit (pulse clock)
    digitalWrite(clockPulse, LOW);
    digitalWrite(clockPulse, HIGH);
  }
}
