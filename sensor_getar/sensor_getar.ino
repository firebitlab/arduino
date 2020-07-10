#define p_sensor 19

void setup()
{
  Serial.begin(9600);
  pinMode(p_sensor, INPUT);
}

void loop()
{
  unsigned char d_sensor = digitalRead(p_sensor);
  if (d_sensor == HIGH)
  {
    Serial.println(d_sensor);

  }
}
