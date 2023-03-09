/*
  Arduino Wireless Communication Tutorial
        Example 1 - Receiver Code

  by Dejan Nedelkovski, www.HowToMechatronics.com

  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN

const byte address[6] = "00001";

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte servo;
  byte maju;
  byte mundur;
};

Data_Package data; //Create a variable with the above structure

void setupNRF() {
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}


#define Pin_D1_L  2 //lpwm
#define Pin_D2_L  4 //rpwm
#define Pin_E_L   3 //pwm enable


#include <Servo.h>
Servo myservo;
int pos = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(Pin_D1_L, OUTPUT);
  pinMode(Pin_D2_L, OUTPUT);
  pinMode(Pin_E_L, OUTPUT);
  motor_stop();
  myservo.attach(5);
  setupNRF();
}

void loop()
{
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    Serial.print("a: ");
    Serial.print(data.servo);
    Serial.print(" b: ");
    Serial.print(data.maju);
    Serial.print(" c: ");
    Serial.println(data.mundur);
  }

  if (data.maju > 10) {
    motor_cw(data.maju);
  }
  else if (data.mundur > 10) {
    motor_ccw(data.mundur);
  }
  else motor_stop();

  myservo.write(pos);
  
  //  motor_cw();     delay(1000);
  //  motor_stop();   delay(1000);
  //  motor_ccw();     delay(1000);
  //  motor_stop();   delay(1000);
  //
  //
  //  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
  //    // in steps of 1 degree
  //    myservo.write(pos);              // tell servo to go to position in variable 'pos'
  //    delay(15);                       // waits 15ms for the servo to reach the position
  //  }
  //  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
  //    myservo.write(pos);              // tell servo to go to position in variable 'pos'
  //    delay(15);                       // waits 15ms for the servo to reach the position
  //  }
}

void motor_cw(byte speedx)
{
  analogWrite(Pin_E_L, speedx);
  digitalWrite(Pin_D1_L, HIGH);
  digitalWrite(Pin_D2_L, LOW);
}

void motor_ccw(byte speedx)
{
  analogWrite(Pin_E_L, speedx); //0-255
  digitalWrite(Pin_D1_L, LOW);
  digitalWrite(Pin_D2_L, HIGH);
}

void motor_stop()
{
  digitalWrite(Pin_D1_L, LOW);
  digitalWrite(Pin_D2_L, LOW);
  analogWrite(Pin_E_L, 0);
}
