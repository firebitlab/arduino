#define p_ph A2
void setup() {

  Serial.begin(9600);
  
}


float adc_ph;
void loop() {
  float ph;
  ph = sensor_ph();
  
  Serial.print(ph); Serial.print(" ");
  Serial.println(adc_ph);
}


float sensor_ph() {
  const int analogInPin = p_ph;
  float avgValue = 0;
  int sensorValue = 0;
  int buf[50], temp;

  //AMBIL 10 SAMPEL ADC
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(p_ph);
    delay(30);
  }
  //AMBIL 10 SAMPEL ADC

  //DIURUTKAN DARI TERKCECIL KE BESAR
  for (int i = 0; i < 9; i++) {
    
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  //DIURUTKAN DARI TERKCECIL KE BESAR

  //DIAMBIL 6 DATA TENGAH index 2-8 KEMUDIAN DIRATA RATA
  for (int i = 2; i < 8; i++) {
    avgValue += buf[i];
  }
  avgValue = avgValue/6;
  //DIAMBIL 6 DATA TENGAH index 2-8 KEMUDIAN DIRATA RATA

  float phValue = -0.0308*avgValue + 23.059;


  adc_ph = avgValue;
  return phValue;
}
