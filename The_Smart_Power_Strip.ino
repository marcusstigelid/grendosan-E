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
int sensorValue = 0;        // value read from the pot  - From example
int outputValue = 0;        // value output to the PWM (analog out) - From example
// Global Variables ------------------------------------------------


// Used to set up communication ------------------------------------
void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}
// Used to set up communication ------------------------------------


void loop() {
  
  
  
  
  // read the analog in value:
  sensorValue = analogRead(analogInPin);            
  // map it to the range of the analog out:
  outputValue = map(sensorValue, 0, 1023, 0, 255);  
  // change the analog out value:
  analogWrite(analogOutPin, outputValue);           

  // print the results to the serial monitor:
  Serial.print("sensor = " );                       
  Serial.print(sensorValue);      
  Serial.print("\t output = ");      
  Serial.println(outputValue);   

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(2);                     
}
