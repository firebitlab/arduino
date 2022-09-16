#define psensor1 A1
#define psensor2 A2
#define psensor3 A3
#define psensor4 A4
#define psensor5 A5

#define nsensor 5
int sensor[nsensor];
float threshold[nsensor];
int data_hitam[nsensor] = {0, 0, 0, 0, 0};
int data_putih[nsensor] = {994, 992, 994, 994, 995};


void hitung_treshold_sensor()
{
  for (int i = 0; i < nsensor; i++)
  {
    threshold[i] = data_hitam[i] - ((data_hitam[i] - data_putih[i]) * sensitifity_sensor); //0.0 - 1.0 makin besar makin sensitif
  }
}

void baca_sensor()
{
  sensor[0] = analogRead(psensor1); delay(2);
  sensor[1] = analogRead(psensor2); delay(2);
  sensor[2] = analogRead(psensor3); delay(2);
  sensor[3] = analogRead(psensor4); delay(2);
  sensor[4] = analogRead(psensor5); delay(2);
  
}

void print_sensor()
{
  for (int i = 0; i < nsensor; i++)
  {
    Serial.print(sensor[i]);
    Serial.print("-");
  }
  Serial.println("");
}


String inputString = "";

unsigned char baca_garis()
{
  baca_sensor();
  for (unsigned char i = 0; i < nsensor; i++)
  {
    if (sensor[i] > threshold[i])
      sensor[i] = 1;
    else
      sensor[i] = 0;
  }
  unsigned char dsensor = (sensor[0] * 1) + (sensor[1] * 2) + (sensor[2] * 4) + (sensor[3] * 8) + (sensor[4] * 16);
  return dsensor;
}
