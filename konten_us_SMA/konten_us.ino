#define p_trigPin_cadangan  15
#define p_echoPin_cadangan  4

void us_setup()
{
  pinMode(p_trigPin_cadangan, OUTPUT);
  pinMode(p_echoPin_cadangan, INPUT);
}
float us_get_jarak(int triger, int echo)
{
  long duration;
  float distance;
  int i;

  digitalWrite(triger, LOW);
  delayMicroseconds(2);
  digitalWrite(triger, HIGH);
  delayMicroseconds(10);
  digitalWrite(triger, LOW);
  duration =  pulseIn(echo, HIGH,10000); //uS

  distance = duration * 0.034 / 2;

  return distance;
}

//--------------------------------#MY AVERAGE
#define n_my_data 4
float my_data[n_my_data];
int i_my_data = 0;

float my_moving_average(float in_data, int debug)
{
  float my_ave;
  float my_sum;

  my_data[i_my_data] = in_data;

  if (my_data[n_my_data - 1] == 0)
  {
    my_sum = 0;

    for (int i = 0; i <= i_my_data; i++)
    {
      my_sum += my_data[i];
    }
    my_ave = my_sum / (i_my_data + 1);
  }
  else
  {
    my_sum = 0;
    for (int i = 0; i < n_my_data; i++)
    {
      my_sum += my_data[i];
    }
    my_ave = my_sum / n_my_data;
  }


  //#Debug
  if (debug)
  {
    for (int i = 0; i < n_my_data; i++)
    {
      Serial.print(i); Serial.print(" "); Serial.println(my_data[i]);
    }
    Serial.print("i"); Serial.print(" "); Serial.println(i_my_data);
    Serial.print("Last"); Serial.print(" "); Serial.println(my_data[n_my_data - 1]);

    Serial.print("Sum"); Serial.print(" "); Serial.println(my_sum);
    Serial.print("Ave"); Serial.print(" "); Serial.println(my_ave);
    Serial.println();
  }



  i_my_data++;
  if (i_my_data >= n_my_data)
    i_my_data = 0;

  return my_ave;
}

void my_moving_ave_clear()
{
  //Make sure All Array is 0
  for (int i = 0; i < n_my_data; i++)
  {
    my_data[i] = 0;
  }
}



void setup() {
  Serial.begin(9600);
  us_setup();
  my_moving_ave_clear();
}

void loop()
{
  
  float distance_cadangan = us_get_jarak(p_trigPin_cadangan, p_echoPin_cadangan);
  float average_cadangan = my_moving_average(distance_cadangan, 0);
  
  Serial.print(distance_cadangan); Serial.print(" ");
  Serial.print(average_cadangan); 
  
  Serial.println();
  delay(1000);
}
