//Methods for AD-HOC HTML server



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

