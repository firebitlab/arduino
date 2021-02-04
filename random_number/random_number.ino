void setup() {
  Serial.begin(9600);
  Serial.println("N INT FLOAT");
  randomSeed(analogRead(0)); // using data from free analog pin to start data [0-1023]
}

int i = 0;
void loop() {
  if (i < 10)
  {
    int random_int = random(0, 1000);
    float random_float = random_int / 1000.0; //must use .0
    Serial.print(i);
    Serial.print(" ");
    Serial.print(random_int);
    Serial.print(" ");
    Serial.print(random_float);
    Serial.println();
    i++;
  }

}
