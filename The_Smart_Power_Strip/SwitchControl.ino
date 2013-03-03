//Methods for switching relays


// Algorithm for parsing the control message string into arrays--
void parse() 
{
  int first_semicolon = String(bufUpdate).indexOf(';');
  for(int x = 0; x < 4; x++)
  {
    ControlSignal[x] = String(bufUpdate).charAt(first_semicolon - 1 + x*2) - '0';
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

}

// Function for switching each socket -----------------------------
void power_switching(int socket) 
{
  if(Switch_State[socket] == HIGH) 
  {
    Switch_State[socket] = LOW;
    digitalWrite(socket, LOW);
  }

  else if(Switch_State[socket] == LOW) 
  {
    Switch_State[socket] = HIGH;
    digitalWrite(socket, HIGH);
  }

}
