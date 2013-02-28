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
const int analogLayoutLength = 6; // The length of the analogLayout vector.
const int digitalLayoutLength = 12; // The length of the digitalLayout vector.
const int sampleNumber = 500;     // Number of sampels to make for each measurement

int sensorValue = 0;        // value read from the pot  - From example
int outputValue = 0;        // value output to the PWM (analog out) - From example

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
int phaseDiff[analogLayoutLength];
int sampleRate; // Samples/sek

// Global Variables ------------------------------------------------


// Will be loaded from EEPROM
char passphrase[] = "jbregell";
char ssid[] = "Toshimoshi";
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
prog_char HTML_06[] PROGMEM = "<form name=\"input\" action=\"\" method=\"post\">List number:<input style='margin";
prog_char HTML_07[] PROGMEM =  "-left:5px' value='' name='nr'/><br/>Password: <input type='password' name='p' style='";
prog_char HTML_08[] PROGMEM = "margin-left:19px'/><br/><br><input type=\"submit\" value=\"Submit\"></form> <br/><br/> ";
prog_char HTML_09[] PROGMEM = "You can also manually enter your network credentials below.<p>";
prog_char HTML_10[] PROGMEM = "<form name=\"input2\" action=\"http:\\169.254.1.1\" method=\"post\">SSID:<input style";
prog_char HTML_11[] PROGMEM = "='margin-left:5px' value='' name='nr'/><br/>Password: <input type='password'";
prog_char HTML_12[] PROGMEM = "name='p' style='margin-left:19px'/><br/>Clicking <input type=\"submit\" value=\"Submit\"";
prog_char HTML_13[] PROGMEM = "> will update the configuration and restart the board.</form> </html>";



//Write commands for AD-HOC to progmem
prog_char CMD_01[] PROGMEM = "set wlan join 4";
prog_char CMD_02[] PROGMEM = "set wlan ssid Powerstrip";
prog_char CMD_03[] PROGMEM = "set wlan chan 1";
prog_char CMD_04[] PROGMEM = "set wlan chan 1";
prog_char CMD_05[] PROGMEM = "set ip address 169.254.1.1";
prog_char CMD_06[] PROGMEM = "set ip remote 80";
prog_char CMD_07[] PROGMEM = "set ip local 80";
prog_char CMD_08[] PROGMEM = "set ip netmask 255.255.0.0";
prog_char CMD_09[] PROGMEM = "set ip dhcp 0";
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
#define IDX_CMD_01      IDX_HTML_10 + 1
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


// Used to set up communication ------------------------------------
void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  Serial << "RAM > setup: " << freeMemory() << endl;

  //Serial << "Loading configuration from EEPROM..." << endl;
  // loadConfig();
  // Serial << "SSID: " << ssid << endl << "Passphrase: " << passphrase << endl;

  //Delay for booting WiFly module
  delay(1000);

  Serial.println(("Starting WiFly"));
  WiFly.begin();
  Serial.println("Started successfully!");


  //Set up adhoc
  //setup_adhoc();

  //Initialize WiFi connection to server
  wifi_init();

  //   time_t tCurrent= (time_t) WiFly.getTime(); 
  //   setTime( tCurrent );
  //   Serial << hour() << minute()<< endl;


}

// MAIN LOOP ---------------------------------------------------------------------------------------------------------
void loop() 
{
  if(counter==0) Serial << "RAM at loop: " << freeMemory() << endl;

  if(wifi_read()) //Check for input from server and read
  { 
    
    parse(); //Parse input
    check_state(); //Check if states have changed
  }

  //  else 
  //  {
  //    Serial << "Read failed"<<endl;
  //    wifi_reconnect(false);
  //  }

  counter++;
  if(counter>=100000) 
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
        //wifi_reconnect(false);
      }
    }

    else 
    { // If buffer is not empty then add latest data to buffer then try to send buffered values
      //addToBuffer();
      //sendBuffered();
    }
    counter=0;
  }
}

//METHODS -------------------------------------------------------------------------------------------------------------

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
  //phaseDifference();
  
  for (int i=1; i<=analogLayoutLength-1;i++){
    activePower[i-1]=RMS[0]*RMS[i]*cos(phaseDiff[i-1]);
  }
}

// Algorithm for parsing the control message string into arrays--
void parse() 
{
  int first_semicolon = String(bufUpdate).indexOf(';');
  for(int x = 0; x < 4; x++)
  {
    ControlSignal[x] = String(bufUpdate).charAt(first_semicolon - 1 + x*2) - '0';
  }

}

// Check if there are changes between Switch_State[] - Switch_State_ControlSignal[] and IF there are... SWITCH!!!!---
void check_state() 
{
  for(int x = 0; x < 4; x++)
  {
    if(Switch_State[x] != ControlSignal[x]) 
    {
      power_switching(x);
    }

  }

}

// Function for switching each socket -----------------------------
void power_switching(int socket) 
{
  if(Switch_State[socket] == HIGH) 
  {
    Switch_State[socket] = LOW;
    digitalWrite(socket, LOW);
  }

  else if(Switch_State[socket] == LOW) 
  {
    Switch_State[socket] = HIGH;
    digitalWrite(socket, HIGH);
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


// Initialize WiFi module and join network-------------------------
boolean wifi_init() 
{


  //  Serial << "Trying to scan..." << endl;
  //  char* pNetScan;
  //  const int buflen = 200;
  //  char* scan = WiFly.showNetworkScan(pNetScan,buflen);
  //  Serial << scan << endl;

  //WiFly.reboot();
  //delay(3000);

  //Remove WiFly TCP responses *HELLO* and *CLOS*
  char prompt[INDICATOR_BUFFER_SIZE];
  WiFly.SendCommandSimple("set comm remote 0",prompt);
  WiFly.SendCommandSimple("set comm close *CLOS*",prompt);
  WiFly.SendCommandSimple("set wlan join 1",prompt);
  WiFly.SendCommandSimple("save",prompt);
  WiFly.SendCommandSimple("reboot",prompt);
  delay(1000);

  //Set network properties
  WiFly.setAuthMode( WIFLY_AUTH_WPA2_PSK);
  WiFly.setJoinMode( WIFLY_JOIN_AUTO );
  WiFly.setDHCPMode( WIFLY_DHCP_ON );

  //Update status
  WiFly.getDeviceStatus();
  if(! WiFly.isifUp() ) 
  {
    //Leave just in case
    WiFly.leave();

    // Join WiFi network
    if(WiFly.setSSID(ssid)) 
    {
      Serial << "SSID:"<< ssid << endl;
    }

    if(WiFly.setPassphrase(passphrase)) 
    {
      Serial << "Pass:"<< passphrase << endl;
    }

    Serial << "Joining... :"<< ssid << endl;
    if( WiFly.join() ) 
    {
      Serial << "Joined " << ssid << endl;
      WiFly.setNTP( NTP_SERVER ); //NTP server
    }

    else 
    {
      Serial << F("Join to ") << ssid << F(" failed.") << endl << "Hang";
      while(true) 
      {
        //Hang
      }

    }
    WiFly.exitCommandMode();

  }


  // Clear out prior requests.
  WiFly.flush();
  while(WiFly.available()) WiFly.read();
  WiFly.closeConnection(); //close any open connections (for reboot of arduino)
  // Try to connect to server
  WiFly.setRemotePort(PORT);

  //if(!WiFly.isConnectionOpen()){  //TESTING WITHOUT THIS
  if(!WiFly.openConnection(SERVER)) 
  {
    Serial.println("Failed:S");
    if(!wifi_reconnect(true))
      return false;
  }

  else 
  {
    Serial.println("Success:S");
    WiFly.exitCommandMode();
    return true;
  } 
}

// Try to reconnect to server -------------------------------------
boolean wifi_reconnect(boolean isReInitialized) 
{
  if(!isReInitialized) 
  {
    Serial << "Reconnecting..."<< endl;
    WiFly.closeConnection();
    if(WiFly.openConnection(SERVER)) 
    {
      Serial << "Connected!"<< endl;
      return true;
    }

    else 
    {
      Serial << "Failed"<< endl << "Re-initialize"<< endl;
      WiFly.reboot();
      delay(1000);
      if(!wifi_init()) 
      {
        Serial << "Hanged!"<< endl;
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
    //Reset buffer
    memset(bufRequest,0,REQUEST_BUFFER_SIZE);
    //byte bufRequestt[80];
    //int i = 0;
    PString req (bufRequest,REQUEST_BUFFER_SIZE);
    while(WiFly.available()) //WiFly.isConnectionOpen() &&
    { //Check if client has received data
      req << WiFly.read();
      //control_message = String (control_message + client.read());
      //i++;
    }

    //WiFly.ScanForPattern(bufRequest,REQUEST_BUFFER_SIZE, "testID:",false, 1000);
    if((String(bufRequest).length()!=0)){

      memset(bufUpdate,0,UPDATE_BUFFER_SIZE);
      int temporary[3];
      int j=0;
      for(int i=0;i<String(bufRequest).length()-1;j++){
        temporary[0] = (bufRequest[i]-'0')*10;
        temporary[1] = (bufRequest[i+1]-'0');
        if(temporary[0]+temporary[1] <= 13){
          bufUpdate[j]=(temporary[0]+temporary[1])*10 + (bufRequest[i+2]-'0');
          i=i+3;
        }
        else{
          bufUpdate[j]=(temporary[0]+temporary[1]);
          i=i+2;
        }
      }
      Serial << "Svar: " << bufUpdate <<"slut" <<endl;
      return true;
    }
    return false;
  }
  else {
    return false;
  }
}

// Try to send data to server -------------------------------------
boolean wifi_send() 
{
  //String sendStream = String(ID + ":"+ "100"+ ";"+ "100"+ ";"+ "100"+ ";"+ "100"+ ":"+ Switch_State[0] + ";"+ Switch_State[1] + ";"+ Switch_State[2] + ";"+ Switch_State[3]); // ID:10;11;12;13:1;0;0;0
  if(WiFly.isConnectionOpen()) 
  {
    PString strSend(bufRequest, REQUEST_BUFFER_SIZE);
    Serial << "Writing" << endl;    
    strSend << ID << ":" << "100" << ";" << "100" << ";" << "100" << ";" << "100" << ":" << 
      Switch_State[0] << ";" << Switch_State[1] << ";" << Switch_State[2] << ";" << Switch_State[3]; // ID:10;11;12;13:1;0;0;0
    WiFly <<  (const char*) strSend << endl;
    Serial <<  (const char*) strSend << endl;
    return true;
  }

  else return false;
}

  //String sendStream = String(ID + ":" + activePower[0] + ";" + activePower[1] + ";" + activePower[2] + ";" + activePower[3] + ":" + Switch_State[0] + ";" + Switch_State[1] + ";" + Switch_State[2] + ";" + Switch_State[3]); // ID:10;11;12;13:1;0;0;0

// Set up ADHOC network -------------------------------------------
void setup_adhoc(){
  Serial << "Setting up ADHOC" << endl;
  char bufCMD[CMD_BUFFER_SIZE];
  //Load commands from PROGMEM and send to WiFly
  for(int i=IDX_CMD_01 ; i<=IDX_CMD_11 ; i++){
    WiFly.SendCommandSimple(GetBuffer_P(i,bufCMD,CMD_BUFFER_SIZE),prompt);
  }
  delay(3000);
  Serial <<"IP: " << WiFly.getIP(bufTemp,TEMP_BUFFER_SIZE) << endl;

  listen();
}


//Listen for incomming connections -------------------------------
void listen () {
  Serial << "Listening!" << endl;
  if(WiFly.serveConnection()){
    Serial << "Incoming!" << endl;

    WiFly.ScanForPattern(bufRequest,REQUEST_BUFFER_SIZE, "HTTP/1.1", 1000);
    //Serial << "HTTP/1.1 message, bytes: " << strlen(responseBuffer) << endl << responseBuffer << endl;

    // Record request
    char nextChar;
    //int i = 0;
    //memset(bufRequest,0,REQUEST_BUFFER_SIZE); //Reset buffer
    while ((nextChar = WiFly.read()) > -1){
      // bufRequest[i]=nextChar;
      //i++; 
    }
    Serial << bufRequest << endl;

    //Make responsebuffer
    char bufResponse[RESPONSE_BUFFER_SIZE];
    //Make response printer
    PString strResponse(bufResponse,RESPONSE_BUFFER_SIZE);

    Serial <<"Mem:R " << freeMemory()<< endl;

    //Look for HTTP GET
    if ( strstr(bufRequest, "GET / HTTP/1.1" ) ) {
      //char* pNetScan;
      //const int buflen = 200;
      //char* scan = WiFly.showNetworkScan(pNetScan,buflen);
      Serial << "JAA"<< endl;
      WiFly.exitCommandMode();
      //Send HTML page from PROGMEM
      for (int j=IDX_HTML_01 ; j<=IDX_HTML_13 ;j++){
        WiFly << GetBuffer_P(j,bufResponse,RESPONSE_BUFFER_SIZE);
      }
      WiFly <<"\r\n\r\n" << "\t";
    }
    WiFly.ScanForPattern(bufRequest,REQUEST_BUFFER_SIZE, "HTTP/1.1",false, 35000);

    while ((nextChar = WiFly.read()) > -1){
    }
    Serial << bufRequest << endl; 
    if (strstr(bufRequest, "POST" )) {

      Serial << "score" << endl;

      //strResponse << OK << STYLE << "Updated (not). The board has been restarted with the new configuration.</html>";
    }

  }
  else{
    Serial << "Timed out!" << endl;
    listen();
  }
  listen();
}

/*
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
int maximumValue(apvector<int> &array)
{    
     int length = array.length( );  // establish size of array
     int max = array[i];       // start with max = first element
     for(int i = (i); i<length; i++)
     {
          if(array[i] > max)
                max = array{i};
     }
     return max;                // return highest value in array
}

//Function to find highest (maximum) value in array ---------------


// Phase Differnece -----------------------------------------------
void phaseDifference(){
  int highestVoltageSampleNumber; //Tells on which sample the highest value of the voltage is located
  int local_vector[sampleNumber]; //Local vector used for calculations
  for(int y=0; y<analogLayoutLength; y++){
    for(int x=0; x<sampleNumber; x++){
      local_vector[x] = analogReadState[y][x]; // Store in the local variable
    }
    if(y=0){
      highestVoltageSampleNumber = maximumValue(local_vector); // Load the highest voltage sample
    }
    if(y!=0){
       int sampleDiff =  maximumValue(local_vector) - highestVoltageSampleNumber; //(May be positive or negative)
       phaseDiff[y] = (((sampleDiff)*sampleRate)/0.02)*360;  //Algorithm for converting samples to degrees
    }
  }
}
// Phase Difference -----------------------------------------------
*/

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

// Sätt ett bestämt värde där man vet en spännigstopp!! utgå ifrån det när du hittar toppen för strömmen....


