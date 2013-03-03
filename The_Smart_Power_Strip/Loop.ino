// MAIN LOOP ---------------------------------------------------------------------------------------------------------


void loop() 
{
  if(counter==0) Serial << "RAM at loop: " << freeMemory() << endl;

  if(wifi_read()) //Check for input from server and read
  { 
    
    parse(); //Parse input
    check_state(); //Check if states have changed
  }

  //  else 
  //  {
  //    Serial << "Read failed"<<endl;
  //    wifi_reconnect(false);
  //  }

  counter++;
  if(counter>=100000) 
  {
    //sample(); // Sample inputs
    //powerCalc();//Calculate active power  
    //Send calculated values and state
    if(bufferIsEmpty) 
    { //Send data if buffer is empty 
      if(!wifi_send()) 
      { //Try to send data and add to buffer when failure
        //addToBuffer(); //Add measurements to buffer if they can't be sent
        //Reconnect if send_update fails (???)
        //wifi_reconnect(false);
      }
    }

    else 
    { // If buffer is not empty then add latest data to buffer then try to send buffered values
      //addToBuffer();
      //sendBuffered();
    }
    counter=0;
  }
}


