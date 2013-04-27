/*
     The smart power strip,
 
 Written by Philip Karlsson and David Johansson
 
 2013-02-13
 
 */

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
  5,6,7,8};

//Array for the digital pins. (These will be changed when decided what to use as Rx, Tx etc..)
// Arrays used for reading the pins -------------------------------
// Global Variables -----------------------------------------------
#define analogLayoutLength 6 // The length of the analogLayout vector.
#define digitalLayoutLength 12 // The length of the digitalLayout vector.
#define sampleNumber 60    // Number of sampels to make for each measurement

#define DATAOUT 11//MOSI
#define DATAIN  12//MISO 
#define SPICLOCK  13//sck
#define SLAVESELECT 10//ss

#define WREN  0x06
#define WRDI  0x04
#define RDSR  0x05
#define WRSR  0x01
#define READ  0x0B
#define WRITE 0x02

boolean Switch_State[4]; // Array for the switching state of each socket

int Switch_to_pin_array[] = 
{
  5, 6, 7, 8 }; // Array for mapping states of the switches to the physical pins
  
boolean ControlSignal[4] = {
  1,1,1,1}; // Array for the revieved controlsignals
  

#define ID "SN-0000001"
long counter = 0;
//long loopmillis;
int activePower[analogLayoutLength-1];

#define sampleRate 30*50 // Samples/sek


// Global Variables ------------------------------------------------


#define chan "1" // Channel for adhoc

boolean bufferEmpty = true;
boolean joined_once=false;
boolean REBOOT = true;

//The following is for storing in EEPROM
// ID of the settings block
#define CONFIG_VERSION "ls5"

// Tell it where to store your config data in EEPROM
#define CONFIG_START 32

// The variables of the settings
char ssid[32];
char passphrase[32];
PString ssidP(ssid,32);
PString passphraseP(passphrase,32);
int securityType;
byte readIndex[3]={
  0x00,0x00,0x00};
byte storeIndex[3]={
  0x00,0x00,0x00};

struct StoreStruct {
  // This is for detection if they are our settings
  char version[4];
  // The variables of the settings
  char ssid[32];
  char passphrase[32];
  int securityType;
  byte readIndex[3];
  byte storeIndex[3];
  boolean Switch_State[4];
}

storage = {
  CONFIG_VERSION,
  // The default values
  "Testssid",
  "Testpass",
  WIFLY_AUTH_WPA2_PSK,
  {
    0x00,0x00,0x00    }
  ,
  {
    0x00,0x00,0x00    },
  {true,true,true,true}
};

//Storage structure for FLASH memory
#define datalength 26
struct FLASHStruct {
  int activePower[analogLayoutLength-1];
  boolean Switch_State[4];
  int Year;
  int Month;
  int Day;
  int Hour;
  int Minute;
  int Second;
}

storageStruct = {
  {
    100,100,100,100  }
  ,
  {
    HIGH, HIGH, HIGH, HIGH  }
  ,
  1970,
  1,
  1,
  0,
  0,
  0
};



//Set UART pins
WiFlySerial WiFly(2,3);

time_t time;

int Year;
int Month;
int Day;
int Hour;
int Minute;
int Second;

//Defines server
#define SERVER "bregell.mine.nu" //"bregell.dyndns.org" //"46.239.111.148"
#define PORT 39500
#define NTP_SERVER "62.119.40.99"

//Buffer sizes
#define REQUEST_BUFFER_SIZE 80
#define TEMP_BUFFER_SIZE 100
#define INDICATOR_BUFFER_SIZE 16
#define CMD_BUFFER_SIZE 50
//#define UPDATE_BUFFER_SIZE 27

//Define buffers
char bufRequest[REQUEST_BUFFER_SIZE];
char prompt[INDICATOR_BUFFER_SIZE]; 
char bufTemp[TEMP_BUFFER_SIZE];


//Write html page to PROGMEM
prog_char HTML_01[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
prog_char HTML_02[] PROGMEM = "\r\n<html style='font-family:Verdana,Geneva,Georgia,Chicago,Arial,Sans-serif;color:#002d80'>";
prog_char HTML_03[] PROGMEM = "Welcome to The Smart Power Strip Setup!";
prog_char HTML_04[] PROGMEM = "<br/><br> Please enter the correspondent number of your Wi-Fi";
prog_char HTML_05[] PROGMEM = " network shown in the list below. Also enter your password. <br/><br/>";
prog_char HTML_06[] PROGMEM = "<form name=\"input\" action=\"nr\" method=\"get\">List number:<input style='margin";
prog_char HTML_07[] PROGMEM =  "-left:5px' value='' name='nr'/><br/>Password: <input type='password' name='p' style='";
prog_char HTML_08[] PROGMEM = "margin-left:19px'/><br/><br><input type=\"submit\" value=\"Submit\"></form> <br/><br/> ";
prog_char HTML_09[] PROGMEM = "You can also manually enter your network credentials below.<p>";
prog_char HTML_10[] PROGMEM = "<form name=\"input2\" action=\"manual\" method=\"get\">SSID:<input style";
prog_char HTML_11[] PROGMEM = "='margin-left:5px' value='' name='s'/><br/>Password: <input type='password'";
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

// Used to set up communication ------------------------------------
void setup() 
{

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  Serial << "RAM > setup: " << freeMemory() << endl;

  SPI_setup();

  //Serial << "Loading configuration from EEPROM..." << endl;
  loadEEPROM();
  //Serial << "SSID: " << ssid << endl << "Passphrase: " << passphrase << endl;

  if(readIndex[2]!=storeIndex[2] || readIndex[1]!=storeIndex[1] || readIndex[0]!=storeIndex[0]){ // If readIndex and storeIndex is not equal
    bufferEmpty=false;
  }
  else
    bufferEmpty=true;

  //Delay for booting WiFly module
  delay(1000);

  Serial.println(F("Starting WiFly"));
  WiFly.begin();
  Serial.println(F("WiFly:S"));



  //  char scantemp[200];
  //  //WiFly.StartCommandMode();
  //  Serial << "Scanning...." << endl;
  //  char prompt[INDICATOR_BUFFER_SIZE];
  //  WiFly.SendCommandSimple("$$$",prompt);
  //  delay(1000);
  //  //WiFly.SendCommandSimple("scan",prompt);
  //  WiFly.SendCommand("scan","'", scantemp, 200, true, 5000, true, false) ;
  //  //char chOut;
  //  delay(3500);
  //  for(int i=0 ;i<=200;i++){
  //    Serial << scantemp[i];
  //  }
  //  Serial << "READ" << endl;
  //  //while(true){
  //  while(WiFly.available() > 0) {
  //    Serial.write(WiFly.read());
  //  }
  //}
  //  if(Serial.available()) { // Outgoing data
  //    WiFly.write( (chOut = Serial.read()) );
  //    Serial.write (chOut);
  //  }



  //Set up adhoc
  //  Serial << freeMemory() << endl;
  //  setup_adhoc();
  //  Serial << endl << ssid << endl << passphrase << endl;
  //  while(true){}

  ssidP="Davids";//"Philips$iPhone$5";//"Toshimoshi";//;
  //  passphraseP="grisnils";
  //ssidP="DJLumia";
  passphraseP="grisnils";//"grisnils";//"jbregell"
  saveEEPROM();
  loadEEPROM();
  //Initialize WiFi connection to server

  for(int j=0; j<4; j++){
    activePower[j]=storageStruct.activePower[j];    
    pinMode(digitalLayout[j], OUTPUT);
    digitalWrite(digitalLayout[j], Switch_State[j]);
    pinMode(analogLayout[j],INPUT);
  }
  pinMode(analogLayout[4],INPUT);
  analogReference(EXTERNAL);

  initialize();

  //  while(true){
  //        counter++;
  //    Year = year();
  //    Month = month();
  //    Day = day();
  //    Hour = hour();
  //    Minute = minute();
  //    Second = second ();
  //    Serial << "Second: "<<Second << endl;
  //    addToBuffer();
  //    activePower[2]+=10;
  //    Switch_State[1]=!Switch_State[1];
  //    if(counter>=15){
  //      sendBuffer();
  //      counter =0;
  //      Serial << "IGEN" << endl;
  //      sendBuffer();
  //      while(true){}
  //    }
  //    delay(2000);
  //  }

  //   time_t tCurrent= (time_t) WiFly.getTime(); 
  //   setTime( tCurrent );
  //   Serial << hour() << minute()<< endl;
  //loopmillis=millis();
  //Serial << loopmillis << endl;
}



