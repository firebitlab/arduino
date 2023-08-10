/*

   13 July 2023
   - Progam RPM belum dimasukan
   - PID belum dicoba
   - Proteksi belum dicoba
   - Kurang tombol reset dan stop pin digital D33
   13 July 2023 Siang
   - PID belum sesuai, algoritma salah
*/


int modee = 0;




//------------------------------------LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcdx(0x3F, 20, 4);
//------------------------------------HX711
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 15; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin

HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

void setuphx() {

  Serial.println();
  Serial.println("Starting...");

  float calibrationValue; // calibration value

#if defined(ESP8266) || defined(ESP32)
  EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch this value from eeprom
#endif
  EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch this value from eeprom

  LoadCell.begin();
  //LoadCell.setReverseOutput();
  unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = false; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration factor (float)
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update());
  Serial.print("Calibration value: ");
  Serial.println(LoadCell.getCalFactor());
  Serial.print("HX711 measured conversion time ms: ");
  Serial.println(LoadCell.getConversionTime());
  Serial.print("HX711 measured sampling rate HZ: ");
  Serial.println(LoadCell.getSPS());
  Serial.print("HX711 measured settlingtime ms: ");
  Serial.println(LoadCell.getSettlingTime());
  Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
  if (LoadCell.getSPS() < 7 || LoadCell.getSPS() > 100 ) {
    lcdx.setCursor(0, 0);
    lcdx.print("Sensor Berat Error");
    while (1) {}
  }
}

float berat;
float beratTarget;
void getHX711() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 500; //increase value to slow down serial print activity

  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      //Serial.print("Load_cell output val: ");

      berat = i - 102231; //nilai hasil kalibrasi dengan tare false
      //berat = i;
      newDataReady = 0;
      t = millis();
      //Serial.println(berat);

      berat = berat / 1000; //konversi ke kg
      if (berat < 0)
        berat = 0;

      if (modee == 0) {
        beratTarget = berat * 0.8025;
        if (beratTarget < 0)
          beratTarget = 0;
      }
    }
  }


}

//------------------------------------DIMMER
#include <RBDdimmer.h>//

#define outputPin  14
#define zerocross  27 // for boards with CHANGEBLE input pins

dimmerLamp dimmer(outputPin, zerocross); //initialase port for dimmer for ESP8266, ESP32, Arduino due boards


void setupDimmer() {
  dimmer.begin(NORMAL_MODE, ON); //dimmer initialisation: name.begin(MODE, STATE)
}
//karena saat uji dimmer diatas 75 malah mati
void setDimmer(int _pow) {
  int poww = _pow * 75 / 100;
  dimmer.setPower(poww);
}

//------------------------------------PZEM
int fault = 0;

#include <PZEM004Tv30.h>
PZEM004Tv30 pzem(&Serial2);

float arus;
int tegangan;
void loop_pzem() {
  float voltage = pzem.voltage();

  if (!isnan(voltage)) {
    tegangan = voltage;
  } else {
    tegangan = -1;
  }

  float current = pzem.current();
  if (!isnan(current)) {
    arus = current;
  } else {
    arus = -1;
  }
}


//------------------------------------DHT
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float suhu;
float suhuTarget = 50;
float errorSuhu;
float kP;
float kI;
float kD;

void getDHT22() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  float suhuu = dht.readTemperature();
  if (isnan(suhuu)) {
    //suhu = -1;
    suhu = suhu;
  }
  else  suhu = suhuu;
}



//------------------------------------PID
#include <PID_v1.h>

//Define Variables we'll be connecting to
double Setpoint, InputPID, OutputPID;

//Specify the links and initial tuning parameters
double Kp = 1.5;
double Ki = 4;
double Kd = 1;
PID myPID(&InputPID, &OutputPID, &Setpoint, Kp, Ki, Kd, DIRECT);

void setupPID()
{
  Setpoint = suhuTarget;
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 100);

}

//------------------------------------RPM
unsigned long rpm = 0;
int lastr;
#define pRPM 26
unsigned long rpmnow = 0;
unsigned long lastrpm = 0;
//unsigned long timerpm;
void IRAM_ATTR detectsMovement() {

  rpmnow = millis();
  rpm = (rpmnow - lastrpm);
  if (rpm >= 17) {
    rpm = 60000 / rpm;
    lastr = rpm;
  }
  else {
    rpm = lastr;
  }
  lastrpm = rpmnow;

  if (OutputPID == 0)
    rpm = 0;
}

//------------------------------------BLYNK
/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6K7jV1UKo"
#define BLYNK_TEMPLATE_NAME "Gabah"
#define BLYNK_AUTH_TOKEN "W90jiIXP704PlcGsXZTyk1l6c0zLf8du"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Robot Education";
char pass[] = "robotika";

void pushBlynk()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, suhu);
  Blynk.virtualWrite(V1, berat);
  Blynk.virtualWrite(V2, tegangan);
  Blynk.virtualWrite(V3, arus);
  Blynk.virtualWrite(V4, rpm);


  Blynk.virtualWrite(V5, beratTarget);
  Blynk.virtualWrite(V6, suhuTarget);
  Blynk.virtualWrite(V7, OutputPID);

  Blynk.virtualWrite(V8, fault);
}

void setupBlynk()
{
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

}

void runBlynk()
{
  Blynk.run();
}

//------------------------------------RELAY BUZZER

#define pRelay 13
#define pBuzzer 12
#define pLEDtick LED_BUILTIN
#define pBstart 25
#define pBstop 33


void buzzerOff() {
  digitalWrite(pBuzzer, LOW);
}

void buzzerOn() {
  digitalWrite(pBuzzer, HIGH);
}




void setup() {
  Serial.begin(9600);

  pinMode(pRelay, OUTPUT);
  pinMode(pLEDtick, OUTPUT);
  pinMode(pBuzzer, OUTPUT);

  pinMode(pBstart, INPUT_PULLUP);
  pinMode(pBstop, INPUT_PULLUP);
  pinMode(pRPM, INPUT_PULLUP);

  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(pRPM), detectsMovement, RISING);


  lcdx.init();
  lcdx.backlight();
  lcdx.setCursor(0, 0);
  lcdx.print("PENGECEKAN.... ");


  dht.begin();

  setuphx();
  setupDimmer(); delay(500); setDimmer(0);
  setupBlynk();
  setupPID();

  //OFF
  buzzerOff();
  digitalWrite(pRelay, LOW);
  lcdx.clear();

  OutputPID = 0;
  setDimmer(OutputPID);
}


int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;
int iInterval = 0;

void loop() {
  taskPriority();
  taskTimer();

}

void taskPriority() {
  getHX711();
  runBlynk();
  cektombol();
  switchmode();
  proteksi();
}

void proteksi() {
  if (tegangan > 250 || arus > 1.2) {
    fault = 1;
    modee = 3;

  }

}
void taskTimer() {
  unsigned long startt = millis();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    if (ledState) {
      ledState = LOW;
    }
    else {
      ledState = HIGH;
    }
    digitalWrite(pLEDtick, ledState);

    if (fault == 1) {
      digitalWrite(pBuzzer, ledState);
    }
    else buzzerOff();


    getDHT22();



    iInterval++;
    if (iInterval == 2)
    {
      iInterval = 0;
      loop_pzem();

      displayLCD();
      pushBlynk();
    }

    if (modee == 1) {
      Serial.print(InputPID); Serial.print(" ");
      Serial.print(OutputPID); Serial.print(" ");
      Serial.println();
    }
  }
}
void cektombol() {
  if (digitalRead(pBstart) == LOW) {
    modee = 1;

  }
  else if (digitalRead(pBstop) == LOW) {
    modee = 0;

  }

}
void switchmode() {
  //MODE STANDBY
  if (modee == 0) {
    OutputPID = 0;
    setDimmer(OutputPID);
    //    lcdx.setCursor(11, 0);
    //    lcdx.print("S");
    //rpm = 0;
  }
  //MODE RUN
  else if (modee == 1) {

    InputPID = suhu;
    myPID.Compute();


    setDimmer(OutputPID);

    //    Serial.print(rpm); Serial.print(" ");
    //    Serial.println();


    if (berat < beratTarget) {
      modee = 2;
      OutputPID = 0;
      setDimmer(OutputPID);
    }
  }
  //MODE DONE
  else if (modee == 2) {
    OutputPID = 0;
    setDimmer(OutputPID);
    //rpm = 0;
    //Serial.println("Selesai");
    //    lcdx.setCursor(11, 0);
    //    lcdx.print("D");
  }
  //MODE PROTEKSI
  else if (modee == 3) {

    OutputPID = 0;
    setDimmer(OutputPID);
    //rpm = 0;
    //Serial.println("Proteksi");
    OutputPID = 0;
    setDimmer(OutputPID);
  }
}


void displayLCD() {
  lcdx.clear();

  if (modee == 0) {
    lcdx.setCursor(11, 0);
    lcdx.print("S");
  }
  else if (modee == 1) {
    lcdx.setCursor(11, 0);
    lcdx.print("R");
  }
  else if (modee == 2) {
    lcdx.setCursor(11, 0);
    lcdx.print("D");
  }
  else if (modee == 3) {
    lcdx.setCursor(11, 0);
    lcdx.print("F");
  }

  lcdx.setCursor(0, 0);
  lcdx.print("Tc ");
  lcdx.print(suhu, 1);
  //lcdx.print(" ");
  lcdx.setCursor(8, 0);
  lcdx.print((char)223);
  lcdx.print("C");

  lcdx.setCursor(0, 1);
  lcdx.print("Tt ");
  lcdx.print(suhuTarget, 1);
  //lcdx.print(" ");
  lcdx.setCursor(8, 1);
  lcdx.print((char)223);
  lcdx.print("C");


  lcdx.setCursor(0, 2);
  lcdx.print("Wc ");
  lcdx.print(berat, 1);
  //lcdx.print(" ");
  lcdx.setCursor(8, 2);
  lcdx.print("kG");

  lcdx.setCursor(0, 3);
  lcdx.print("Wt ");
  lcdx.print(beratTarget, 1);
  //lcdx.print(" ");
  lcdx.setCursor(8, 3);
  lcdx.print("kG");


  lcdx.setCursor(13, 0);
  lcdx.print(rpm);
  //lcdx.print("");
  lcdx.setCursor(17, 0);
  lcdx.print("RPM");


  lcdx.setCursor(11, 1);
  lcdx.print("D ");
  lcdx.print(OutputPID, 0);
  //lcdx.print("  ");
  lcdx.setCursor(19, 1);
  lcdx.print("%");

  lcdx.setCursor(11, 2);
  lcdx.print("V ");
  lcdx.print(tegangan);
  //lcdx.print("  ");
  lcdx.setCursor(19, 2);
  lcdx.print("V");


  lcdx.setCursor(11, 3);
  lcdx.print("I ");
  lcdx.print(arus, 1);
  //lcdx.print("  ");
  lcdx.setCursor(19, 3);
  lcdx.print("A");
}
