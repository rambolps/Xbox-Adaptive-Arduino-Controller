/*
 *  Project     Xbox Adaptive Arduino Controller
 *  @author     Ryan Rambali
 *  @link       https://github.com/rambolps/Xbox-Adaptive-Arduino-Controller
 */

#include <XInput.h>
#include <EEPROM.h>

//Analog Pin Defintions
#define JLV 0
#define JLH 1
#define JRV 2
#define JRH 3
#define DPV 4
#define DPH 5

//Digital Pin Defintions
#define Pin_LOGO 0
#define Pin_A 1
#define Pin_B 2
#define Pin_X 3
#define Pin_Y 4
#define Pin_LB 5
#define Pin_RB 6
#define Pin_BACK 7
#define Pin_START 8
#define Pin_L3 9
#define Pin_R3 10
#define Pin_LT 11
#define Pin_RT 12
#define PIN_MODE 13
#define PIN_STICKY 14
#define PIN_LED 15


//Global Variables
byte buttons[19];
byte pinToButton[13] = {0,1,2,3,4,5,6,7,8,9,10,15,16};
bool gameMode;


void setup() {
	//Start Xinput Protocal
  XInput.begin();

  //Start Serial Communication (DEBUG MODE)
  Serial.begin(9600);


  //Set Button Layout from EEPROM
  setLayout();

  //Start Controller in Game Mode
  gameMode = true;

  //Set Button Pins to Input
  for(int i = 0; i < 15; i++){
    pinMode(i, INPUT_PULLUP);
  }

  //set LED Pin to Output
  pinMode(PIN_LED, OUTPUT);

}


void loop() {

  delay(100);

  for(int i= 0; i < 13; i++){
    readAndSetButtonInput(i);
  }
  
}

void resetEEPROM(){

  for(int i = 0; i < 19; i++){
    EEPROM.update(i,i);
  }
}

void setLayout(){
  for(int i = 0; i < 19; i++){
    buttons[i] = EEPROM.read(i);
  }
}

void readAndSetButtonInput(byte pin){
  bool press = !digitalRead(pin)
  XInput.setButton(pinToButton[pin], temp);
}
