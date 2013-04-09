//Methods for AD-HOC HTML server



// Set up ADHOC network -------------------------------------------
void setup_adhoc(){
  Serial << "Setting up ADHOC" << endl;
  char bufCMD[CMD_BUFFER_SIZE];
  //Load commands from PROGMEM and send to WiFly
  for(int i=IDX_CMD_01 ; i<=IDX_CMD_11 ; i++){
    WiFly.SendCommandSimple(GetBuffer_P(i,bufCMD,CMD_BUFFER_SIZE),prompt);
  }
  delay(1000);
  Serial << freeMemory() << endl;
  Serial << "IP: " << WiFly.getIP(bufTemp,TEMP_BUFFER_SIZE) << endl;

  listen();
}


//Listen for incomming connections -------------------------------
void listen () {
  while(true){
    memset(bufRequest,'0',REQUEST_BUFFER_SIZE);
    memset(bufTemp,'0',TEMP_BUFFER_SIZE);
    //WiFly.getDeviceStatus();
    Serial << "Listening!" << endl;
    if(WiFly.serveConnection()){
      while(WiFly.isConnectionOpen()){
      Serial << "Incoming!" << endl;

      WiFly.ScanForPattern(bufRequest,REQUEST_BUFFER_SIZE, "HTTP/1.1",true, 10000);
      //Serial << "HTTP/1.1 message, bytes: " << strlen(responseBuffer) << endl << responseBuffer << endl;

      // Record request
      char nextChar;

        while ((nextChar = WiFly.read()) > -1){
      }

      Serial << bufRequest << endl;

      //Declare temporary for parsing
      int idx_nr;
      boolean done = false;
      int k=0;
      char pass[32];

      Serial <<"Mem:R " << freeMemory()<< endl;

      //Look for HTTP GET
      if ( strstr(bufRequest, "GET / HTTP/1.1" ) ) {
        
        //WiFly.exitCommandMode();
        //Send HTML page from PROGMEM
        for (int j=IDX_HTML_01 ; j<=IDX_HTML_05 ;j++){
          WiFly << GetBuffer_P(j,bufTemp,TEMP_BUFFER_SIZE);
        }
        for (int l=IDX_HTML_05 ; l<=IDX_HTML_13 ;l++){
          WiFly << GetBuffer_P(l,bufTemp,TEMP_BUFFER_SIZE);
        }
        WiFly <<"\r\n\r\n" << "\t";
        delay(40000);
        //WiFly.SendCommandSimple("close",prompt);
        //WiFly.closeConnection();
      }

      else if (strstr(bufRequest, "GET /nr" )) {

        idx_nr = String(bufRequest).indexOf("nr=") + 2;
        char number[5];
        while(!done)
        {
          number[k] = String(bufRequest).charAt(idx_nr + 1 + k );
          if(number[k]=='&'){
            number[k]=0;
            done=true;
            Serial << "Number: " << number << endl;
          }
          k++;
        }
        idx_nr = String(bufRequest).indexOf("p=") + 1; //här ska det va något annat
        done = false;
        k=0;
        while(!done)
        {
          pass[k] = String(bufRequest).charAt(idx_nr + 1 + k );
          if(pass[k]==' '){
            pass[k]=0;
            done=true;
            Serial << "Passphrase: " << pass << endl;
          }
          k++;
        }

        //strResponse << OK << STYLE << "Updated (not). The board has been restarted with the new configuration.</html>";
      }

      else if (strstr(bufRequest, "GET /manual" )) {
        idx_nr = String(bufRequest).indexOf("s=") + 1;
        char name[32];
        while(!done)
        {
          name[k] = String(bufRequest).charAt(idx_nr + 1 + k );
          if(name[k]=='&'){
            name[k]=0;
            done=true;
            Serial << "Name: " << name << endl;
            if(name != 0){
              for(int i=0;i<32;i++){
                if(name[i]!=' ')
                  ssid[i]=name[i];
                else
                  ssid[i]='$';
              }
              Serial << "SSID: " << ssid << endl;
            }
          }
          k++;
        }
        idx_nr = String(bufRequest).indexOf("p=") + 1; //här ska det va något annat
        done = false;
        k=0;
        while(!done)
        {
          pass[k] = String(bufRequest).charAt(idx_nr + 1 + k );
          if(pass[k]==' '){
            pass[k]=0;
            done=true;
            Serial << "pass: " << pass << endl;
            if(pass != 0){
              for(int i=0;i<32;i++){
                passphrase[i]=pass[i];
              }
              Serial << "passphrase: " << passphrase << endl;
              //return; 
            }
          }
          k++;
        }
        //strResponse << OK << STYLE << "Updated (not). The board has been restarted with the new configuration.</html>";
      }

    }
    char dump;
    while(WiFly.available()>0)
    dump=WiFly.read();
    }
    else{
      Serial << "Timed out!" << endl;
    }
  }
}




