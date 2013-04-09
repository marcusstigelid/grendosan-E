//Methods for controlling WiFi module  (exeption AD-HOC mode)


// Initialize WiFi module and join network-------------------------
boolean wifi_init() 
{

  //Remove WiFly TCP responses *HELLO* and *CLOS*
  char prompt[INDICATOR_BUFFER_SIZE];
  WiFly.SendCommandSimple("set comm remote 0",prompt);
  WiFly.SendCommandSimple("set comm close *CLOS*",prompt);
  WiFly.SendCommandSimple("set wlan join 1",prompt);
  WiFly.SendCommandSimple("save",prompt);
  WiFly.SendCommandSimple("reboot",prompt);
  delay(2000);

  //Set network properties
  WiFly.setAuthMode( WIFLY_AUTH_WPA1);//WPA2_PSK);//WPA1);//
  WiFly.setJoinMode( WIFLY_JOIN_AUTO );
  WiFly.setDHCPMode( WIFLY_DHCP_ON );

  //Update status
  WiFly.getDeviceStatus();
  if(!WiFly.isifUp()) 
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
    if( WiFly.join(ssid) ) 
    {
      Serial << "Joined " << ssid << endl;
      if(!joined_once){
        WiFly.setNTP_UTC_Offset(0);
        delay(1000);
        WiFly.setNTP(NTP_SERVER); //NTP server
        delay(2000);
        WiFly.SendCommandSimple("get time",prompt);
        delay(2000);
        setTime(WiFly.getTime());
        Serial <<"TID: " << WiFly.getTime()/60/60/24/365 << endl;;
        Year = year();
        Month = month();
        Day = day();
        Hour = hour();
        Minute = minute();
        Second = second ();
        joined_once = true;
      }  
      }

    else 
    {
      Serial << F("Join to ") << ssid << F(" failed.") << endl << "Hang";
      return false;
    }
    WiFly.exitCommandMode();

  }

  // Clear out prior requests.
  WiFly.flush();
  while(WiFly.available()) WiFly.read();

  return true;
}

// Try to reconnect to server -------------------------------------
boolean wifi_reconnect()//boolean isReInitialized) 
{
  WiFly.closeConnection(); //close any open connections (for reboot of arduino)
  // Try to connect to server
  WiFly.setRemotePort(PORT);

  //if(!WiFly.isConnectionOpen()){  //TESTING WITHOUT THIS
  if(!WiFly.openConnection(SERVER)) 
  {
    Serial.println("Failed:S");
      return false;
  }

  else 
  {
    Serial.println("Success:S");
    WiFly.exitCommandMode();
    return true;
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
      Serial << "Svar: " << bufTemp <<"slut" <<endl;
      return true;
    }
    return true;
  }
  else {
    return false;
  }
}

// Try to send data to server -------------------------------------
boolean wifi_send() 
{
  //String sendStream = String(ID + ":"+ "100"+ ";"+ "100"+ ";"+ "100"+ ";"+ "100"+ ":"+ Switch_State[0] + ";"+ Switch_State[1] + ";"+ Switch_State[2] + ";"+ Switch_State[3]); // ID:10;11;12;13:1;0;0;0
  WiFly.getDeviceStatus();
  if(WiFly.isConnectionOpen()) 
  {
    WiFly.exitCommandMode();
    PString strSend(bufRequest, REQUEST_BUFFER_SIZE);
    Serial << "Writing" << endl;    
    strSend << ID << ":" << activePower[0] << ";" << activePower[1] << ";" << activePower[2] << ";" << activePower[3] << ":" << 
      Switch_State[0] << ";" << Switch_State[1] << ";" << Switch_State[2] << ";" << Switch_State[3] << ":" << Year << ";" << Month << ";" << Day << ";" << Hour << ";" << Minute << ";" << Second; // ID:10;11;12;13:1;0;0;0
    WiFly <<  (const char*) strSend << endl;
    Serial <<  (const char*) strSend << endl;
    return true;
  }

  else return false;
}

  //String sendStream = String(ID + ":" + activePower[0] + ";" + activePower[1] + ";" + activePower[2] + ";" + activePower[3] + ":" + Switch_State[0] + ";" + Switch_State[1] + ";" + Switch_State[2] + ";" + Switch_State[3]); // ID:10;11;12;13:1;0;0;0

