#include <ServoTimer2.h>

#define PWM_1         9
#define DIR_1         7
#define PWM_2         10
#define DIR_2         5 

#define ENC_1         2
#define ENC_2         3

#define BRAKE         8
#define BUZZER        12
#define VBAT          A7

float loop_time = 50;

volatile byte pos;
volatile int enc_count = 0;
int16_t motor_speed;
int f = 1;
boolean lock = 1;

long currentT, previousT_1, previousT_2 = 0; 

ServoTimer2 steering_servo;

void beep() {
    digitalWrite(BUZZER, HIGH);
    delay(70);
    digitalWrite(BUZZER, LOW);
    delay(80);
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

void setup() {
  Serial.begin(115200);

  // Pins D9 and D10 - 7.8 kHz
  TCCR1A = 0b00000001; // 8bit
  TCCR1B = 0b00001010; // x8 fast pwm

  steering_servo.attach(A3); 
  steering_servo.write(1500); 

  pinMode(DIR_1, OUTPUT);  
  pinMode(DIR_2, OUTPUT);
  pinMode(BRAKE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(ENC_1, INPUT);
  pinMode(ENC_2, INPUT);
  
  Motor1_control(0);
  Motor2_control(0);

  attachInterrupt(0, ENC_READ, CHANGE);
  attachInterrupt(1, ENC_READ, CHANGE);
}

void loop() {

  currentT = millis();

  if (currentT - previousT_1 >= loop_time) {

    motor_speed = -enc_count;
    enc_count = 0;

    switch (f) {
      case 1:
        if (lock) Serial.println("Look to reaction wheel from servo side...");
        lock = 0;
        break;
      case 2:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Slow rotating reaction wheel (clockwise).");
        lock = 0;
        Motor1_control(-30);
        break;
      case 3:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Speed up.");
        lock = 0;
        Motor1_control(-60);
        break; 
      case 4:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Speed up.");
        lock = 0;
        Motor1_control(-110);
        break;    
      case 5:
        digitalWrite(BRAKE, LOW);
        if (lock) Serial.println("Stop.");
        lock = 0;
        Motor1_control(0);
        break;      
      case 6:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Slow rotating reaction wheel (counter clockwise).");
        lock = 0;
        Motor1_control(30);
        break;
      case 7:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Speed up.");
        lock = 0;
        Motor1_control(60);
        break; 
      case 8:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Speed up.");
        lock = 0;
        Motor1_control(110);
        break;    
      case 9:
        digitalWrite(BRAKE, LOW);
        if (lock) Serial.println("Stop.");
        lock = 0;
        Motor1_control(0);
        break; 
      case 10:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Checking encoder...");
        lock = 0;
        Motor1_control(-60);
        break; 
     case 11:
        digitalWrite(BRAKE, LOW);
        if (lock && motor_speed > 20) {
          Serial.println("Encoder OK.");
          Serial.print("Speed: "); Serial.println(motor_speed);
          Serial.println("Stop.");
        } else if (lock && motor_speed <= 0) {
          Serial.println("Encoder FAIL.");
          Serial.print("Speed: "); Serial.println(motor_speed);
          Serial.println("Stop.");
        }
        lock = 0;
        Motor1_control(0);
        break;       
     case 12:
        if (lock) Serial.println("Checking steering servo (to left)...");
        lock = 0;
        steering_servo.write(1200); 
        break; 
     case 13:
        if (lock) Serial.println("Checking steering servo (to right)...");
        lock = 0;
        steering_servo.write(1800); 
        break;  
     case 14:
        if (lock) Serial.println("Checking steering servo (to center)...");
        lock = 0;
        steering_servo.write(1500); 
        break;  
     case 15:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Slow rotating rear wheel (forward).");
        lock = 0;
        Motor2_control(30);
        break;
      case 16:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Speed up.");
        lock = 0;
        Motor2_control(60);
        break; 
      case 17:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Speed up.");
        lock = 0;
        Motor2_control(110);
        break;    
      case 18:
        digitalWrite(BRAKE, LOW);
        if (lock) Serial.println("Stop.");
        lock = 0;
        Motor2_control(0);
        break;      
      case 19:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Slow rotating rear wheel (backward).");
        lock = 0;
        Motor2_control(-30);
        break;
      case 20:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Speed up.");
        lock = 0;
        Motor2_control(-60);
        break; 
      case 21:
        digitalWrite(BRAKE, HIGH);
        if (lock) Serial.println("Speed up.");
        lock = 0;
        Motor2_control(-110);
        break;    
      case 22:
        digitalWrite(BRAKE, LOW);
        if (lock) Serial.println("Stop.");
        lock = 0;
        Motor2_control(0);
        break; 
    }
    previousT_1 = currentT;
  }
  
  if (currentT - previousT_2 >= 3000) { 
     f++;
     if (f == 23) {
      f = 1;
      Serial.println("Beep!");
      beep();  
     }
     lock = 1;
     previousT_2 = currentT;
  }
}

