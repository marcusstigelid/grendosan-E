// MAIN LOOP ---------------------------------------------------------------------------------------------------------

void loop() 
{

  if(counter==0) Serial << "RAM at loop: " << freeMemory() << endl; // Returns free RAM
  
  //delay(10);
  if(wifi_read()) //Check for input from server and read
  { 
   // Serial << "Start" << bufTemp << "STOP" << endl;
    if(String(bufTemp).charAt(0)=='S'){
    Serial << "hej" << endl;
    parse(); //Parse input
    check_state(); //Check if states have changed
    }
  }

  counter++;

  if(counter>=100000)//millis()>=loopmillis+5000) 
  {
    sample(); // Sample inputs
    //loopmillis=millis();
    Year = year();
    Month = month();
    Day = day();
    Hour = hour();
    Minute = minute();
    Second = second ();
     
    //Send calculated values and state
    Serial << bufferEmpty << endl;
    if(bufferEmpty) 
    { //Send data if buffer is empty 
      if(!wifi_send()) 
      { //Try to send data and add to buffer when failure
        addToBuffer(); //Add measurements to buffer if they can't be sent
        if(!sendBuffer()){
          Serial << "HEJ" << endl;
          if(!wifi_reconnect()){
            WiFly.getDeviceStatus();
            if(!WiFly.isAssociated())
              initialize();

          }
        }
      }
    }

    else 
    { // If buffer is not empty then add latest data to buffer then try to send buffered values
      addToBuffer();
      if(!sendBuffer()){
          Serial << "HEJ2" << endl;
        if(!wifi_reconnect()){
          WiFly.getDeviceStatus();
          if(!WiFly.isAssociated())
            initialize();

        }

      }
    }
    //WiFly.exitCommandMode();
    counter=0;
  }
}

