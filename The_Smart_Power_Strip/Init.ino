// Initialization ---------------------------------------------------------------------------------------------------------

void initialize()
{

  if (!wifi_init()) // Joined Wifi?
  {
    if(REBOOT == true)
    {
      setup_adhoc();
      wifi_init();
    }
    REBOOT = false;  
  }
  else{
    if(!joined_once){
      boolean leaveloop=false;

      WiFly.setNTP_UTC_Offset(0);
      delay(1000);
      WiFly.setNTP(NTP_SERVER); //NTP server
      delay(2000);
      while(!leaveloop){
        WiFly.SendCommandSimple("time",prompt);
        delay(5000);
        setTime(WiFly.getTime());
        delay(2000);
        WiFly.getDeviceStatus();
        if(year()!=1970){
          leaveloop=true;
          joined_once = true;
        }
        else if(!WiFly.isAssociated())
          leaveloop=true;        
      }
      Year = year();
      Month = month();
      Day = day();
      Hour = hour();
      Minute = minute();
      Second = second ();
      Serial << Year << endl;
    } 
  }
  wifi_reconnect();
}






