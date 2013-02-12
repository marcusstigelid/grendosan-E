

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
int Switch_State[] = {HIGH, HIGH, HIGH, HIGH};  // Array for the switching state of eache socket DEFAULT is TRUE
int Switch_to_pin_array[] = {5, 6, 7, 8};       // Array for mapping states of the switches to the physical pins
int Switch_State_ControlSignal[4];              // Array for the revieved controlsignals
String control_message = "id:auth:1;0;1;1";     //The controll message string read from the wifi module
// Global Variables ------------------------------------------------


// Used to set up communication ------------------------------------
void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}
// Used to set up communication ------------------------------------


void loop() {
  // HÄR BEHÖVEER VI LÄSA IN KONTROLLSIGLNALERNA!!!!!!!
  
  // Algorithm for decoding the control message string into arrays--
  int first_semicolon = control_message.indexOf(';');
  for (int x = 0; x < 4; x++) {
    Switch_State_ControlSignal[x] = control_message.charAt(first_semicolon - 1 + x*2);
  }  
  // Algorithm for decoding the control message string into arrays--
  
  
  // Check if there are changes between Switch_State[] - Switch_State_ControlSignal[] and IF there are... SWITCH!!!!---
  for (int x = 0; x < 4; x++){
    if(Switch_State[x] != Switch_State_ControlSignal[x]){
      power_switching(x);
    }
  }
  // Check if there are changes between Switch_State[] - Switch_State_ControlSignal[] and IF there are... SWITCH!!!!---
  
  // Read the values from the analog and digital pins---------------
  for (int x = 0; x <= analogLayoutLength-1; x++){    // Read the analog pins and store them in analogReadstate
    analogReadState[x] = analogRead(analogLayout[x]);
  }

  for (int x = 0; x <= digitalLayoutLength-1; x++){  // Read the digital pins and store them in digitalReadstate
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

  // Algorithm for decoding the control message string into arrays--
  
  // Algorithm for decoding the control message string into arrays--




}

// Function for switching each socket -----------------------------
void power_switching(int socket){
  if (Switch_State[socket] == HIGH){
   Switch_State[socket] = LOW;
   digitalWrite(socket, LOW);
  }
 if (Switch_State[socket] == LOW){
  Switch_State[socket] = HIGH;
  digitalWrite(socket, HIGH);
 } 
}
// Function for switching each socket -----------------------------


