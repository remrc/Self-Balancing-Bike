#define MPU6050 0x68              // Device address
#define ACCEL_CONFIG 0x1C         // Accelerometer configuration address
#define GYRO_CONFIG  0x1B         // Gyro configuration address

//Registers: Accelerometer, Temp, Gyroscope
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C

#define accSens 0             // 0 = 2g, 1 = 4g, 2 = 8g, 3 = 16g
#define gyroSens 1            // 0 = 250rad/s, 1 = 500rad/s, 2 1000rad/s, 3 = 2000rad/s

void writeTo(byte device, byte address, byte value) {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(value);
  Wire.endTransmission(true);
}

void save() {
    EEPROM.put(0, offsets);
    delay(100);
    EEPROM.get(0, offsets);
    if (offsets.ID == 35) calibrated = true;
    calibrating = false;
    Serial.println("calibrating off");
    beep();
}

void beep() {
    digitalWrite(BUZZER, HIGH);
    delay(70);
    digitalWrite(BUZZER, LOW);
    delay(80);
}

void angle_calc() {

  Wire.beginTransmission(MPU6050);
  Wire.write(ACCEL_YOUT_H);                       
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050, 2, true);   
  AcY = Wire.read() << 8 | Wire.read(); 
  
  Wire.beginTransmission(MPU6050);
  Wire.write(ACCEL_ZOUT_H);                  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050, 2, true);  
  AcZ = Wire.read() << 8 | Wire.read(); 

  Wire.beginTransmission(MPU6050);
  Wire.write(GYRO_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050, 2, true); 
  GyX = Wire.read() << 8 | Wire.read();

  AcYc = AcY - offsets.AcY;
  AcZc = AcZ - offsets.AcZ;
  GyX -= GyX_offset;

  robot_angle += GyX * loop_time / 1000 / 65.536;                 
  Acc_angle = -atan2(AcYc, -AcZc) * 57.2958;   
  robot_angle = robot_angle * Gyro_amount + Acc_angle * (1.0 - Gyro_amount);
  
  if (abs(robot_angle) > 10) vertical = false;
  if (abs(robot_angle) < 0.4) vertical = true;
  
}

void angle_setup() {
  Wire.begin();
  delay(100);
  writeTo(MPU6050, PWR_MGMT_1, 0);
  writeTo(MPU6050, ACCEL_CONFIG, accSens << 3);   // Specifying output scaling of accelerometer
  writeTo(MPU6050, GYRO_CONFIG, gyroSens << 3);   // Specifying output scaling of gyroscope
  delay(100);

  for (int i = 0; i < 1024; i++) {
    angle_calc();
    GyX_offset_sum += GyX;
    delay(3);
  }
  GyX_offset = GyX_offset_sum >> 10;
  beep();
  beep();
  Serial.print("GyX offset: ");  Serial.println(GyX_offset);
}

void battVoltage(double voltage) {
  if (voltage > 8 && voltage <= 9.5) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }
}

void Motor1_control(int sp) {
  if (sp > 0) digitalWrite(DIR_1, LOW);
    else digitalWrite(DIR_1, HIGH);
  analogWrite(PWM_1, 255 - abs(sp));
}

void Motor2_control(int sp) {
  if (sp > 0) digitalWrite(DIR_2, LOW);
    else digitalWrite(DIR_2, HIGH);
  analogWrite(PWM_2, 255 - abs(sp));
}

int Tuning() {
  if (!Serial.available())  return 0;
  delay(2);
  char param = Serial.read();               // get parameter byte
  if (!Serial.available()) return 0;
  char cmd = Serial.read();                 // get command byte
  Serial.flush();
  switch (param) {
    case 'p':
      if (cmd == '+')    K1 += 1;
      if (cmd == '-')    K1 -= 1;
      printValues();
      break;
    case 'i':
      if (cmd == '+')    K2 += 0.5;
      if (cmd == '-')    K2 -= 0.5;
      printValues();
      break;
    case 's':
      if (cmd == '+')    K3 += 0.2;
      if (cmd == '-')    K3 -= 0.2;
      printValues();
      break;  
    case 'a':
      if (cmd == '+')    K4 += 0.05;
      if (cmd == '-')    K4 -= 0.05;
      printValues();
      break;    
    case 'c':
      if (cmd == '+' && !calibrating) {
        calibrating = true;
        Serial.println("calibrating on");
      }
      if (cmd == '-' && calibrating)  {
          offsets.ID = 35;
          offsets.AcZ = AcZ + 16384;
          offsets.AcY = AcY;
          Serial.print("AcY: "); Serial.print(offsets.AcY); Serial.print(" AcZ: "); Serial.println(offsets.AcZ); 
          save();
      }
      break;                
   }
   return 1;
}

void printValues() {
  Serial.print("K1: "); Serial.print(K1);
  Serial.print(" K2: "); Serial.print(K2);
  Serial.print(" K3: "); Serial.print(K3,4);
  Serial.print(" K4: "); Serial.println(K4,4);
}

void ENC_READ() {
  byte cur = (!digitalRead(ENC_1) << 1) + !digitalRead(ENC_2);
  byte old = pos & B00000011;
  byte dir = (pos & B00110000) >> 4;
 
  if (cur == 3) cur = 2;
  else if (cur == 2) cur = 3;
 
  if (cur != old) {
  if (dir == 0) {
    if (cur == 1 || cur == 3) dir = cur;
    } else {
      if (cur == 0) {
        if (dir == 1 && old == 3) enc_count--;
        else if (dir == 3 && old == 1) enc_count++;
        dir = 0;
      }
    }
    pos = (dir << 4) + (old << 2) + cur;
  }
}

