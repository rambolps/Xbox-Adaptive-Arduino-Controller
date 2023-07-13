/*
 *  Project     Xbox Adaptive Arduino Controller
 *  @author     Ryan Rambali
 *  @link       https://github.com/rambolps/Xbox-Adaptive-Arduino-Controller
 */

#include <XInput.h>
#include <EEPROM.h>

//Analog Pin Defintions
#define JLX A1
#define JLY A0
#define JRX A4
#define JRY A5
#define DPX A3
#define DPY A2

//Digital Pin Defintions
#define ioSelect 2
#define clockPulse 3
#define dataIn 4

#define ledR 5
#define ledG 6
#define ledB 9

//Wait time in between program button pressed
#define WAIT_TIME 2000

//Global Variables
byte inputButtons[20];
byte inputButtonsX[24];
bool gameMode;
byte triggerMode;
bool RTON;
bool LTON;
bool RFLON;
bool RFRON;
int oldButton;
int PState;

enum PStates : uint8_t {
  Defualt,
  WaitingSB,
  S1,
  S2,
  S3,
  S4,
  S5,
};

//Button Enum
enum ButtonDict : uint8_t{
  Button_X = 1,
  Button_Y = 2,
  Button_A = 6,
  Button_B = 5,
  Button_START = 11,
  Button_SELECT = 12,
  Button_LOGO = 10,
  Button_RB = 4,
  Button_RT = 3,
  Button_R3 = 9,
  Button_LB = 7,
  Button_LT = 0,
  Button_L3 = 8,
  Button_MODE = 13,
  Button_OPTION = 14,
  Button_M1 = 15,
  Button_M2 = 16,
  Button_M3 = 17,
  Button_M4 = 18,
  Button_M5 = 19,
};

void setLed(int R, int G, int B){
  analogWrite(ledR, R);
  analogWrite(ledG, G);
  analogWrite(ledB, B);
}

void waitLED(int R, int G, int B, int W){
  setLed(255,255,255);
  delay(W);
  setLed(R,G,B);
}

void setup() {
	//Start Xinput Protocal
  XInput.begin();

  //Start Serial Communication (DEBUG MODE)
  Serial.begin(9600);

  //Start Controller in Game Mode
  gameMode = true;
  setLed(0,255,0);

  //set trigger mode to normal
  //0 = normal, 1 = sticky, 2 = rapid
  triggerMode = 0;
  RTON = false;
  LTON = false;
  RFLON = false;
  RFRON = false;
  
  //set program mode state
  PState = 0;
  

  //Set Pin Modes For Button Inputs
  pinMode(ioSelect, OUTPUT);
  pinMode(clockPulse, OUTPUT);
  pinMode(dataIn, INPUT);

  //Set Joystick Ranges
  XInput.setRange(JOY_LEFT, -512, 512);
  XInput.setRange(JOY_RIGHT, -512, 512);

  //Led Setup
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
}


void loop() {

  readButtonInputs();

  if(gameMode == true){
    
    executeJoysticks();
    executeDpad();
    executeGameButtons();
    executeTrigger();

  } else{
      bool isAButtonPressed = false;

      for(int i = 0; i < 20; i++){
        if(inputButtons[i] == 1){
          isAButtonPressed = true;
        }
      }

      if (isAButtonPressed == true){
        executeProgramButtons();
      }
  }

  delay(1);
  
}


int convertMX(int mine){
  if(mine == Button_X){
    return 3;
  }

  if(mine == Button_Y){
    return 4;
  }

  if(mine == Button_A){
    return 1;
  }

  if(mine == Button_B){
    return 2;
  }

  if(mine == Button_START){
    return 8;
  }

  if(mine == Button_SELECT){
    return 7;
  }

  if(mine == Button_LOGO){
    return 0;
  }

  if(mine == Button_RB){
    return 6;
  }

  if(mine == Button_RT){
    return 16;
  }

  if(mine == Button_LB){
    return 5;
  }

  if(mine == Button_LT){
    return 15;
  }

  if(mine == Button_R3){
    return 10;
  }

  if(mine == Button_L3){
    return 9;
  }
}

void executeJoysticks(){
  //Left Joystick
  int LJVX = analogRead(JLX);
  int LJVY = analogRead(JLY);

  LJVX -= 512;
  LJVY -= 512;

  LJVX *= -1;
  LJVY *= -1;

  if(LJVX > -5 && LJVX < 5 && LJVY > -5 && LJVY < 5){
    XInput.setJoystick(JOY_LEFT, 0, 0);
  } else{
    XInput.setJoystick(JOY_LEFT, LJVX, LJVY);
  }

  //Right Joystick
  int RJVX = analogRead(JRX);
  int RJVY = analogRead(JRY);

  RJVX -= 512;
  RJVY -= 512;

  RJVX *= 1;
  RJVY *= -1;

  if(RJVX > -5 && RJVX < 5 && RJVY > -5 && RJVY < 5){
    XInput.setJoystick(JOY_RIGHT, 0, 0);
  } else{
    XInput.setJoystick(JOY_RIGHT, RJVX, RJVY);
  }
}

int executeDpad(){
  //get x y values
  float DJVX = float(analogRead(DPX));
  float DJVY = float(analogRead(DPY));

  double distUp = sqrt(sq(512-DJVX)+sq(0-DJVY));
  double distDown = sqrt(sq(512-DJVX)+sq(1023-DJVY));
  double distRight = sqrt(sq(0-DJVX)+sq(512-DJVY));
  double distLeft = sqrt(sq(1023-DJVX)+sq(512-DJVY));

  double distTL = sqrt(sq(1023-DJVX)+sq(0-DJVY));
  double distTR = sqrt(sq(0-DJVX)+sq(0-DJVY));
  double distBL = sqrt(sq(1023-DJVX)+sq(1023-DJVY));
  double distBR = sqrt(sq(0-DJVX)+sq(1023-DJVY));

  
  double dists[8] = {distUp,distDown,distLeft,distRight, distTL, distTR, distBL, distBR};

  double smallDist = distUp;

  for(int i = 1; i < 8; i++){
    if(smallDist > dists[i]){
      smallDist = dists[i];
    }
  }

  //minimum distance
  if (smallDist > 300){
    XInput.setButton(DPAD_UP, 0);
    XInput.setButton(DPAD_DOWN, 0);
    XInput.setButton(DPAD_LEFT, 0);
    XInput.setButton(DPAD_RIGHT, 0);

    return 0;
  }

  if(smallDist == distUp){
    XInput.setButton(DPAD_UP, 1);
    XInput.setButton(DPAD_DOWN, 0);
    XInput.setButton(DPAD_LEFT, 0);
    XInput.setButton(DPAD_RIGHT, 0);
  }

  if(smallDist == distDown){
    XInput.setButton(DPAD_UP, 0);
    XInput.setButton(DPAD_DOWN, 1);
    XInput.setButton(DPAD_LEFT, 0);
    XInput.setButton(DPAD_RIGHT, 0);
  }

  if(smallDist == distLeft){
    XInput.setButton(DPAD_UP, 0);
    XInput.setButton(DPAD_DOWN, 0);
    XInput.setButton(DPAD_LEFT, 1);
    XInput.setButton(DPAD_RIGHT, 0);
  }

  if(smallDist == distRight){
    XInput.setButton(DPAD_UP, 0);
    XInput.setButton(DPAD_DOWN, 0);
    XInput.setButton(DPAD_LEFT, 0);
    XInput.setButton(DPAD_RIGHT, 1);
  }

  //two buttons
  if(smallDist == distTL){
    XInput.setButton(DPAD_UP, 1);
    XInput.setButton(DPAD_DOWN, 0);
    XInput.setButton(DPAD_LEFT, 1);
    XInput.setButton(DPAD_RIGHT, 0);
  }

  if(smallDist == distTR){
    XInput.setButton(DPAD_UP, 1);
    XInput.setButton(DPAD_DOWN, 0);
    XInput.setButton(DPAD_LEFT, 0);
    XInput.setButton(DPAD_RIGHT, 1);
  }

  if(smallDist == distBL){
    XInput.setButton(DPAD_UP, 0);
    XInput.setButton(DPAD_DOWN, 1);
    XInput.setButton(DPAD_LEFT, 1);
    XInput.setButton(DPAD_RIGHT, 0);
  }

  if(smallDist == distBR){
    XInput.setButton(DPAD_UP, 0);
    XInput.setButton(DPAD_DOWN, 1);
    XInput.setButton(DPAD_LEFT, 0);
    XInput.setButton(DPAD_RIGHT, 1);
  }
  
  return 0;
}

void resetEEPROM(){

  //set defualt values for the regular buttons
  EEPROM.update(Button_X, Button_X);
  EEPROM.update(Button_Y, Button_Y);
  EEPROM.update(Button_A, Button_A);
  EEPROM.update(Button_B, Button_B);
  EEPROM.update(Button_START, Button_START);
  EEPROM.update(Button_SELECT, Button_SELECT);
  EEPROM.update(Button_LOGO, Button_LOGO);
  EEPROM.update(Button_RB, Button_RB);
  EEPROM.update(Button_RT, Button_RT);
  EEPROM.update(Button_R3, Button_R3);
  EEPROM.update(Button_LB, Button_LB);
  EEPROM.update(Button_LT, Button_LT);
  EEPROM.update(Button_L3, Button_L3);

  //set all the macro buuttons to 100
  //100 is the "empty data" value
  for (int i = 15; i < 40; i++){
    EEPROM.update(i,100);
  }
}

void miniMacro(int mem){
  if(EEPROM.read(mem) != 100){
    XInput.setButton(convertMX(EEPROM.read(mem)), 1);
    //REDO delay to incorparate proper recorded timings
    delay(200);
    XInput.setButton(convertMX(EEPROM.read(mem)), 0);

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


  for(int i = 0; i < 24; i++){
    //set value of input to the array
    inputButtonsX[i] = digitalRead(dataIn);


    //load next bit (pulse clock)
    digitalWrite(clockPulse, LOW);
    digitalWrite(clockPulse, HIGH);
  }

  for(int i = 0; i < 16; i++){
    inputButtons[i] = inputButtonsX[i];
  }

    for(int i = 16; i < 20; i++){
    inputButtons[i] = inputButtonsX[i+4];
  }
}

int executeGameButtons(){
  //check if system mode was switched
  if(inputButtons[Button_MODE] == 1){
    gameMode = false;
    waitLED(255,0,0,WAIT_TIME);
    return 1;
  }


  //check if trigger mode was switched
  if(inputButtons[Button_OPTION] == 1){
    //change triger mode
    //0 = normal, 1 = sticky, 2 = rapid
    triggerMode++;

    if(triggerMode == 3){
      triggerMode = 0;
    }

    XInput.setTrigger(TRIGGER_LEFT, 0);
    XInput.setTrigger(TRIGGER_RIGHT, 0);
    RTON = false;
    LTON = false;
    RFLON = false;
    RFRON = false;
    waitLED(0,255,0,WAIT_TIME); 
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
      XInput.setButton(convertMX(convertMEEPROM(i)), inputButtons[i]);
    }
    
  }

}

int convertMEEPROM(int mine){
  if(mine == Button_X){
    return EEPROM.read(Button_X);
  }

  if(mine == Button_Y){
    return EEPROM.read(Button_Y);
  }

  if(mine == Button_A){
    return EEPROM.read(Button_A);
  }

  if(mine == Button_B){
    return EEPROM.read(Button_B);
  }

  if(mine == Button_START){
    return EEPROM.read(Button_START);
  }

  if(mine == Button_SELECT){
    return EEPROM.read(Button_SELECT);
  }

  if(mine == Button_LOGO){
    return EEPROM.read(Button_LOGO);
  }

  if(mine == Button_RB){
    return EEPROM.read(Button_RB);
  }

  if(mine == Button_RT){
    return EEPROM.read(Button_RT);
  }

  if(mine == Button_LB){
    return EEPROM.read(Button_LB);
  }

  if(mine == Button_LT){
    return EEPROM.read(Button_LT);
  }

  if(mine == Button_R3){
    return EEPROM.read(Button_R3);
  }

  if(mine == Button_L3){
    return EEPROM.read(Button_L3);
  }
}

void executeTrigger(){
  if (triggerMode == 0)
  {
    if (inputButtons[convertMEEPROM(Button_LT)] == 1)
    {
      XInput.setTrigger(TRIGGER_LEFT, 255);
    }
    else
    {
      XInput.setTrigger(TRIGGER_LEFT, 0);
    }
    
    if (inputButtons[convertMEEPROM(Button_RT)] == 1)
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
    if (inputButtons[convertMEEPROM(Button_LT)] == 1)
    {
      
      waitLED(0,255,0,WAIT_TIME);

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

    if (inputButtons[convertMEEPROM(Button_RT)] == 1)
    {
      waitLED(0,255,0,WAIT_TIME);
      
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

    if (inputButtons[convertMEEPROM(Button_LT)] == 1){
      RFLON = !RFLON;
      XInput.setTrigger(TRIGGER_LEFT, 0);
      waitLED(0,255,0,WAIT_TIME);
    }

    if (inputButtons[convertMEEPROM(Button_RT)] == 1){
      RFRON = !RFRON;
      XInput.setTrigger(TRIGGER_RIGHT, 0);
      waitLED(0,255,0,WAIT_TIME);
    }


    if (RFLON)
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
    
    if (RFRON)
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
  
}


int executeProgramButtons(){

  //check if system mode was switched
  if(inputButtons[Button_MODE] == 1){
    gameMode = true;
    waitLED(0,255,0,WAIT_TIME);
    return 1;
  }


  //Macro
  if(PState == Defualt){
    if(inputButtons[Button_OPTION] == 1){
      resetEEPROM();
      waitLED(255,0,0,WAIT_TIME);
      return 2;
    }

    if(inputButtons[Button_M1] == 1){
      oldButton = Button_M1;
      
      for (int i = 15; i < 20; i++){
        EEPROM.update(i,100);
      }

      waitLED(0,0,255,WAIT_TIME);
      PState = S1;
      return 0;
    }

    if(inputButtons[Button_M2] == 1){
      oldButton = Button_M2;
      
      for (int i = 20; i < 25; i++){
        EEPROM.update(i,100);
      }

      waitLED(0,0,255,WAIT_TIME);
      PState = S1;
      return 0;
    }

    if(inputButtons[Button_M3] == 1){
      oldButton = Button_M3;
      
      for (int i = 25; i < 30; i++){
        EEPROM.update(i,100);
      }

      waitLED(0,0,255,WAIT_TIME);
      PState = S1;
      return 0;
    }

    if(inputButtons[Button_M4] == 1){
      oldButton = Button_M4;
      
      for (int i = 30; i < 35; i++){
        EEPROM.update(i,100);
      }

      waitLED(0,0,255,WAIT_TIME);
      PState = S1;
      return 0;
    }

    if(inputButtons[Button_M5] == 1){
      oldButton = Button_M5;
      
      for (int i = 35; i < 40; i++){
        EEPROM.update(i,100);
      }

      waitLED(0,0,255,WAIT_TIME);
      PState = S1;
      return 0;
    }
  }

  if(PState == S1){
    if(inputButtons[Button_OPTION] == 1){
      PState = Defualt;
      waitLED(255,0,0,WAIT_TIME);
      return 2;
    }

    for(int i = 0; i < 15; i++){
      if(inputButtons[i] == 1){
        if(oldButton == Button_M1){
          EEPROM.update(15, i);
        }

        if(oldButton == Button_M2){
          EEPROM.update(20, i);
        }

        if(oldButton == Button_M3){
          EEPROM.update(25, i);
        }

        if(oldButton == Button_M4){
          EEPROM.update(30, i);
        }

        if(oldButton == Button_M5){
          EEPROM.update(35, i);
        }
        waitLED(255,0,255,WAIT_TIME);
        PState = S2;
        return 0;
      }
    }
  }

  if(PState == S2){
    if(inputButtons[Button_OPTION] == 1){
      PState = Defualt;
      waitLED(255,0,0,WAIT_TIME);
      return 2;
    }

    for(int i = 0; i < 15; i++){
      if(inputButtons[i] == 1){
        if(oldButton == Button_M1){
          EEPROM.update(16, i);
        }

        if(oldButton == Button_M2){
          EEPROM.update(21, i);
        }

        if(oldButton == Button_M3){
          EEPROM.update(26, i);
        }

        if(oldButton == Button_M4){
          EEPROM.update(31, i);
        }

        if(oldButton == Button_M5){
          EEPROM.update(36, i);
        }
        waitLED(255,128,0,WAIT_TIME);
        PState = S3;
        return 0;
      }
    }
  }

  if(PState == S3){
    if(inputButtons[Button_OPTION] == 1){
      PState = Defualt;
      waitLED(255,0,0,WAIT_TIME);
      return 2;
    }

    for(int i = 0; i < 15; i++){
      if(inputButtons[i] == 1){
        if(oldButton == Button_M1){
          EEPROM.update(17, i);
        }

        if(oldButton == Button_M2){
          EEPROM.update(22, i);
        }

        if(oldButton == Button_M3){
          EEPROM.update(27, i);
        }

        if(oldButton == Button_M4){
          EEPROM.update(32, i);
        }

        if(oldButton == Button_M5){
          EEPROM.update(37, i);
        }

        waitLED(0,255,255,WAIT_TIME);
        PState = S4;
        return 0;
      }  
    }
  }

  if(PState == S4){
    if(inputButtons[Button_OPTION] == 1){
      PState = Defualt;
      waitLED(255,0,0,WAIT_TIME);
      return 2;
    }

    for(int i = 0; i < 15; i++){
      if(inputButtons[i] == 1){
        if(oldButton == Button_M1){
          EEPROM.update(18, i);
        }

        if(oldButton == Button_M2){
          EEPROM.update(23, i);
        }

        if(oldButton == Button_M3){
          EEPROM.update(28, i);
        }

        if(oldButton == Button_M4){
          EEPROM.update(33, i);
        }

        if(oldButton == Button_M5){
          EEPROM.update(38, i);
        }
        waitLED(255,255,0,WAIT_TIME);
        PState = S5;
        return 0;
      }
    }
  }

  if(PState == S5){
    if(inputButtons[Button_OPTION] == 1){
      PState = Defualt;
      waitLED(255,0,0,WAIT_TIME);
      return 2;
    }

    for(int i = 0; i < 15; i++){

      if(inputButtons[i] == 1){
        if(oldButton == Button_M1){
          EEPROM.update(19, i);
        }

        if(oldButton == Button_M2){
          EEPROM.update(24, i);
        }

        if(oldButton == Button_M3){
          EEPROM.update(29, i);
        }

        if(oldButton == Button_M4){
          EEPROM.update(34, i);
        }

        if(oldButton == Button_M5){
          EEPROM.update(39, i);
        }
        PState = Defualt;
        waitLED(255,0,0,WAIT_TIME);
        return 0;
      }
    }
  }
}

