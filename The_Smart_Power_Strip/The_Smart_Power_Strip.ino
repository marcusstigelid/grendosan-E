/*
     The smart power strip,
 
 Written by Philip Karlsson and David Johansson
 Electronic Engineering,
 Chalmers University of Technology
 
 2013-05-08
 
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
  

#define ID "SN-0000003"
long counter = 0;
long loopmillis=millis();
int activePower[analogLayoutLength-1];

const int sampleRate = 1500; // Samples/sek


// Global Variables ------------------------------------------------


#define chan "1" // Channel for adhoc

boolean bufferEmpty = true;
boolean joined_once=false;
boolean REBOOT = true;

//The following is for storing in EEPROM
// ID of the settings block
#define CONFIG_VERSION "ls7"

// Tell it where to store your config data in EEPROM
#define CONFIG_START 32

// The variables of the settings
char ssid[32];
PString ssidP(ssid,32);
byte readIndex[3]={
  0x00,0x00,0x00};
byte storeIndex[3]={
  0x00,0x00,0x00};

struct StoreStruct {
  // This is for detection if they are our settings
  char version[4];
  // The variables of the settings
  char ssid[32];
  byte readIndex[3];
  byte storeIndex[3];
  boolean Switch_State[4];
}

storage = {
  CONFIG_VERSION,
  // The default values
  "Testssid",
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

//Write commands to progmem
prog_char CMD_01[] PROGMEM = "set opt device_id Powerstrip";//"set wlan auth 0";
prog_char CMD_02[] PROGMEM = "run web_app";
prog_char CMD_03[] PROGMEM = "set sys printlvl 1";//0x10";
prog_char CMD_04[] PROGMEM = "set wlan linkmon 10";
prog_char CMD_05[] PROGMEM = "set wlan join 0";
prog_char CMD_06[] PROGMEM = "set ip flags 0x6";
prog_char CMD_07[] PROGMEM = "save";
prog_char CMD_08[] PROGMEM = "reboot";


//Command indices
#define IDX_CMD_01      0
#define IDX_CMD_02      IDX_CMD_01 + 1
#define IDX_CMD_03      IDX_CMD_01 + 2
#define IDX_CMD_04      IDX_CMD_01 + 3
#define IDX_CMD_05      IDX_CMD_01 + 4
#define IDX_CMD_06      IDX_CMD_01 + 5
#define IDX_CMD_07      IDX_CMD_01 + 6
#define IDX_CMD_08      IDX_CMD_01 + 7

PROGMEM const char *WT_string_table[] =      
{  
  CMD_01,
  CMD_02,
  CMD_03,
  CMD_04,
  CMD_05,
  CMD_06,
  CMD_07,
  CMD_08
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

  Serial << freeMemory() << endl;

  SPI_setup();

  //Serial << "Loading configuration from EEPROM..." << endl;
  loadEEPROM();

  if(readIndex[2]!=storeIndex[2] || readIndex[1]!=storeIndex[1] || readIndex[0]!=storeIndex[0]){ // If readIndex and storeIndex is not equal
    bufferEmpty=false;
  }
  else
    bufferEmpty=true;

  //Delay for booting WiFly module
  delay(1000);
  WiFly.begin();
  Serial.println(F("WiFly"));

//  ssidP="Toshimoshi";//"Philips$iPhone$5";//"Toshimoshi";//;
//  passphraseP="jbregell";//"grisnils";//"jbregell"
//  saveEEPROM();
//  loadEEPROM();

  for(int j=0; j<4; j++){
    activePower[j]=storageStruct.activePower[j];    
    pinMode(digitalLayout[j], OUTPUT);
    digitalWrite(digitalLayout[j], Switch_State[j]);
    pinMode(analogLayout[j],INPUT);
  }
  pinMode(analogLayout[4],INPUT);
  analogReference(EXTERNAL);
  
  //Initialize WiFi connection to server
  initialize();
}



