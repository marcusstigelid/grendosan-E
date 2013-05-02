// MAIN LOOP ---------------------------------------------------------------------------------------------------------

void loop() 
{

  //if(counter==0) Serial << "RAM at loop: " << freeMemory() << endl; // Returns free RAM

  //delay(10);
  if(wifi_read()) //Check for input from server and read
  { 
    // Serial << "Start" << bufTemp << "STOP" << endl;
    if(String(bufTemp).charAt(0)=='S'){
      //Serial << "hej" << endl;
      Serial << ID << ":OK" << endl;
      WiFly << ID << ":OK" << endl;
      parse(); //Parse input
      check_state(); //Check if states have changed
      while(WiFly.available())
        WiFly.read();
    }
  }



  if(millis()>=loopmillis+10000)//counter>=100000)// 
  {
    counter++;
    sample(); // Sample inputs
    loopmillis=millis();
    Year = year();
    Month = month();
    Day = day();
    Hour = hour();
    Minute = minute();
    Second = second ();

 
    
    //Send calculated values and state
    //Serial << bufferEmpty << endl;
    if(bufferEmpty) 
    { //Send data if buffer is empty 
      if(!wifi_send()) 
      { //Try to send data and add to buffer when failure
        addToBuffer(); //Add measurements to buffer if they can't be sent
        if(!sendBuffer()){
          Serial << "Reconnect 1" << endl;
          if(!wifi_reconnect()){
            WiFly.getDeviceStatus();
            delay(100);
            if(WiFly.isifUp()==0)
              initialize();

          }
        }
      }
    }

    else 
    { // If buffer is not empty then add latest data to buffer then try to send buffered values
      addToBuffer();
      if(!sendBuffer()){
        Serial << "Reconnect 2" << endl;
        if(!wifi_reconnect()){
          WiFly.getDeviceStatus();
          delay(100);
          if(WiFly.isifUp()==0 && counter==6)
            initialize();

        }

      }
    }
    //WiFly.exitCommandMode();
    if(counter==6) counter=0;
  }
}


