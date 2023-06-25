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
bool RTON;
bool LTON;

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
  RTON = false;
  LTON = false;
  

  //Set Pin Modes For Button Inputs
  pinMode(ioSelect, OUTPUT);
  pinMode(clockPulse, OUTPUT);
  pinMode(dataIn, INPUT);

}


void loop() {

  delay(1);
  
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

void miniMacro(byte mem){
  if(EEPROM.read(mem) != 100){
    XInput.setButton(EEPROM.read(mem), 1);
    //REDO delay to incorparate proper recorded timings
    delay(10);
    XInput.setButton(EEPROM.read(mem), 0);

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

      XInput.setTrigger(TRIGGER_LEFT, 0);
      XInput.setTrigger(TRIGGER_RIGHT, 0);
      RTON = false;
      LTON = false;
      return 2;
    }

    //send macro button signals
    if(inputButtons[Button_M1] == 1){
      for(int i = 15; i < 20; i++){
        miniMacro(i);
      }
      return 0;
    }

    if(inputButtons[Button_M2] == 1){
      for(int i = 20; i < 25; i++){
        miniMacro(i);
      }
      return 0;
    }

    if(inputButtons[Button_M3] == 1){
      for(int i = 25; i < 30; i++){
        miniMacro(i);
      }
      return 0;
    }

    if(inputButtons[Button_M4] == 1){
      for(int i = 30; i < 35; i++){
        miniMacro(i);
      }
      return 0;
    }

    if(inputButtons[Button_M5] == 1){
      for(int i = 35; i < 40; i++){
        miniMacro(i);
      }
      return 0;
    }

    //send gamepad button signals
    for(int i = 0; i < 13; i++){
      //set button state
      if (i != Button_LT && i != Button_RT)
      {
        XInput.setButton(i, inputButtons[i]);
      }
      
    }

  }

  void executeTrigger(){
    if (triggerMode == 0)
    {
      if (inputButtons[Button_LT] == 1)
      {
        XInput.setTrigger(TRIGGER_LEFT, 255);
      }
      else
      {
        XInput.setTrigger(TRIGGER_LEFT, 0);
      }
      
	    if (inputButtons[Button_RT] == 1)
      {
        XInput.setTrigger(TRIGGER_RIGHT, 255);
      }
      else
      {
        XInput.setTrigger(TRIGGER_RIGHT, 0);
      }
    }
    else if (triggerMode == 1)
    {
      if (inputButtons[Button_LT] == 1)
      {
        if (LTON)
        {
          XInput.setTrigger(TRIGGER_LEFT, 0);
          LTON = false;
        } else
        {
          XInput.setTrigger(TRIGGER_LEFT, 255);
          LTON = true;
        }
      }

      if (inputButtons[Button_RT] == 1)
      {
        if (RTON)
        {
          XInput.setTrigger(TRIGGER_RIGHT, 0);
          RTON = false;
        } else
        {
          XInput.setTrigger(TRIGGER_RIGHT, 255);
          RTON = true;
        }
      }
    }
    else
    {
      if (LTON)
      {
        XInput.setTrigger(TRIGGER_LEFT, 0);
        LTON = false;
      } else
      {
        XInput.setTrigger(TRIGGER_LEFT, 255);
        LTON = true;
      }

      if (RTON)
      {
        XInput.setTrigger(TRIGGER_RIGHT, 0);
        RTON = false;
      } else
      {
        XInput.setTrigger(TRIGGER_RIGHT, 255);
        RTON = true;
      }
    }
    
  }

  byte executeProgramButtons(){
    //check if system mode was switched
    if(inputButtons[Button_MODE] == 1){
      gameMode = true;
      return 1;
    }

    if(inputButtons[Button_OPTION] == 1){
      resetEEPROM();
      return 2;
    }
  }
}
