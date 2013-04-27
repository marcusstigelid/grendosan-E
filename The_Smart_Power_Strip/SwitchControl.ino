//Methods for switching relays


// Algorithm for parsing the control message string into arrays--
void parse() 
{ 
  int state;

  int first_semicolon = String(bufTemp).indexOf(';');
  for(int x = 0; x < 4; x++)
  {
    state = String(bufTemp).charAt(first_semicolon - 1 + x*2);
    if(state==48)
      ControlSignal[x] = false;
    else if(state==49)
      ControlSignal[x] = true;
    else
      ControlSignal[x] = Switch_State[x];
  }
}

// Check if there are changes between Switch_State[] - Switch_State_ControlSignal[] and IF there are... SWITCH!!!!---
void check_state() 
{
  for(int x = 0; x < 4; x++)
  {
    if(Switch_State[x] != ControlSignal[x]) 
    {
      power_switching(x);
    }

  }
  saveEEPROM();
} 

// Function for switching each socket -----------------------------
void power_switching(int socket) 
{
  if(Switch_State[socket] == HIGH) 
  {
    Switch_State[socket] = LOW;
    digitalWrite(digitalLayout[socket], LOW);
  }

  else if(Switch_State[socket] == LOW) 
  {
    Switch_State[socket] = HIGH;
    digitalWrite(digitalLayout[socket], HIGH);
  }
}
