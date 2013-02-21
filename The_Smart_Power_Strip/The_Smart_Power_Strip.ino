/*
     The smart power strip,
 
 Written by Philip Karlsson and David Johansson
 
 2013-02-13
 
 */

#include <SPI.h>
#include <Arduino.h>
#include <Streaming.h>
#include <SoftwareSerial.h>
#include <WiFlySerial.h>
#include <PString.h>
#include <MemoryFree.h>
#include <Time.h>
#include <EEPROM.h>
//#include "Credentials.h"
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
const int analogLayoutLength = 5; // The length of the analogLayout vector.
const int digitalLayoutLength = 12; // The length of the digitalLayout vector.
const int sampleNumber = 100; // Number of sampels to make for each measurement
int analogReadState[analogLayoutLength][sampleNumber]; // Stores the sampled vectors of the analog pins 
int digitalReadState[digitalLayoutLength]; // Stores the read state of the digital pins
int Switch_State[] = 
{
  HIGH, HIGH, HIGH, HIGH }

; // Array for the switching state of eache socket DEFAULT is TRUE
int Switch_to_pin_array[] = 
{
  5, 6, 7, 8 }

; // Array for mapping states of the switches to the physical pins
int Switch_State_ControlSignal[4]; // Array for the revieved controlsignals
String control_message = "id:auth:1;0;1;1"; //The controll message string read from the wifi module
String ID = "testID";
int counter = 0;
boolean bufferIsEmpty = true;
// Global Variables ------------------------------------------------


// Will be loaded from EEPROM
//char passphrase[] = "hunden123";
//char ssid[] = "wifi";
const char* chan = "6"; // Channel for adhoc

//The following is for storing in EEPROM
// ID of the settings block
#define CONFIG_VERSION "ls1"

// Tell it where to store your config data in EEPROM
#define CONFIG_START 32

char ssid[32];
char passphrase[32];
int securityType;

struct StoreStruct {
  // This is for detection if they are our settings
  char version[4];
  // The variables of the settings
  char ssid[32];
  char passphrase[32];
  int securityType;
} storage = {
  CONFIG_VERSION,
  // The default values
  "Philip och Jennifers Wifi",
  "W-nånting",
  WIFLY_AUTH_WPA2_PSK
};



// Pins are 3 for INCOMING TO Arduino, 5 for OUTGOING TO Wifly
// Arduino       WiFly
//  2 - receive  TX   (Send from Wifly, Receive to Arduino)
//  3 - send     RX   (Send from Arduino, Receive to WiFly) 


WiFlySerial WiFly(3,2);
// TEMP
//byte server[] = {10, 0, 1, 3};
//WiFlyClient client(server, 52492);
//TEMP


#define SERVER "10.0.1.3"

#define REQUEST_BUFFER_SIZE 80
#define RESPONSE_BUFFER_SIZE 300 // Will probably have to be much larger
#define TEMP_BUFFER_SIZE 80

char bufRequest[REQUEST_BUFFER_SIZE];
char bufTemp[TEMP_BUFFER_SIZE];

//prog_char s_WT_HTML_HEAD_01[] PROGMEM = "HTTP/1.1 200 OK \r ";
//prog_char s_WT_HTML_HEAD_02[] PROGMEM = "Content-Type: text/html;charset=UTF-8\r ";
//prog_char s_WT_HTML_HEAD_03[] PROGMEM = " Content-Length: ";
//prog_char s_WT_HTML_HEAD_04[] PROGMEM = "Connection: close \r\n\r\n ";
//prog_char s_WT_HTML_HEAD_05[] PROGMEM = "<META HTTP-EQUIV=\"REFRESH\" CONTENT=\"30\">";
//
//#define IDX_WT_HTML_HEAD_01      0
//#define IDX_WT_HTML_HEAD_02      IDX_WT_HTML_HEAD_01 + 1
//#define IDX_WT_HTML_HEAD_03      IDX_WT_HTML_HEAD_01 + 2
//#define IDX_WT_HTML_HEAD_04      IDX_WT_HTML_HEAD_01 + 3
//#define IDX_WT_HTML_HEAD_05      IDX_WT_HTML_HEAD_01 + 4
//
//PROGMEM const char *WT_string_table[] =      
//{   
//  s_WT_HTML_HEAD_01,
//  s_WT_HTML_HEAD_02,
//  s_WT_HTML_HEAD_03,
//  s_WT_HTML_HEAD_04,
//  s_WT_HTML_HEAD_05
//};

// GetBuffer_P
// Returns pointer to a supplied Buffer, from PROGMEM based on StringIndex provided.
// based on example from http://arduino.cc/en/Reference/PROGMEM
// Reduced need with Arduino's 1.0 F().  

//char* GetBuffer_P(const int StringIndex, char* pBuffer, int bufSize) { 
//  strncpy_P(pBuffer, (char*)pgm_read_word(&(WT_string_table[StringIndex])), bufSize);  
//  return pBuffer; 
//}

//Sass!?
//#define REQUEST_BUFFER_SIZE 120
//#define HEADER_BUFFER_SIZE 150
//#define BODY_BUFFER_SIZE 100 
//char bufRequest[REQUEST_BUFFER_SIZE];
//char bufHeader[HEADER_BUFFER_SIZE];
//char bufBody[BODY_BUFFER_SIZE];
// Used to set up communication ------------------------------------
void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  Serial << "Available RAM at setup: " << freeMemory() << endl;

  Serial << "Loading configuration from EEPROM..." << endl;
  loadConfig();
  Serial << "SSID: " << ssid << endl << "Passphrase: " << passphrase << endl;

  //Delay for booting WiFly module
  delay(1000);

  Serial.println(("Starting WiFly"));
  WiFly.begin();
  Serial.println("WiFly started successfully!");


  //Set up adhoc
  setup_adhoc();

  //Initialize WiFi connection to server
  wifi_init();


}

// MAIN LOOP ---------------------------------------------------------------------------------------------------------
void loop() 
{
  Serial << "Available RAM at loop: " << freeMemory() << endl;
  if(wifi_read()) 
  { //Check for input from server and read
    Serial << "Read successfull!"<< endl;
    //parse(); //Parse input
    //check_state(); //Check if states have changed
  }

  else 
  {
    Serial << "Read failed"<<endl;
    wifi_reconnect(false);
  }

  counter++;
  if(counter>=400) 
  {
    //sample(); // Sample inputs
    //powerCalc();//Calculate active power  
    //Send calculated values and state
    if(bufferIsEmpty) 
    { //Send data if buffer is empty 
      if(!wifi_send()) 
      { //Try to send data and add to buffer when failure
        //addToBuffer(); //Add measurements to buffer if they can't be sent
        //Reconnect if send_update fails (???)
        wifi_reconnect(false);
      }

      else 
      {
        Serial << "Message sent!";
      }

      counter=0;
    }

    else 
    { // If buffer is not empty then add latest data to buffer then try to send buffered values
      //addToBuffer();
      //sendBuffered();
    }

  }

}

//METHODS -------------------------------------------------------------------------------------------------------------
// Algorithm for parsing the control message string into arrays--
void parse() 
{
  int first_semicolon = control_message.indexOf(';');
  for(int x = 0; x < 4; x++)
  {
    Switch_State_ControlSignal[x] = control_message.charAt(first_semicolon - 1 + x*2);
  }

}

// Check if there are changes between Switch_State[] - Switch_State_ControlSignal[] and IF there are... SWITCH!!!!---
void check_state() 
{
  for(int x = 0; x < 4; x++)
  {
    if(Switch_State[x] != Switch_State_ControlSignal[x]) 
    {
      power_switching(x);
    }

  }

}

//Samples the analog inputs <sampleNumber> times -----------------
void sample() 
{
  for(int i = 0; i <= sampleNumber-1 ; i++)
  {
    for(int x = 0; x <= analogLayoutLength-1; x++)
    { // Read the analog pins and store them in analogReadstate
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
void power_switching(int socket) 
{
  if(Switch_State[socket] == HIGH) 
  {
    Switch_State[socket] = LOW;
    digitalWrite(socket, LOW);
  }

  if(Switch_State[socket] == LOW) 
  {
    Switch_State[socket] = HIGH;
    digitalWrite(socket, HIGH);
  }

}

// Initialize WiFi module and join network-------------------------
boolean wifi_init() 
{


  Serial << "Trying to scan..." << endl;
  char* pNetScan;
  const int buflen = 200;
  char* scan = WiFly.showNetworkScan(pNetScan,buflen);
  Serial << scan << endl;

  // TEST THIS!

  WiFly.setAuthMode( WIFLY_AUTH_WPA2_PSK);
  WiFly.setJoinMode( WIFLY_JOIN_AUTO );
  WiFly.setDHCPMode( WIFLY_DHCP_ON );
  // First we set the SSID (putting the SSID on the join command doesn't work
  char prompt[80];
  Serial.println("Setting SSID using Ron method");
  WiFly.SendCommandSimple("set wlan ssid ",prompt);
  WiFly.SendCommandSimple(ssid,prompt);
  Serial.println("Setting Passphrase using Ron method");
  WiFly.SendCommandSimple("set wlan phrase ",prompt);
  WiFly.SendCommandSimple(passphrase,prompt);  
  if (WiFly.SendCommandSimple("join", prompt)) {
    // TODO: Extract information from complete response?
    // TODO: Change this to still work when server mode not active

    Serial << "Success?" << endl;
  }

  else{
    Serial.println("Setting up stuff...");
    WiFly.setAuthMode( WIFLY_AUTH_WPA2_PSK);
    WiFly.setJoinMode( WIFLY_JOIN_AUTO );
    WiFly.setDHCPMode( WIFLY_DHCP_ON );
    WiFly.getDeviceStatus();
    if(! WiFly.isifUp() ) 
    {
      Serial << "Leave:"<< ssid << WiFly.leave() << endl;
      // join
      if(WiFly.setSSID(ssid)) 
      {
        Serial << "SSID Set :"<< ssid << endl;
      }

      if(WiFly.setPassphrase(passphrase)) 
      {
        Serial << "Passphrase Set :"<< endl;
      }

      Serial << "Joining... :"<< ssid << endl;
      if( WiFly.join() ) 
      {
        Serial << F("Joined ") << ssid << F(" successfully.") << endl;
        //WiFly.setNTP( ntp_server ); //NTP server
      }

      else 
      {
        Serial << F("Join to ") << ssid << F(" failed.") << endl << "You got hanged";
        while(true) 
        {
          //Hang
        }

      }

    }
  }

  // Clear out prior requests.
  WiFly.flush();
  while(WiFly.available()) WiFly.read();
  WiFly.closeConnection(); //close any open connections
  // Try to connect to server
  if(!WiFly.openConnection(SERVER)) 
  {
    Serial.println("Failed to connect to server");
    wifi_reconnect(true);
    return false;
  }

  else Serial.println("Connection successfull!");
  return true;
}

// Try to reconnect to server -------------------------------------
boolean wifi_reconnect(boolean isReInitialized) 
{
  if(!isReInitialized) 
  {
    Serial << "Reconnecting..."<< endl;
    if(WiFly.openConnection(SERVER)) 
    {
      Serial << "Connected!"<< endl;
    }

    else 
    {
      Serial << "Connection to server failed"<< endl << "Trying to re-initialize WiFly"<< endl;
      if(!wifi_init()) 
      {
        Serial << "All evasive manouvers have failed: You got hanged!"<< endl;
        while(true) 
        {
          //Hang
        }

      }

    }

  }

}

// Try to read input from server ----------------------------------
boolean wifi_read() 
{
  if(WiFly.isConnectionOpen()) 
  { //Check if client is connected
    while(WiFly.isConnectionOpen() && WiFly.available()>-1) 
    { //Check if client has received data
      char temp = WiFly.read();
      //control_message = String (control_message + client.read());
      Serial.print(temp);
      return true;
    }

  }

  else return false;
}

// Try to send data to server -------------------------------------
boolean wifi_send() 
{
  String sendStream = String(ID + ":"+ "100"+ ";"+ "100"+ ";"+ "100"+ ";"+ "100"+ ":"+ Switch_State[0] + ";"+ Switch_State[1] + ";"+ Switch_State[2] + ";"+ Switch_State[3]); // ID:10;11;12;13:1;0;0;0
  if(WiFly.isConnectionOpen()) 
  {
    Serial.println("Still connected: writing data to server.");
    //client.println("GET HTTP/1.0");//sendStream);
    WiFly.write("Please just work");
    return true;
  }

  else return false;
}

// Set up ADHOC network -------------------------------------------
void setup_adhoc(){
  Serial << "Setting up ADHOC" << endl;
  WiFly.setIP("192.168.0.1");
  WiFly.setNetMask("255.255.255.0");
  WiFly.setSSID("Powerstrip");
  WiFly.setPassphrase("passphrase");

  WiFly.setChannel(chan);
  WiFly.setDHCPMode(WIFLY_DHCP_OFF);
  WiFly.setAuthMode(WIFLY_AUTH_ADHOC);
  WiFly.setJoinMode(WIFLY_JOIN_MAKE_ADHOC);
  if(WiFly.join()){
    Serial << "ADHOC is live" << endl;
  }
  else{
    Serial << "Setup failed" << endl;
    while(true){
    }
  }
  listen();
}

//Listen for incomming connections -------------------------------
void listen () {

  if(WiFly.serveConnection()){
    Serial << "Incoming connection!" << endl;

    char* responseBuffer;
    const int buflength = 200;
    WiFly.ScanForPattern(responseBuffer,buflength, "HTTP/1.1", 1000);
    Serial << "HTTP/1.1 message, bytes: " << strlen(responseBuffer) << endl << responseBuffer << endl;


    //TEMP: const boolean bCollecting,  const unsigned long WaitTime, const boolean bPromptAfterResult);

    // read past the browser's header details (the exact behaviour of the module is to be tested!)
    char chMisc;
    while ((chMisc = WiFly.read()) > -1)
      Serial << chMisc;

    char* response = (char*) malloc(RESPONSE_BUFFER_SIZE);

    // Form header and body of response
    make_response(response,  responseBuffer, REQUEST_BUFFER_SIZE);

    // Send response
    WiFly << response << "\r\n\r\n" << "\t";

  }
  else{
    Serial << "Timed out!" << endl;
    listen();
  }
}

//TEMPORARY 
int make_response( char* response,  char* pRequest, const int sizeRequest ) {

  PString strResponse( response, RESPONSE_BUFFER_SIZE);
  const char *OK = PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
  const char *STYLE = PSTR("\r\n<html style='font-family:Verdana,Geneva,Georgia,Chicago,Arial,Sans-serif;color:#002d80'>");

  if ( strstr(pRequest, "GET / HTTP" ) ) {
    char* pNetScan;
    const int buflen = 200;
    char* scan = WiFly.showNetworkScan(pNetScan,buflen);
    strResponse << OK << STYLE << "Welcome to The Smart Power Strip Setup! <br/><br> Please enter the correspondent number of your Wi-Fi network shown in the list below. Also enter your password. <br/><br/>" << scan <<
      "<br/><br/>" << "<p>List number:<input style='margin-left:5px' value='' name='nr'/><br/>Password: <input type='password' name='p' style='margin-left:19px'/><br/><br/><br/>" << 
      "<form method='post'>You can also manually enter your network credentials below.<p>SSID: <input style='margin-left:53px' value='' name='s'/><br/>Password: <input type='password' name='p' style='margin-left:19px'/><br/>" <<
      "<br/><br/>Clicking <input type='submit' value='Update'/> will update the configuration and restart the board.</p></form></html>";

    Serial << strResponse << endl;

    // need to exit command mode to be able to send data
    WiFly.exitCommandMode();

  } 
  else if (strstr(pRequest, "POST / HTTP" )) {

    Serial << pRequest << endl;

    strResponse << OK << STYLE << "Updated (not). The board has been restarted with the new configuration.</html>";
  }
  return strResponse.length();
}

//TEMP
// MakeResponseHeader
// Form a HTML header, including length of body.
//int MakeResponseHeader( char* pHeader, char* pBody ) {
//
//  PString strHeader( pHeader, HEADER_BUFFER_SIZE);
//  // send a standard http response header    
//
//  strHeader << GetBuffer_P(IDX_WT_HTML_HEAD_01,bufTemp,TEMP_BUFFER_SIZE)
//    << GetBuffer_P(IDX_WT_HTML_HEAD_02,bufTemp,TEMP_BUFFER_SIZE)
//      << GetBuffer_P(IDX_WT_HTML_HEAD_03,bufTemp,TEMP_BUFFER_SIZE) << (int) strlen(pBody) << " \r"
//        << GetBuffer_P(IDX_WT_HTML_HEAD_04,bufTemp,TEMP_BUFFER_SIZE);
//
//  return strHeader.length();
//}

//Load WiFi configuration from EEPROM ---------------------------------
void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
    for (unsigned int t=0; t<sizeof(storage); t++)
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);
}

//Save WiFi configuration to EEPROM -----------------------------------
void saveConfig() {
  for (unsigned int t=0; t<sizeof(storage); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));
}
 // Use like this!
//void loop() {
  // [...]
//  int i = storage.c - 'a';
  // [...]

  // [...]
//  storage.c = 'a';
//  if (ok)
//    saveConfig();
  // [...]
//}
