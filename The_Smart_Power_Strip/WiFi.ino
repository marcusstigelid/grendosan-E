//Methods for controlling WiFi module  (exeption AD-HOC mode)


// Initialize WiFi module and join network-------------------------
boolean wifi_init() 
{
  if(!joined_once){
    char bufCMD[CMD_BUFFER_SIZE];
    //Remove WiFly TCP responses *HELLO* and *CLOS*  
    for(int i=IDX_CMD_03 ; i<=IDX_CMD_08 ; i++){
      WiFly.SendCommandSimple(GetBuffer_P(i,bufCMD,CMD_BUFFER_SIZE),prompt);
    }
    delay(2000);

    WiFly.setDHCPMode( WIFLY_DHCP_ON );
  }

  //Update status
  WiFly.getDeviceStatus();
  if(WiFly.isifUp()==0) 
  {
    //Leave just in case
    //WiFly.leave();

    Serial << F("Joining") << endl;
    if( WiFly.join() ) 
    {
      Serial << F("Joined")<< endl; 
      return true;
    }

    else 
    {
      Serial << F("Fail") << endl;
      return false;
    }
    //WiFly.exitCommandMode();

  }
  else
    return true;
}

// Try to reconnect to server -------------------------------------
boolean wifi_reconnect()
{
  WiFly.closeConnection(); //close any open connections (for reboot of arduino)
  // Try to connect to server
  WiFly.setRemotePort(PORT);

  //if(!WiFly.isConnectionOpen()){  //TESTING WITHOUT THIS
  if(!WiFly.openConnection(SERVER)) 
  {
    Serial.println("Fail");
    return false;
  }

  else 
  {
    Serial.println("Connect");
    WiFly.exitCommandMode();
    return true;
  } 
}

// Try to read input from server ----------------------------------
boolean wifi_read() 
{
  if(true)//WiFly.isTCPConnected()) 
  { //Check if client is connected
    //Reset buffer
    memset(bufRequest,0,REQUEST_BUFFER_SIZE);
    //byte bufRequestt[80];
    //int i = 0;
    PString req (bufRequest,REQUEST_BUFFER_SIZE);
    while(WiFly.available()>0)
    { //Check if client has received data
      req << WiFly.read();
    }

    if((String(bufRequest).length()!=0)){

      memset(bufTemp,0,TEMP_BUFFER_SIZE);
      int temporary[3];
      int j=0;
      for(int i=0;i<String(bufRequest).length()-1;j++){
        temporary[0] = (bufRequest[i]-'0')*10;
        temporary[1] = (bufRequest[i+1]-'0');
        if(temporary[0]+temporary[1] <= 13){
          bufTemp[j]=(temporary[0]+temporary[1])*10 + (bufRequest[i+2]-'0');
          i=i+3;
        }
        else{
          bufTemp[j]=(temporary[0]+temporary[1]);
          i=i+2;
        }
      }
      Serial << "S: " << bufTemp <<endl;
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
  if(WiFly.isConnectionOpen())
  {
    memset(bufRequest,'0',REQUEST_BUFFER_SIZE);
    PString strSend(bufRequest, REQUEST_BUFFER_SIZE);
    //Serial << F("Sending") << Year << Second << endl;    
    //WiFly << F("PowerStrip#id:") << ID << F(",power:") << activePower[0] << ";" << activePower[1] << ";" << activePower[2] << ";" << activePower[3] << F(",status:") << 
      //Switch_State[0] << ";" << Switch_State[1] << ";" << Switch_State[2] << ";" << Switch_State[3] << F(",date:") << Year << ";" << Month << ";" << Day << F(",time:") << Hour << ";" << Minute << ";" << Second;
    strSend << ID << ":" << activePower[0] << ";" << activePower[1] << ";" << activePower[2] << ";" << activePower[3] << ":" << 
      Switch_State[0] << ";" << Switch_State[1] << ";" << Switch_State[2] << ";" << Switch_State[3] << ":" << Year << ";" << Month << ";" << Day << ":" << Hour << ";" << Minute << ";" << Second << endl;
    WiFly <<  (const char*) strSend;
    Serial <<  (const char*) strSend << endl;
    return true;
  }
  else return false;
}

