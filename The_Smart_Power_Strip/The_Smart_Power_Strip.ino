<<<<<<< HEAD
=======

  
>>>>>>> PhaseDiff...
/*
     The smart power strip,
 
 Written by Philip Karlsson and David Johansson
 
 2013-02-13
 
 */

#include <SPI.h>
<<<<<<< HEAD
#include <Arduino.h>
#include <Streaming.h>
#include <SoftwareSerial.h>
#include <WiFlySerial.h>
#include <PString.h>
#include <MemoryFree.h>
#include <Time.h>
#include <EEPROM.h>
//#include "Credentials.h"
=======
//#include <Configuration.h>
//#include <Debug.h>
//#include <ParsedStream.h>
//#include <SpiUart.h>
#include <WiFly.h>
#include <WiFlyClient.h>
//#include <WiFlyDevice.h>
//#include <WiFlyServer.h>
#include <Credentials.h>
#include <SoftwareSerial.h>


>>>>>>> PhaseDiff...
// Arrays used for reading the pins -------------------------------
const int analogLayout[] = {
  A0, A1, A2, A3, A4, A5 }

; // Array for the analog pins
const int digitalLayout[] = 
{
  2,3,4,5,6,7,8,9,10,11,12,13 }

; //Array for the digital pins. (These will be changed when decided what to use as Rx, Tx etc..)
// Arrays used for reading the pins -------------------------------
// Global Variables -----------------------------------------------
const int analogLayoutLength = 6; // The length of the analogLayout vector.
const int digitalLayoutLength = 12; // The length of the digitalLayout vector.
const int sampleNumber = 500;     // Number of sampels to make for each measurement
<<<<<<< HEAD

int sensorValue = 0;        // value read from the pot  - From example
int outputValue = 0;        // value output to the PWM (analog out) - From example

=======
int sensorValue = 0;        // value read from the pot  - From example
int outputValue = 0;        // value output to the PWM (analog out) - From example
const int sampleNumber = 100;     // Number of sampels to make for each measurement
>>>>>>> PhaseDiff...
int analogReadState[analogLayoutLength][sampleNumber]; // Stores the sampled vectors of the analog pins 
int digitalReadState[digitalLayoutLength]; // Stores the read state of the digital pins
int Switch_State[] = 
{
  HIGH, HIGH, HIGH, HIGH }; // Array for the switching state of eache socket DEFAULT is TRUE

int Switch_to_pin_array[] = 
{
  5, 6, 7, 8 }

; // Array for mapping states of the switches to the physical pins
int ControlSignal[4] = {1,1,1,1}; // Array for the revieved controlsignals
//String control_message = "id:auth:1;0;1;1"; //The controll message string read from the wifi module                REMOVE
String ID = "testID";
long counter = 0;
boolean bufferIsEmpty = true;
int RMS[sampleNumber-1];
int activePower[sampleNumber-1];
<<<<<<< HEAD
int phaseDiff[analogLayoutLength];
int sampleRate; // Samples/sek
=======
int phaseDiff[sampleNumber-1];
int phaseDiff[analogLayoutLength];
int sampleRate; // Samples/sek --- SKA MÄTAS UPP FÖR ATT FÅ KORREKTA UTRÄKNINGAR!!! FÖR SATAN!!!
>>>>>>> PhaseDiff...

// Global Variables ------------------------------------------------


// Will be loaded from EEPROM
<<<<<<< HEAD
char passphrase[] = "grisnils";
char ssid[] = "Davids";
const char* chan = "1"; // Channel for adhoc

//The following is for storing in EEPROM
// ID of the settings block
#define CONFIG_VERSION "ls1"

// Tell it where to store your config data in EEPROM
#define CONFIG_START 32

//char ssid[32];
//char passphrase[32];
int securityType;

struct StoreStruct {
  // This is for detection if they are our settings
  char version[4];
  // The variables of the settings
  char ssid[32];
  char passphrase[32];
  int securityType;
} 
storage = {
  CONFIG_VERSION,
  // The default values
  "Philip och Jennifers Wifi",
  "W-nånting",
  WIFLY_AUTH_WPA2_PSK
};


//Set UART pins
WiFlySerial WiFly(2,3);


//Defines server
#define SERVER "bregell.dyndns.org" //"46.239.111.148"
#define PORT 39500
#define NTP_SERVER "ntp1.sp.se"

//Buffer sizes
#define REQUEST_BUFFER_SIZE 80
#define RESPONSE_BUFFER_SIZE 100
#define TEMP_BUFFER_SIZE 100
#define INDICATOR_BUFFER_SIZE 16
#define CMD_BUFFER_SIZE 50
#define UPDATE_BUFFER_SIZE 27

//Define buffers
char bufRequest[REQUEST_BUFFER_SIZE];
char prompt[INDICATOR_BUFFER_SIZE]; 
char bufTemp[TEMP_BUFFER_SIZE];
char bufUpdate[UPDATE_BUFFER_SIZE];


//Write html page to PROGMEM
prog_char HTML_01[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
prog_char HTML_02[] PROGMEM = "\r\n<html style='font-family:Verdana,Geneva,Georgia,Chicago,Arial,Sans-serif;color:#002d80'>";
prog_char HTML_03[] PROGMEM = "Welcome to The Smart Power Strip Setup!";
prog_char HTML_04[] PROGMEM = "<br/><br> Please enter the correspondent number of your Wi-Fi";
prog_char HTML_05[] PROGMEM = " network shown in the list below. Also enter your password. <br/><br/>";
prog_char HTML_06[] PROGMEM = "<form name=\"input\" action=\"\" method=\"get\">List number:<input style='margin";
prog_char HTML_07[] PROGMEM =  "-left:5px' value='' name='nr'/><br/>Password: <input type='password' name='p' style='";
prog_char HTML_08[] PROGMEM = "margin-left:19px'/><br/><br><input type=\"submit\" value=\"Submit\"></form> <br/><br/> ";
prog_char HTML_09[] PROGMEM = "You can also manually enter your network credentials below.<p>";
prog_char HTML_10[] PROGMEM = "<form name=\"input2\" action=\"\" method=\"get\">SSID:<input style";
prog_char HTML_11[] PROGMEM = "='margin-left:5px' value='' name='nr'/><br/>Password: <input type='password'";
prog_char HTML_12[] PROGMEM = "name='p' style='margin-left:19px'/><br/>Clicking <input type=\"submit\" value=\"Submit\"";
prog_char HTML_13[] PROGMEM = "> will update the configuration and restart the board.</form> </html>";



//Write commands for AD-HOC to progmem
prog_char CMD_01[] PROGMEM = "set wlan auth 0";
prog_char CMD_02[] PROGMEM = "set ip dhcp 0";
prog_char CMD_03[] PROGMEM = "set wlan ssid Powerstrip";
prog_char CMD_04[] PROGMEM = "set wlan chan 11";
prog_char CMD_05[] PROGMEM = "set ip address 169.254.1.1";
prog_char CMD_06[] PROGMEM = "set ip remote 80";
prog_char CMD_07[] PROGMEM = "set ip local 80";
prog_char CMD_08[] PROGMEM = "set ip netmask 255.255.0.0";
prog_char CMD_09[] PROGMEM = "set wlan join 4";
prog_char CMD_10[] PROGMEM = "save";
prog_char CMD_11[] PROGMEM = "reboot";


//HTML indices
#define IDX_HTML_01      0
#define IDX_HTML_02      IDX_HTML_01 + 1
#define IDX_HTML_03      IDX_HTML_01 + 2
#define IDX_HTML_04      IDX_HTML_01 + 3
#define IDX_HTML_05      IDX_HTML_01 + 4
#define IDX_HTML_06      IDX_HTML_01 + 5
#define IDX_HTML_07      IDX_HTML_01 + 6
#define IDX_HTML_08      IDX_HTML_01 + 7
#define IDX_HTML_09      IDX_HTML_01 + 8
#define IDX_HTML_10      IDX_HTML_01 + 9
#define IDX_HTML_11      IDX_HTML_01 + 10
#define IDX_HTML_12      IDX_HTML_01 + 11
#define IDX_HTML_13      IDX_HTML_01 + 12

//Command indices
#define IDX_CMD_01      IDX_HTML_13 + 1
#define IDX_CMD_02      IDX_CMD_01 + 1
#define IDX_CMD_03      IDX_CMD_01 + 2
#define IDX_CMD_04      IDX_CMD_01 + 3
#define IDX_CMD_05      IDX_CMD_01 + 4
#define IDX_CMD_06      IDX_CMD_01 + 5
#define IDX_CMD_07      IDX_CMD_01 + 6
#define IDX_CMD_08      IDX_CMD_01 + 7
#define IDX_CMD_09      IDX_CMD_01 + 8
#define IDX_CMD_10      IDX_CMD_01 + 9
#define IDX_CMD_11      IDX_CMD_01 + 10

PROGMEM const char *WT_string_table[] =      
{  
  HTML_01,
  HTML_02,
  HTML_03,
  HTML_04,
  HTML_05,
  HTML_06,
  HTML_07,
  HTML_08,
  HTML_09,
  HTML_10,
  HTML_11,
  HTML_12,
  HTML_13,
  CMD_01,
  CMD_02,
  CMD_03,
  CMD_04,
  CMD_05,
  CMD_06,
  CMD_07,
  CMD_08,
  CMD_09,
  CMD_10,
  CMD_11
};

// GetBuffer_P
// Returns pointer to a supplied Buffer, from PROGMEM based on StringIndex provided.
char* GetBuffer_P(const int StringIndex, char* pBuffer, int bufSize) { 
  strncpy_P(pBuffer, (char*)pgm_read_word(&(WT_string_table[StringIndex])), bufSize);  
  return pBuffer; 
}
=======
char passphrase[] = "hunden123";
char ssid[] = "wifi";

//Ports for WiFly UART communication
SoftwareSerial SerialRNXV(3, 2);

// TEMP
byte server[] = {
  10, 0, 1, 3}; // Google
WiFlyClient client(server, 52492);
//WiFlyClient client(server, 80);

>>>>>>> PhaseDiff...



// Used to set up communication ------------------------------------
void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
<<<<<<< HEAD

  Serial << "RAM > setup: " << freeMemory() << endl;
=======
  SerialRNXV.begin(9600);
  
  //Set up UART communication
  WiFly.setUart(&SerialRNXV);
  //Delay for booting WiFly module
  delay(100);

  //Initialize WiFi connection to server
  WiFi_init (); 
}
// Used to set up communication ------------------------------------


// MAIN LOOP ---------------------------------------------------------------------------------------------------------
void loop() {
  if (WiFi_read()){ //Check for input from server and read
    //parse(); //Parse input
    //check_state(); //Check if states have changed
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

>>>>>>> PhaseDiff...

  //Serial << "Loading configuration from EEPROM..." << endl;
  // loadConfig();
  // Serial << "SSID: " << ssid << endl << "Passphrase: " << passphrase << endl;

<<<<<<< HEAD
  //Delay for booting WiFly module
  delay(1000);

  Serial.println(("Starting WiFly"));
  WiFly.begin();
  Serial.println("Started successfully!");

=======
void powerCalc () {

  int squaredSum[sampleNumber]={0,0,0,0,0};
  int RMS[sampleNumber-1]={0,0,0,0};
  int activePower[sampleNumber-1]={0,0,0,0};
  for (int i=0; i<=analogLayoutLength-1;i++){
    for(int j=0; j<=sampleNumber-1; j++){
      squaredSum[i]=squaredSum[i] + pow(analogReadState[i][j],2);
    }
    RMS[i]=sqrt(squaredSum[i]/sampleNumber);
  }
  phaseDifference();
  
  for (int i=1; i<=analogLayoutLength-1;i++){
    activePower[i-1]=RMS[0]*RMS[i]*cos(phaseDiff[i-1]);
  }
}

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
    delay(1);
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



  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
        while (1) {
          // Hang on failure.
        }
  }
  Serial.println("connecting...");
}
// Initialize WiFi module and join network-------------------------

// Try to read input from server ----------------------------------
boolean WiFi_read() {
  if (client.connected()) { //Check if client is connected
    if (client.available()){
    while (client.available()){ //Check if client has received data
      char temp = client.read();
      //control_message = String (control_message + client.read());
      Serial.print(temp);
      return true;
    }
    }
    else return false;
  }

  else if (!client.connected()) {
    Serial.println();
    Serial.println("reconnecting.");
    client.stop();
    client.connect();
    WiFi_read();
// Fuction for finding zero in an array --------------------------- (Part of the Phase Differance calculations)
int findZero(int init, int array[]){
  int lastValue = array[init];
  for (int i=init; i<sampleNumber; i++){
    if(((lastValue > 0) && (array[i] < 0)) || ((lastValue < 0) && (array[i] > 0)) || array[i] == 0){
      return i;
    }
  }
}
// Fuction for finding zero in an array --------------------------- (Part of the Phase Differance calculations)
>>>>>>> PhaseDiff...

  //Set up adhoc
  setup_adhoc();

<<<<<<< HEAD
  //Initialize WiFi connection to server
  //wifi_init();

  //   time_t tCurrent= (time_t) WiFly.getTime(); 
  //   setTime( tCurrent );
  //   Serial << hour() << minute()<< endl;


}

=======
  String sendStream = String(ID + ":" + activePower[0] + ";" + activePower[1] + ";" + activePower[2] + ";" + activePower[3] + ":" + Switch_State[0] + ";" + Switch_State[1] + ";" + Switch_State[2] + ";" + Switch_State[3]); // ID:10;11;12;13:1;0;0;0

  if (client.connected()) {
    Serial.println("connected");
    //client.println("GET HTTP/1.0");//sendStream);
    client.write("Please just work");
    return true;
  } 
  else if (!client.connected()) {
    return false;
  }  
}



// Fuction for finding zero in an array ---------------------------
int findZero(apvector<int> &array){
  int lastValue = array[0];
  for (int i=0; i<sampleNumber; i++){
    if((lastValue > 0 && array[i] < 0) || (lastValue < 0 && array[i] > 0) || array[i] = 0){
      return i;
    }
  }
}
// Fuction for finding zero in an array ---------------------------

// Gör om funktionen nedan......
//Function to find highest (maximum) value in array ---------------

//Function to find highest (maximum) value in array --------------- (Part of the Phase Differance calculations)
int maximumValue(int start, int stop_, int array[])
{    
     int max = array[start];       // start with max = first element
     for(int i = start; i<stop_; i++)
     {
          if(array[i] > max){
                max = array[i];
          }
     }
     return max;                // return highest value in array
}

//Function to find highest (maximum) value in array --------------- (Part of the Phase Differance calculations)



// Phase Differnece -----------------------------------------------
void phaseDifference(){
  int highestVoltageSampleNumber; //Tells on which sample the highest value of the voltage is located
  int firstzeroCrossing, secondzeroCrossing;
  int local_vector[sampleNumber]; //Local vector used for calculations
  for(int y=0; y<analogLayoutLength; y++){
    for(int x=0; x<sampleNumber; x++){
      local_vector[x] = analogReadState[y][x]; // Store in the local variable
    }
    if(y=0){
      firstzeroCrossing = findZero(50, local_vector);//Find first zero crossing in the voltage vecor with guidance from the given sample index (50 in this case )
      secondzeroCrossing = findZero((firstzeroCrossing + 40), local_vector); // the findzeroCrossing(argument 1) should be close to the end of 0----->T as guidance so 40 is probably wrong... 
      highestVoltageSampleNumber = maximumValue(firstzeroCrossing, secondzeroCrossing, local_vector); // Load the highest voltage sample
    }
    if(y!=0){
       int sampleDiff =  maximumValue(firstzeroCrossing, secondzeroCrossing, local_vector) - highestVoltageSampleNumber; //(May be positive or negative)
       phaseDiff[y] = (((sampleDiff)*sampleRate)/0.02)*360;  //Algorithm for converting samples to degrees
    }
  }
}
// Phase Difference -----------------------------------------------


>>>>>>> PhaseDiff...

