#include <XInput.h>

//Digital Pin Defintions
#define ioSelect 2
#define clockPulse 3
#define dataIn 4
#define Vx A0 // Define / Equate "Vx" with A0, the pin where Vx is connected
#define Vy A1 // Define / Equate "Vy" with A1, the pin where Vy is connected


int x, y;
byte inputButtons[8];

// Analog Input Range
const int AnalogRead_Max = 1023;  // 10-bit ADC

void setup() {
  //Start Xinput Protocal
   XInput.begin();

  //Start Serial Communication (DEBUG MODE)
  Serial.begin(9600);


  //Set Pin Modes For Button Inputs
  pinMode(ioSelect, OUTPUT);
  pinMode(clockPulse, OUTPUT);
  pinMode(dataIn, INPUT);
  XInput.setRange(JOY_LEFT, 0, 1023);
 
 
};

void loop() {
  int joystickValueX = analogRead(Vx);
  int joystickValueY = analogRead(Vy);
  XInput.setJoystick(JOY_LEFT, joystickValueX, joystickValueY);  // move x, leave y centered

 Serial.println(joystickValueX);
 Serial.println(joystickValueY);
 Serial.println(inputButtons[0]);
 Serial.println(inputButtons[1]);
 Serial.println(inputButtons[2]);
 Serial.println(inputButtons[3]);
 Serial.println(inputButtons[4]);
 Serial.println(inputButtons[5]);
 Serial.println(inputButtons[6]);
 Serial.println(inputButtons[7]);
  readButtonInputs();
  setButtonstate();
  delay(100);
 
};

void readButtonInputs(){

  //enable parallel input
  digitalWrite(ioSelect, 0);

  //start clock pin low
  digitalWrite(clockPulse, 0);

  //set clock pin high (load data into SR)
  digitalWrite(clockPulse, 1);

  //enable serial output
  digitalWrite(ioSelect, 1);


  for(int i = 0; i < 8; i++){
    //set value of input to the array
    inputButtons[i] = digitalRead(dataIn);


    //load next bit (pulse clock)
    digitalWrite(clockPulse, LOW);
    digitalWrite(clockPulse, HIGH);
  };

};
void buttonPress(XInputControl i) {
  XInput.press(i);
  delay(1000);

  XInput.release(i);

};
void setButtonstate(){
  for (int i=0; i < 8; i++){
    if (i==0){
      XInput.setButton(8, inputButtons[i]);

    }
    else {
    XInput.setButton(i, inputButtons[i]);
    }
   
  }
}
void executeButtons() {
    if(inputButtons[0] == 1) {
    buttonPress(BUTTON_A);
    }

    if(inputButtons[1] == 1) {
    buttonPress(BUTTON_B);
    }
   
    if(inputButtons[2] == 1) {
    buttonPress(BUTTON_X);
    }
   
    if(inputButtons[3] == 1) {
    buttonPress(BUTTON_Y);
    }
   
    if(inputButtons[4] == 1) {
    buttonPress(BUTTON_START);
    }
   
    if(inputButtons[5] == 1) {
    buttonPress(BUTTON_LOGO);
    }
   
    if(inputButtons[6] == 1) {
    buttonPress(BUTTON_BACK);
    }
   
    if(inputButtons[7] == 1) {
    buttonPress(BUTTON_L3);
    }
   
   
   
};

