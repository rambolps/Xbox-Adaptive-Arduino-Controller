//Digital Pin Defintions
#define ioSelect 2
#define clockPulse 3
#define dataIn 4


byte inputButtonsX[24];
byte inputButtons[20];



void setup() {
  //Start Serial Communication (DEBUG MODE)
  Serial.begin(9600);


  //Set Pin Modes For Button Inputs
  pinMode(ioSelect, OUTPUT);
  pinMode(clockPulse, OUTPUT);
  pinMode(dataIn, INPUT);
 
 
}

void loop() {
  readButtonInputs();
  //displayButtonPos();
  for(int i = 0; i < 20; i++){
    Serial.print(i);
    Serial.print(": ");
    Serial.println(inputButtons[i]);
  }
  
  delay(500);
 
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




