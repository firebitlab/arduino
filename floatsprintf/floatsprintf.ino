/*
   dtostrf(a, b, c, d);
   a = float variable
   b = This is the length of the string that will be created
   c = The number of digits after the deimal point to print
   d = the array to store the results
*/
void setup()
{
  Serial.begin(115200);
}

double d = 111.1111;
float f = 222.2222;
char str_i[10];     //depend on your float lenght character
char data[50];

void loop()
{
  //--------------------------------------------
  sprintf(data, "Can't display %f", f);       //not working on arduino IDE
  Serial.println(data);

  //-------------------------------------------- DOUBLE 10 character and 6 digit after decimal point
  dtostrf(f, 10, 6, str_i );
  sprintf(data, "Can display %s", str_i);
  Serial.println(data);
 
  //-------------------------------------------- FLOAT 10 character and 2 digit after decimal point
  dtostrf(f, 10, 2, str_i );
  sprintf(data, "Can display %s", str_i);
  Serial.println(data);

  //-------------------------------------------- FLOAT 2 digit after decimal point
  dtostrf(f, 3, 2, str_i );
  sprintf(data, "Can display %s", str_i);
  Serial.println(data);

  //-------------------------------------------- DOUBLE 2 digit after decimal point 
  dtostrf(d, 3, 2, str_i );
  sprintf(data, "Can display %s", str_i);
  Serial.println(data);

  while (2); //loop forever

}
