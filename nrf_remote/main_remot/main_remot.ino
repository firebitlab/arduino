/*
  Arduino Wireless Communication Tutorial
      Example 1 - Transmitter Code

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

Data_Package data; // Create a variable with the above structure

void setupNRF() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

#define batas 50
int batasServoKiri = 90 - batas;
int batasServoKanan = 90 + batas;

#include <JoystickShield.h> // include JoystickShield Library
JoystickShield joystickShield; // create an instance of JoystickShield object

void setup() {
  Serial.begin(115200);

  delay(100);
  joystickShield.calibrateJoystick();

  setupNRF();
}

unsigned long previousMillis = 0;
const long interval = 100;
void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    joystickShield.processEvents(); // process events

    if (joystickShield.isUpButton()) {
      //Serial.println("Up Button Clicked") ;
      data.mundur = 150;
    }
    else data.mundur = 0;


    //    if (joystickShield.isRightButton()) {
    //      Serial.println("Right Button Clicked") ;
    //
    //    }
    //

    if (joystickShield.isDownButton()) {
      //Serial.println("Down Button Clicked") ;
      data.maju = 150;
    }
    else if (joystickShield.isLeftButton()) {
      //Serial.println("Left Button Clicked") ;
      data.maju = 220;
    }
    else data.maju = 0;


    // new position functions
    int x = joystickShield.xAmplitude();
    //int y = joystickShield.yAmplitude();
    //Serial.print("x ");  Serial.println(x);
    //Serial.print(" y "); Serial.println(y);

    int xServo = map(x, -100, 100, batasServoKiri, batasServoKanan);
    //int yServo = map(y, -100, 100, 0, 180);

    //Serial.print("x ");  Serial.println(xServo);
    data.servo = xServo;
    //Serial.print(" y "); Serial.println(yServo);


    //const char text[] = "Hello World";
    radio.write(&data, sizeof(Data_Package));
    //Serial.println("Kirim");
  }
}
