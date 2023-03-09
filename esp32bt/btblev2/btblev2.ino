#define pOut1 LED_BUILTIN
#define NYALA HIGH

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>



BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
uint8_t txValue = 0, state = 0;
String inputString;
int dataInt;
bool stringComplete = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      inputString = rxValue.c_str(); // Convert to standard c string format
      //Serial.println(inputString);
      stringComplete = true;
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(pOut1, OUTPUT);
  digitalWrite(pOut1, !NYALA);

  // Create the BLE Device
  BLEDevice::init("ESP32 BLE");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    serialComplete();
  }
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
    else if (dataInt == 2)
      digitalWrite(pOut1, !NYALA);

  }
}
