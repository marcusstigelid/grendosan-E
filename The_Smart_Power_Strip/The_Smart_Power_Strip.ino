

/*
 The smart power strip,
 
 Written by Philip Karlsson and David Johansson
 
 2013-02-13
 
 */


// Arrays used for reading the pins -------------------------------
const int analogLayout[] = {A0, A1, A2, A3, A4, A5};                // Array for the analog pins
const int digitalLayout[] = {2,3,4,5,6,7,8,9,10,11,12,13};          //Array for the digital pins. (These will be changed when decided what to use as Rx, Tx etc..)
// Arrays used for reading the pins -------------------------------


// Global Variables -----------------------------------------------
const int analogLayoutLength = 6; // The length of the analogLayout vector.
const int digitalLayoutLength = 12; // The length of the digitalLayout vector.
int sensorValue = 0;        // value read from the pot  - From example
int outputValue = 0;        // value output to the PWM (analog out) - From example
int analogReadState[analogLayoutLength]; // Stores the read state of the analog pins
int digitalReadState[digitalLayoutLength]; // Stores the read state of the digital pins
// Global Variables ------------------------------------------------


// Used to set up communication ------------------------------------
void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}
// Used to set up communication ------------------------------------


void loop() {
  
  
  // Read the values from the analog and digital pins---------------
  for (int x = 1; x <= analogLayoutLength; x++){    // Read the analog pins and store them in analogReadstate
    analogReadState[x] = analogRead(analogLayout[x]);
  }

  for (int x = 1; x <= digitalLayoutLength; x++){  // Read the digital pins and store them in digitalReadstate
    digitalReadState[x] = digitalRead(digitalLayout[x]);
  }
  // Read the values from the analog and digital pins---------------

 
  
  // Print the values through the serial port ----------------------
  // Analog Values --------------
  Serial.println("Value on A0: "); 
  Serial.print(analogReadState[0]);
  
  Serial.println("Value on A1: "); 
  Serial.print(analogReadState[1]);
  
  Serial.println("Value on A2: "); 
  Serial.print(analogReadState[2]);
  
  Serial.println("Value on A3: "); 
  Serial.print(analogReadState[3]);
  
  Serial.println("Value on A4: "); 
  Serial.print(analogReadState[4]);
  
  Serial.println("Value on A5: "); 
  Serial.print(analogReadState[5]);
  // Analog Values --------------
  // Print the values through the serial port ----------------------
}

float powerCalc (int current[], int voltage[]) {
  
 float power = 10;
 int n= 10;
 for (int i=0; i <= n-1; i++){
   
 }
 return power;
  
 }
