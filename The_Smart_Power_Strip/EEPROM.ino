//Methods for storing network credentials on EEPROM


//Load WiFi credentials and FLASH addresses from EEPROM ------------------------
void loadEEPROM() {
  // To make sure there are correct settings.
  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
    EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
    EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
    for (unsigned int t=0; t<sizeof(storage); t++)
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);

  //Update ssid and passphrase and addresses from storage structure
  for(int i=0;i<32;i++){
    ssid[i]=storage.ssid[i];
    passphrase[i]=storage.passphrase[i]; 
  }
  readIndex[0] = storage.readIndex[0];
  readIndex[1] = storage.readIndex[1];
  readIndex[2] = storage.readIndex[2];
  storeIndex[0] = storage.storeIndex[0];
  storeIndex[1] = storage.storeIndex[1];
  storeIndex[2] = storage.storeIndex[2];
  securityType = storage.securityType;
  Switch_State[0] = storage.Switch_State[0];
  Switch_State[1] = storage.Switch_State[1];
  Switch_State[2] = storage.Switch_State[2];
  Switch_State[3] = storage.Switch_State[3];  
}


//Save WiFi credentials and FLASH addresses to EEPROM -------------------------
void saveEEPROM() {
  //Update ssid and passphrase and addresses to storage structure
  for(int i=0;i<32;i++){
    storage.ssid[i]=ssid[i];
    storage.passphrase[i]=passphrase[i];
  }
  storage.readIndex[0] = readIndex[0];
  storage.readIndex[1] = readIndex[1];
  storage.readIndex[2] = readIndex[2];
  storage.storeIndex[0] = storeIndex[0];
  storage.storeIndex[1] = storeIndex[1];
  storage.storeIndex[2] = storeIndex[2];
  storage.securityType = securityType;
  storage.Switch_State[0] = Switch_State[0];
  storage.Switch_State[1] = Switch_State[1];
  storage.Switch_State[2] = Switch_State[2];
  storage.Switch_State[3] = Switch_State[3];

  //Write storage to EEPROM
  for (unsigned int t=0; t<sizeof(storage); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));
}
