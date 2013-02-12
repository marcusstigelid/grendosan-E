#include <SPI.h>
//#include <Configuration.h>
//#include <Debug.h>
//#include <ParsedStream.h>
//#include <SpiUart.h>
#include <WiFly.h>
#include <WiFlyClient.h>
//#include <WiFlyDevice.h>
//#include <WiFlyServer.h>




/*
 The smart power strip,
 
 Written by Philip Karlsson and David Johansson
 
 2013-02-13
 
 */


// Arrays used for reading the pins -------------------------------
const int analogLayout[] = {
  A0, A1, A2, A3, A4, A5};                // Array for the analog pins
const int digitalLayout[] = {
  2,3,4,5,6,7,8,9,10,11,12,13};          //Array for the digital pins. (These will be changed when decided what to use as Rx, Tx etc..)
// Arrays used for reading the pins -------------------------------


// Global Variables -----------------------------------------------
const int analogLayoutLength = 6; // The length of the analogLayout vector.
const int digitalLayoutLength = 12; // The length of the digitalLayout vector.
int sensorValue = 0;        // value read from the pot  - From example
int outputValue = 0;        // value output to the PWM (analog out) - From example
int analogReadState[analogLayoutLength]; // Stores the read state of the analog pins
int digitalReadState[digitalLayoutLength]; // Stores the read state of the digital pins


int switch_state[4] = {
  1, 0, 1 , 1}; 
String control_message = "test";

String ID = "testID";
// Global Variables ------------------------------------------------


// Will be loaded from EEPROM
char passphrase[] = "pass";
char ssid[] = "ssid";




// TEMP
byte server[] = { 
  66, 249, 89, 104   }; // Google

WiFlyClient client(server, 80);





// Used to set up communication ------------------------------------
void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  delay(100);
  WiFi_init (); 

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

void WiFi_read() {
  if (client.connected()) { //Check if client is connected
    if (client.available()){ //Check if client has received data
      control_message = String (control_message + client.read());
    }
  }

  else if (!client.connected()) {
    Serial.println();
    Serial.println("reconnecting.");
    client.stop();
    client.connect();
    WiFi_read();
  }
}



void send_update () {

  String sendStream = String(ID + ":" + "100" + ";" + "100" + ";" + "100" + ";" + "100" + ":" + switch_state[0] + ";" + switch_state[1] + ";" + switch_state[2] + ";" + switch_state[3]); // ID:10;11;12;13:1;0;0;0

  if (client.connected()) {
    Serial.println("connected");
    client.println(sendStream);
  } 
  else if (!client.connected()) {
    Serial.println();
    Serial.println("reconnecting.");
    client.stop();
    client.connect();
    send_update();
  }  
}




////Sketch for main loop
//
//WiFi_read();
//
////Handle input from server here
//
//counter++;
//
//if(counter>=20){
//  
////Read analog inputs
//
////Calculate active power  
//  
// counter=0;
// send_update(); 
//}


