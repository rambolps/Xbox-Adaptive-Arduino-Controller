#define ioSelect 2
#define clockPulse 3
#define dataIn 4


int x, y;
byte inputButtons[8];


void setup() {
  // put your setup code here, to run once:
 //Start Serial Communication (DEBUG MODE)
  Serial.begin(9600);


  //Set Pin Modes For Button Inputs
  pinMode(ioSelect, OUTPUT);
  pinMode(clockPulse, OUTPUT);
  pinMode(dataIn, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  readButtonInputs();
  executeButtons();
  delay(100);
  

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


  for(int i = 0; i < 8; i++){
    //set value of input to the array
    inputButtons[i] = digitalRead(dataIn);


    //load next bit (pulse clock)
    digitalWrite(clockPulse, LOW);
    digitalWrite(clockPulse, HIGH);
  };
};

void executeButtons() {
    if(inputButtons[0] == 1) {
      Serial.println("A")
    }

    if(inputButtons[1] == 1) {
    Serial.println("B")
    }
    
    if(inputButtons[2] == 1) {
    Serial.println("X")
    }
    
    if(inputButtons[3] == 1) {
    Serial.println("Y")
    }
    
    if(inputButtons[4] == 1) {
   Serial.println("logo")
    }
    
    if(inputButtons[5] == 1) {
   Serial.println("start")
    }
    
    if(inputButtons[6] == 1) {
    Serial.println("back")
    }
    
    if(inputButtons[7] == 1) {
    Serial.println("l3")
    }
    
    
    
};