void setup() {
  Serial.begin(9600);
  setupDHT22();
}


void loop() {
  if(taskTimer1s()){
    getDHT22();
  }
}


unsigned long previousMillis = 0;
const long interval = 1000;
boolean taskTimer1s() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  else return false;
}
