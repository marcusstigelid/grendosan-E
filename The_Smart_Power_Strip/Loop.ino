// MAIN LOOP ---------------------------------------------------------------------------------------------------------


void loop() 
{

  if(counter==0) Serial << "RAM at loop: " << freeMemory() << endl; // Returns free RAM

  if(wifi_read()) //Check for input from server and read
  { 
    parse(); //Parse input
    check_state(); //Check if states have changed
  }


//  else 
//  {
//    Serial << "Fail"<<endl;
//    wifi_reconnect(); 
//  }

  counter++;
  if(counter>=100000) 
  {
    //sample(); // Sample inputs
    Year = year();
    Month = month();
    Day = day();
    Hour = hour();
    Minute = minute();
    Second = second ();
    //powerCalc();//Calculate active power  
    //Send calculated values and state
    if(bufferEmpty) 
    { //Send data if buffer is empty 
      if(!wifi_send()) 
      { //Try to send data and add to buffer when failure
        addToBuffer(); //Add measurements to buffer if they can't be sent
        if(!sendBuffer()){

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

        if(!wifi_reconnect()){
          WiFly.getDeviceStatus();
          if(!WiFly.isAssociated())
            initialize();

        }

      }
    }
    counter=0;
  }
}

