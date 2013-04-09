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
    wifi_reconnect();
  }
}
     
     
        
