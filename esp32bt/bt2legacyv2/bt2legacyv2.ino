#define pOut1 LED_BUILTIN
#define NYALA HIGH

#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  pinMode(pOut1, OUTPUT);
  digitalWrite(pOut1, !NYALA);
}

String inputString = "";
int dataInt;
bool stringComplete = false;


void loop() {
  serialComplete(); //Aktifasi Relay
  serialCek();      //Cek data Bluetooth
}

void serialComplete() {
  if (stringComplete) {
    dataInt = inputString.toInt();
    inputString = "";
    stringComplete = false;

    
    //RELAY
    Serial.println(dataInt); 
    if (dataInt == 1)
      digitalWrite(pOut1, NYALA);
    else if(dataInt==2)
      digitalWrite(pOut1, !NYALA);
      
    
  }
}
void serialCek() {
  while (SerialBT.available()) {
    char inChar = SerialBT.read();
    if (inChar == '\n')
      stringComplete = true;
    else
      inputString += inChar;
  }
}
