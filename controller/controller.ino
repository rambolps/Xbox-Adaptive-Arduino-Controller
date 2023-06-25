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
#define dataIn 4;

#define ledR 5
#define ledG 6
#define ledB 8




//Global Variables
byte inputButtons[20];
bool gameMode;
byte triggerMode;

//Button Enum
enum ButtonDict{
  Button_X,
  Button_Y,
  Button_A,
  Button_B,
  Button_START,
  Button_SELECT,
  Button_LOGO,
  Button_RB,
  Button_RT,
  Button_R3,
  Button_LB,
  Button_LT,
  Button_L3,
  Button_MODE,
  Button_OPTION,
  Button_M1,
  Button_M2,
  Button_M3,
  Button_M4,
  Button_M5,
}

void setup() {
	//Start Xinput Protocal
  XInput.begin();

  //Start Serial Communication (DEBUG MODE)
  Serial.begin(9600);

  //Start Controller in Game Mode
  gameMode = true;

  //set trigger mode to normal
  //0 = normal, 1 = sticky, 2 = rapid
  triggerMode = 0;

  //Set Pin Modes For Button Inputs
  pinMode(ioSelect, OUTPUT);
  pinMode(clockPulse, OUTPUT);
  pinMode(dataIn, INPUT);

}


void loop() {

  delay(100);
  
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

  byte executeGameButtons(){
    //check if system mode was switched
    if(inputButtons[Button_MODE] == 1){
      gameMode = false;
      return 1;
    }


    //check if trigger mode was switched
    if(inputButtons[Button_OPTION] == 1){
      //change trigegr mode
      //0 = normal, 1 = sticky, 2 = rapid
      triggerMode++;

      if(triggerMode == 3){
        triggerMode = 0;
      }

      return 2;
    }

    //send macro button signals
    if

    //send gamepad button signals
    for(int i = 0; i < 13; i++){
      //set button state
      XInput.setButton(i, inputButtons[i]);
    }



  }

  byte executeProgramButtons(){
    //check if system mode was switched
    if(inputButtons[13] == 1){
      gameMode = true;
      return 1;
    }


  }
}
