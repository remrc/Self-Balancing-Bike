#define    STX          0x02
#define    ETX          0x03
byte cmd[8] = {0, 0, 0, 0, 0, 0, 0, 0};
byte buttonStatus = 0; 

void readControlParameters() {
  if (Serial.available())  {                           // data received from smartphone
    //delay(1);
    cmd[0] =  Serial.read();  
    if(cmd[0] == STX)  {
      int i = 1;      
      while (Serial.available())  {
        //delay(1);
        cmd[i] = Serial.read();
        if(cmd[i] > 127 || i > 7)                 break;     // Communication error
        if((cmd[i] == ETX) && (i == 2 || i == 7)) break;     // Button or Joystick data
        i++;
      }
      if (i == 2) getButtonState(cmd[1]);                  // 3 Bytes  ex: < STX "C" ETX >
      else if (i == 7) getJoystickState(cmd);              // 6 Bytes  ex: < STX "200" "180" ETX >
    }
  } 
}

void getJoystickState (byte data[8])    {
  int joyX = (data[1] - 48) * 100 + (data[2] - 48) * 10 + (data[3] - 48);   // obtain the Int from the ASCII representation
  int joyY = (data[4] - 48) * 100 + (data[5] - 48) * 10 + (data[6] - 48);
  joyX = joyX - 200;                                                        // Offset to avoid
  joyY = joyY - 200;                                                        // transmitting negative numbers

  if (joyX < -100 || joyX > 100 || joyY < -100 || joyY > 100) return;       // commmunication error
  if (joyX < - 10 || joyX > 10)  { // dead zone
    if (joyX > 0) // exponential
      steering_remote = (-joyX * joyX + 0.1 * joyX) / 100.0;
    else   
      steering_remote = (joyX * joyX + 0.1 * joyX) / 100.0;
  } else 
      steering_remote = 0;
  steering_remote = -map(steering_remote, -100, 100, -STEERING_MAX, STEERING_MAX);
  if (joyY < - 10 || joyY > 10)  // dead zone 
     speed_remote = map(joyY, 100, -100, SPEED_MAX, -SPEED_MAX);
  else
     speed_remote = 0;        
}

void getButtonState (int bStatus)  {
  switch (bStatus) {
// -----------------  BUTTON #1  -----------------------
   case 'A':
      buttonStatus |= B000001;        // ON
      break;
    case 'B':
      buttonStatus &= B111110;        // OFF
      break;
// -----------------  BUTTON #2  -----------------------
   case 'C':
      buttonStatus |= B000010;        // ON    
      break;
    case 'D':
      buttonStatus &= B111101;        // OFF    
      break;
  }
}

String getButtonStatusString()  {
  String bStatus = "";
  for (int i = 0; i < 6; i++)  {
    if (buttonStatus & (B100000 >> i))      bStatus += "1";
    else                                    bStatus += "0";
  }
  return bStatus;
}

void sendControlParameters() {
  Serial.print((char)STX);                                                // Start of Transmission
  Serial.print(getButtonStatusString());  Serial.print((char)0x1);        // buttons status feedback
  Serial.print(0);                        Serial.print((char)0x4);        // datafield #1
  Serial.print(0);                        Serial.print((char)0x5);        // datafield #2
  Serial.print(0);                                                        // datafield #3
  Serial.print((char)ETX);                                                // End of Transmission
}
