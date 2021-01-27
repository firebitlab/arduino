void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  //Header Column, separator data using space

  Serial.print("Data_1"); Serial.print(" ");
  Serial.print("Data_2"); Serial.print(" ");
  Serial.print("Data_3"); Serial.print(" ");
  Serial.print("Data_4"); Serial.print(" ");
  Serial.print("Data_5"); Serial.print(" ");
  Serial.println();

}

void loop() {
  // put your main code here, to run repeatedly:

   Serial.print(random(5)); Serial.print(" ");
  Serial.print(random(10)); Serial.print(" ");
  Serial.print(random(12)); Serial.print(" ");
  Serial.print(random(20)); Serial.print(" ");
  Serial.print(random(30)); Serial.print(" ");
  Serial.println();

  delay(1000);

}
