void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Data_1"); Serial.print(" ");
  Serial.print("Data_2"); Serial.print(" ");
  
  Serial.println();

}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print(random(5)); Serial.print(" ");
  Serial.print(random(5)); Serial.print(" ");
 
  Serial.println();

  delay(100);

}
