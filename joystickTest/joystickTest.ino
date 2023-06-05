#define Vx A0 // Define / Equate "Vx" with A0, the pin where Vx is connected
#define Vy A1 // Define / Equate "Vy" with A1, the pin where Vy is connected

void setup() {
  // put your setup code here, to run once:
  pinMode(Vx, INPUT); // Configure Vx (A0) as an Input
	pinMode(Vy, INPUT); // Configure Vy (A1) as an Input
  Serial.begin(9600); // Initialize Serial Port at 9600 baud to display the results

}

void loop() {
  // put your main code here, to run repeatedly:
  int x, y, btn;
	
	x = analogRead(Vx);	// Read the analog value of Vx (Analog Values are from 0-1023 which equate to 0V to 5V)
	y = analogRead(Vy); // Read the analog value of Vy
  Serial.print(x);	// Print the X value to the serial port
	Serial.print("\t"); // Print a Tab character 
	Serial.print(y); 	// Print the Y value
	Serial.print("\t"); // Print a Tab
	
	delay(250); // Delay 250ms so the results don't print too quickly
}
