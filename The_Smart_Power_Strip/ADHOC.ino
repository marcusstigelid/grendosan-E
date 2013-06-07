//Methods for AD-HOC HTML server

// Set up ADHOC network -------------------------------------------
void setup_adhoc(){
  char bufCMD[CMD_BUFFER_SIZE];
  //Load commands from PROGMEM and send to WiFly
//  for(int i=IDX_CMD_01 ; i<=IDX_CMD_02 ; i++){
//    WiFly.SendCommandSimple(GetBuffer_P(i,bufCMD,CMD_BUFFER_SIZE),prompt);
//  }
  WiFly.SendCommandSimple(GetBuffer_P(IDX_CMD_01,bufCMD,CMD_BUFFER_SIZE),prompt);
  WiFly.SendCommandSimple(GetBuffer_P(IDX_CMD_07,bufCMD,CMD_BUFFER_SIZE),prompt);
  WiFly.SendCommandSimple(GetBuffer_P(IDX_CMD_02,bufCMD,CMD_BUFFER_SIZE),prompt);
  delay(1000);
  Serial << "Config" << endl;

  listen();
}

void listen (){
  boolean is_done=false;
  while(!is_done){
    if(1==WiFly.ScanForPattern(bufRequest,REQUEST_BUFFER_SIZE, "Disabling",false, 10000))
      is_done=true;
  }
  delay(1000);
  //ssidP=WiFly.getSSID(bufRequest,REQUEST_BUFFER_SIZE);
  //saveEEPROM();
}





