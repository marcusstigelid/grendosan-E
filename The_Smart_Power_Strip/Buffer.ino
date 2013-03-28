//Methods for storing and loading data to FLASH memory



// Shifts a byte out on MOSI and receives a byte from MISO ---------
char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}

//Initial SPI setup ------------------------------------------------
void SPI_setup() 
{
  //Set pin modes
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT);
  digitalWrite(SLAVESELECT,HIGH); //disable device
  
  //Set the SPI settings register:
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (fastest)
  SPCR = (1<<SPE)|(1<<MSTR);
  byte clr;
  clr=SPSR;
  clr=SPDR;
  delay(10);

  //Load standard values
  for(int j=0; j<4; j++){
    activePower[j]=storageStruct.activePower[j];
    Switch_State[j]=storageStruct.Switch_State[j];
  }
}


//Handles addressing when storing data on FLASH --------------------------------
void addToBuffer() 
{
  loadEEPROM(); //Load addresses from EEPROM
  storeString();  //Store values
  bufferEmpty=false;

  //Determine next store address
  int step;
  if((storeIndex[2]+datalength*2)>=256){
    step = 256 - storeIndex[2];
  }
  else step = datalength;

  for(int i=0; i<step;i++){
    if(storeIndex[2]+1!=256){
      storeIndex[2]+=1;
    }
    else if(storeIndex[1]+1!=256){
      storeIndex[2]=0;
      storeIndex[1]+=1;
    }
    else if(storeIndex[0]+1!=256){
      storeIndex[2]=0;
      storeIndex[1]=0;
      storeIndex[0]+=1;
    }
    else{
      storeIndex[2]=0;
      storeIndex[1]=0x01;
      storeIndex[0]=0;    
    }
  }
  saveEEPROM();  //Update addresses to EEPROM
}

// Stores what is in activePower and Switch_State to storeIndex -----------------
void storeString(){
  
  //Update values in data structure
  for(int j=0; j<4; j++){
  storageStruct.activePower[j]=activePower[j];
  storageStruct.Switch_State[j]=Switch_State[j];
  }

  //char time[7]; //Time will have to be added!!
  
  
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN); // Write enable
  digitalWrite(SLAVESELECT,HIGH);
  delay(10);
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WRITE); //Write instruction
  //Transfer start address
  spi_transfer(storeIndex[0]);
  spi_transfer(storeIndex[1]);
  spi_transfer(storeIndex[2]);
  //Write storageStruct to FLASH
  for(int i=0;i<datalength;i++){
    spi_transfer(byte(*((char*)&storageStruct + i)));
    delay(10);
  }
  digitalWrite(SLAVESELECT,HIGH); //release chip
  delay(10);
}



//Sends buffer if it is not empty and handles FLASH addressing ----------------------
boolean sendBuffer() 
{ 
  loadEEPROM(); //Update addresses from EEPROM
  while(!bufferEmpty){ // Send next value until buffer is empty
    if(readIndex[2]!=storeIndex[2] || readIndex[1]!=storeIndex[1] || readIndex[0]!=storeIndex[0]){ // If readIndex and storeIndex is not equal
      readString(); //Read next buffered values
      if(true){//wifi_send()) //Try to send loaded values
        //Update readIndex
        int step;
        if((readIndex[2]+datalength*2)>=256){
          step = 256 - readIndex[2];
        }
        else step = datalength;
        for(int i=0; i<step;i++){
          if(readIndex[2]+1!=256){
            readIndex[2]+=1;
          }
          else if(readIndex[1]+1!=256){
            readIndex[2]=0;
            readIndex[1]+=1;
          }
          else if(readIndex[0]+1!=256){
            readIndex[2]=0;
            readIndex[1]=0;
            readIndex[0]+=1;
          }
          else{
            readIndex[2]=0;
            readIndex[1]=0x01;
            readIndex[0]=0;    
          } 
        }
        saveEEPROM(); //Save new readIndex to EEPROM     
      }
      else return false;
    }
    //If readIndex = storeIndex then buffer is empty => reset addresses
    else{ 
      bufferEmpty=true;
      //Erase the sectors used in previous buffering
      while(readIndex[0]>=1){
        erase();
        readIndex[0]-=1;
      }
      erase();
      readIndex[0]=0x00;
      readIndex[1]=0x00;
      readIndex[2]=0x00;
      storeIndex[0]=0x00;
      storeIndex[1]=0x00;
      storeIndex[2]=0x00;
      saveEEPROM(); //Update addresses to EEPROM
    }
  }
  return true;
}

// Updates what is in activePower and Switch_State from readIndex -----------------
void readString(){
  delay(10);
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(0x0B); //Read instruction
  //Transfer read address 
  spi_transfer(readIndex[0]);
  spi_transfer(readIndex[1]);
  spi_transfer(readIndex[2]);
  spi_transfer(0x00);
  //Update storageStruct from FLASH
  for(int i=0;i<datalength;i++){
    *((char*)&storageStruct + i) = spi_transfer(0xFF);
  }
  digitalWrite(SLAVESELECT,HIGH); //release chip
  //Update activePower and Switch_State from storageStruct
  for(int j=0; j<4; j++){
  activePower[j]=storageStruct.activePower[j];
  Switch_State[j]=storageStruct.Switch_State[j];
  }
  //char time[7]; //Will be done later   
}


//Erases the sector in which readIndex is included
void erase (){
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN); //Write enable
  digitalWrite(SLAVESELECT,HIGH);
  delay(10);

  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(0x20); //Erase opcode
  spi_transfer(readIndex[0]);
  spi_transfer(readIndex[1]);
  spi_transfer(readIndex[2]);
  digitalWrite(SLAVESELECT,HIGH); 
  delay  (2000); //Delay for the erasure to finish
}