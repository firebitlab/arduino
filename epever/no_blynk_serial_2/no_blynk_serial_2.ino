// CONNECT THE RS485 MODULE.
// MAX485 module <-> ESP8266
//  - DI -> D10 / GPIO1 / TX
//  - RO -> D9 / GPIO3 / RX
//  - DE and RE are interconnected with a jumper and then connected do eighter pin D1 or D2
//  - VCC to +5V / VIN on ESP8266
//  - GNDs wired together
// -------------------------------------
// You do not need to disconnect the RS485 while uploading code.
// After first upload you should be able to upload over WiFi
// Tested on NodeMCU + MAX485 module
// RJ 45 cable: Green -> A, Blue -> B, Brown -> GND module + GND ESP8266
// MAX485: DE + RE interconnected with a jumper and connected to D1 or D2
//
// Developed by @jaminNZx
// With modifications by @tekk

#include <ModbusMaster.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

const int defaultBaudRate = 115200;
int timerTask1, timerTask2, timerTask3;
float battChargeCurrent, battDischargeCurrent, battOverallCurrent, battChargePower;
float bvoltage, ctemp, btemp, bremaining, lpower, lcurrent, pvvoltage, pvcurrent, pvpower;
float stats_today_pv_volt_min, stats_today_pv_volt_max;
uint8_t result;
bool rs485DataReceived = true;
bool loadPoweredOn = true;


ModbusMaster node;

void preTransmission() {
}

void postTransmission() {
}

// A list of the regisities to query in order
typedef void (*RegistryList[])();

RegistryList Registries = {
  AddressRegistry_3100,
  AddressRegistry_3106,
  AddressRegistry_310D,
  AddressRegistry_311A,
  AddressRegistry_331B,
};

// keep log of where we are
uint8_t currentRegistryNumber = 0;

// function to switch to next registry
void nextRegistryNumber() {
  // better not use modulo, because after overlow it will start reading in incorrect order
  currentRegistryNumber++;
  if (currentRegistryNumber >= ARRAY_SIZE(Registries)) {
    currentRegistryNumber = 0;
  }
}

// ****************************************************************************

// --------------------------------------------------------------------------------


// exec a function of registry read (cycles between different addresses)
void executeCurrentRegistryFunction() {
  Registries[currentRegistryNumber]();
}

uint8_t setOutputLoadPower(uint8_t state) {
  Serial.print("Writing coil 0x0006 value to: ");
  Serial.println(state);

  delay(10);
  // Set coil at address 0x0006 (Force the load on/off)
  result = node.writeSingleCoil(0x0006, state);

  if (result == node.ku8MBSuccess) {
    node.getResponseBuffer(0x00);
    Serial.println("Success.");
  }

  return result;
}

// callback to on/off button state changes from the Blynk app


uint8_t readOutputLoadState() {
  delay(10);
  result = node.readHoldingRegisters(0x903D, 1);

  if (result == node.ku8MBSuccess) {
    loadPoweredOn = (node.getResponseBuffer(0x00) & 0x02) > 0;

    Serial.print("Set success. Load: ");
    Serial.println(loadPoweredOn);
  } else {
    // update of status failed
    Serial.println("readHoldingRegisters(0x903D, 1) failed!");
  }
  return result;
}

// reads Load Enable Override coil
uint8_t checkLoadCoilState() {
  Serial.print("Reading coil 0x0006... ");

  delay(10);
  result = node.readCoils(0x0006, 1);

  Serial.print("Result: ");
  Serial.println(result);

  if (result == node.ku8MBSuccess) {
    loadPoweredOn = (node.getResponseBuffer(0x00) > 0);

    Serial.print(" Value: ");
    Serial.println(loadPoweredOn);
  } else {
    Serial.println("Failed to read coil 0x0006!");
  }

  return result;
}

// -----------------------------------------------------------------

void AddressRegistry_3100() {
  result = node.readInputRegisters(0x3100, 6);

  if (result == node.ku8MBSuccess) {

    pvvoltage = node.getResponseBuffer(0x00) / 100.0f;
    Serial.print("PV Voltage: ");
    Serial.println(pvvoltage);

    pvcurrent = node.getResponseBuffer(0x01) / 100.0f;
    Serial.print("PV Current: ");
    Serial.println(pvcurrent);

    pvpower = (node.getResponseBuffer(0x02) | node.getResponseBuffer(0x03) << 16) / 100.0f;
    Serial.print("PV Power: ");
    Serial.println(pvpower);

    bvoltage = node.getResponseBuffer(0x04) / 100.0f;
    Serial.print("Battery Voltage: ");
    Serial.println(bvoltage);

    battChargeCurrent = node.getResponseBuffer(0x05) / 100.0f;
    Serial.print("Battery Charge Current: ");
    Serial.println(battChargeCurrent);
  }
}

void AddressRegistry_3106()
{
  result = node.readInputRegisters(0x3106, 2);

  if (result == node.ku8MBSuccess) {
    battChargePower = (node.getResponseBuffer(0x00) | node.getResponseBuffer(0x01) << 16)  / 100.0f;
    Serial.print("Battery Charge Power: ");
    Serial.println(battChargePower);
  }
}

void AddressRegistry_310D()
{
  result = node.readInputRegisters(0x310D, 3);

  if (result == node.ku8MBSuccess) {
    lcurrent = node.getResponseBuffer(0x00) / 100.0f;
    Serial.print("Load Current: ");
    Serial.println(lcurrent);

    lpower = (node.getResponseBuffer(0x01) | node.getResponseBuffer(0x02) << 16) / 100.0f;
    Serial.print("Load Power: ");
    Serial.println(lpower);
  } else {
    rs485DataReceived = false;
    Serial.println("Read register 0x310D failed!");
  }
}

void AddressRegistry_311A() {
  result = node.readInputRegisters(0x311A, 2);

  if (result == node.ku8MBSuccess) {
    bremaining = node.getResponseBuffer(0x00) / 1.0f;
    Serial.print("Battery Remaining %: ");
    Serial.println(bremaining);

    btemp = node.getResponseBuffer(0x01) / 100.0f;
    Serial.print("Battery Temperature: ");
    Serial.println(btemp);
  } else {
    rs485DataReceived = false;
    Serial.println("Read register 0x311A failed!");
  }
}

void AddressRegistry_331B() {
  result = node.readInputRegisters(0x331B, 2);

  if (result == node.ku8MBSuccess) {
    battOverallCurrent = (node.getResponseBuffer(0x00) | node.getResponseBuffer(0x01) << 16) / 100.0f;
    Serial.print("Battery Discharge Current: ");
    Serial.println(battOverallCurrent);
  } else {
    rs485DataReceived = false;
    Serial.println("Read register 0x331B failed!");
  }
}


#define p_ledtick LED_BUILTIN
int ledState = LOW; 
unsigned long previousMillis = 0;

#define RXD2 16
#define TXD2 17

void setup()
{
  Serial.begin(defaultBaudRate);
  Serial2.begin(defaultBaudRate);
  
  // Modbus slave ID 1
  node.begin(1, Serial2);

  // callbacks to toggle DE + RE on MAX485
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);


  pinMode(p_ledtick, OUTPUT);


  Serial.println("Setup OK!");
  Serial.println("----------------------------");
  Serial.println();
}

void loop()
{
  unsigned long currentMillis = millis();
  const long interval = 5000;
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;

    if (ledState == LOW) 
    {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(p_ledtick, ledState);

    executeCurrentRegistryFunction();
    nextRegistryNumber();
  }
}
