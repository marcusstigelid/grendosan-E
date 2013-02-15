

/*
 The smart power strip,
 
 Written by Philip Karlsson and David Johansson
 
 2013-02-13
 
 */

#include <SPI.h>
//#include <Configuration.h>
//#include <Debug.h>
//#include <ParsedStream.h>
//#include <SpiUart.h>
#include <WiFly.h>
#include <WiFlyClient.h>
//#include <WiFlyDevice.h>
//#include <WiFlyServer.h>


// Arrays used for reading the pins -------------------------------
const int analogLayout[] = {
  A0, A1, A2, A3, A4, A5};                // Array for the analog pins
const int digitalLayout[] = {
  2,3,4,5,6,7,8,9,10,11,12,13};          //Array for the digital pins. (These will be changed when decided what to use as Rx, Tx etc..)
// Arrays used for reading the pins -------------------------------


// Global Variables -----------------------------------------------
<<<<<<< HEAD
const int analogLayoutLength = 6; // The length of the analogLayout vector.
const int digitalLayoutLength = 12; // The length of the digitalLayout vector.
int sensorValue = 0;        // value read from the pot  - From example
int outputValue = 0;        // value output to the PWM (analog out) - From example
int analogReadState[analogLayoutLength]; // Stores the read state of the analog pins
int digitalReadState[digitalLayoutLength]; // Stores the read state of the digital pins
=======
const int analogLayoutLength = 5; // The length of the analogLayout vector.
const int digitalLayoutLength = 12; // The length of the digitalLayout vector.
int sensorValue = 0;        // value read from the pot  - From example
int outputValue = 0;        // value output to the PWM (analog out) - From example
const int sampleNumber = 500;     // Number of sampels to make for each measurement
int analogReadState[analogLayoutLength][sampleNumber]; // Stores the sampled vectors of the analog pins 
int digitalReadState[digitalLayoutLength]; // Stores the read state of the digital pins
int Switch_State[] = {
  HIGH, HIGH, HIGH, HIGH};  // Array for the switching state of eache socket DEFAULT is TRUE
int Switch_to_pin_array[] = {
  5, 6, 7, 8};       // Array for mapping states of the switches to the physical pins
int Switch_State_ControlSignal[4];              // Array for the revieved controlsignals
String control_message = "id:auth:1;0;1;1";     //The controll message string read from the wifi module
String ID = "testID";
int counter = 0;
boolean bufferIsEmpty = true;
>>>>>>> origin/master
// Global Variables ------------------------------------------------


// Will be loaded from EEPROM
char passphrase[] = "pass";
char ssid[] = "ssid";


// TEMP
byte server[] = {
  66, 249, 89, 104}; // Google

WiFlyClient client(server, 80);
//TEMP




// Used to set up communication ------------------------------------
void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  //Delay for booting WiFly module
  delay(100);
  //Initialize WiFi connection to server
  WiFi_init (); 

}
// Used to set up communication ------------------------------------


// MAIN LOOP ---------------------------------------------------------------------------------------------------------
void loop() {
<<<<<<< HEAD
  
  
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
=======

  if (WiFi_read()){ //Check for input from server and read
    parse(); //Parse input
    check_state(); //Check if states have changed
  }

  counter++;
  if(counter>=20){

    sample(); // Sample inputs

    //powerCalc();//Calculate active power  

    //Send calculated values and state
    if(bufferIsEmpty){ //Send data if buffer is empty 
      if(!send_update()){ //Try to send data and add to buffer when failure
        //addToBuffer(); //Add measurements to buffer if they can't be sent
        
        //Reconnect if send_update fails (???)
        Serial.println();
        Serial.println("reconnecting.");
        client.stop();
        client.connect();
      }
      counter=0;
    }
    else{ // If buffer is not empty then add latest data to buffer then try to send buffered values
      //addToBuffer();
      //sendBuffered();
    }
  }
}


//METHODS -------------------------------------------------------------------------------------------------------------

// Algorithm for parsing the control message string into arrays--
void parse (){
  int first_semicolon = control_message.indexOf(';');
  for (int x = 0; x < 4; x++) {
    Switch_State_ControlSignal[x] = control_message.charAt(first_semicolon - 1 + x*2);
  }  
}
// Algorithm for parsing the control message string into arrays--


// Check if there are changes between Switch_State[] - Switch_State_ControlSignal[] and IF there are... SWITCH!!!!---
void check_state(){
  for (int x = 0; x < 4; x++){
    if(Switch_State[x] != Switch_State_ControlSignal[x]){
      power_switching(x);
    }
  }
}
// Check if there are changes between Switch_State[] - Switch_State_ControlSignal[] and IF there are... SWITCH!!!!---


//Samples the analog inputs <sampleNumber> times -----------------
void sample () {
  for (int i = 0; i <= sampleNumber-1 ; i++){
    for (int x = 0; x <= analogLayoutLength-1; x++){    // Read the analog pins and store them in analogReadstate
      analogReadState[x][i] = analogRead(analogLayout[x]);
    }
    delay(0.001);
  }
}
//Samples the analog inputs <sampleNumber> times -----------------


// Read the values from the analog and digital pins---------------
//void read_pins(){
//  for (int x = 0; x <= analogLayoutLength-1; x++){    // Read the analog pins and store them in analogReadstate
//    analogReadState[x] = analogRead(analogLayout[x]);
//  }
//
//  for (int x = 0; x <= digitalLayoutLength-1; x++){  // Read the digital pins and store them in digitalReadstate
//    digitalReadState[x] = digitalRead(digitalLayout[x]);
//  }
//}
// Read the values from the analog and digital pins---------------


// Print the values through the serial port ----------------------
// Analog Values --------------
//void print_values(){
//
//  Serial.println("Value on A0: "); 
//  Serial.print(analogReadState[0]);
//
//  Serial.println("Value on A1: "); 
//  Serial.print(analogReadState[1]);
//
//  Serial.println("Value on A2: "); 
//  Serial.print(analogReadState[2]);
//
//  Serial.println("Value on A3: "); 
//  Serial.print(analogReadState[3]);
//
//  Serial.println("Value on A4: "); 
//  Serial.print(analogReadState[4]);
//
//  Serial.println("Value on A5: "); 
//  Serial.print(analogReadState[5]);
//}
// Analog Values --------------
// Print the values through the serial port ----------------------


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


// Initialize WiFi module and join network-------------------------
void WiFi_init () {

  Serial.println("[Wifly]");

  WiFly.begin();

  Serial.println("finished beginning");

  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
    //    while (1) {
    //      // Hang on failure.
    //    }
  }
  Serial.println("connecting...");
}
// Initialize WiFi module and join network-------------------------

// Try to read input from server ----------------------------------
boolean WiFi_read() {
  if (client.connected()) { //Check if client is connected
    if (client.available()){ //Check if client has received data
      control_message = String (control_message + client.read());
      return true;
    }
    else return false;
  }

  else if (!client.connected()) {
    Serial.println();
    Serial.println("reconnecting.");
    client.stop();
    client.connect();
    WiFi_read();
  }
}
// Try to read input from server ----------------------------------


// Try to send data to server -------------------------------------
boolean send_update () {

  String sendStream = String(ID + ":" + "100" + ";" + "100" + ";" + "100" + ";" + "100" + ":" + Switch_State[0] + ";" + Switch_State[1] + ";" + Switch_State[2] + ";" + Switch_State[3]); // ID:10;11;12;13:1;0;0;0

  if (client.connected()) {
    Serial.println("connected");
    client.println(sendStream);
    return true;
  } 
  else if (!client.connected()) {
    return false;
  }  
}
// Try to send data to server -------------------------------------






>>>>>>> origin/master
