#define piLOplus_LAYellow 32 //LO+  RX2 Konek 0 DC 1
#define piLOmin_RARed 33  //LO-  TX2  Konek 0 DC 1
#define paOutput 36  //LO-  TX2
#define poLEDtick LED_BUILTIN
unsigned char dLOplus_LAYellow;
unsigned char dLOmin_RARed;
int dOutput = 0;

#define tresholdSinyal 3000


void setup()
{
  Serial.begin(115200);
  pinMode(piLOplus_LAYellow, INPUT_PULLUP);
  pinMode(piLOmin_RARed, INPUT_PULLUP);
  pinMode(poLEDtick, OUTPUT);

  my_moving_ave_clear();
}


int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1   ; //1000Hz
float BPM = 0;
int mavBPM = 0;
int error = 0; //untuk kalibrasi
int msBPM;

int flagBPM = 0;
void loop()
{

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    dLOmin_RARed = digitalRead(piLOmin_RARed);
    dLOplus_LAYellow = digitalRead(piLOplus_LAYellow);
    dOutput = analogRead(paOutput);

    //dataLOPrint();
    //dataSignalPrint();


    // LEAD TIDAK TERHUBUNG
    // LED NYALA
    if (dLOmin_RARed == HIGH || dLOplus_LAYellow == HIGH) {
      digitalWrite(poLEDtick, HIGH); //NYALA
      msBPM = 0;
    }


    // LEAD TERHUBUNG
    // LED KEDIP
    else {


      if (dOutput > tresholdSinyal && flagBPM == 0) {

        flagBPM = 1;
        digitalWrite(poLEDtick, HIGH);


        if (msBPM > 525)
        {
          BPM = 60000.0 / msBPM;
          mavBPM = my_moving_average(BPM, 0);
          mavBPM -= error;
          //Serial.print(BPM); Serial.print(" ");
          Serial.println(mavBPM);
        }



        msBPM = 0;

      }
      else if (dOutput < tresholdSinyal) {
        flagBPM = 0;
        digitalWrite(poLEDtick, LOW);
        msBPM++;
      }
    }
  }
}

void dataPrint() {
  //Serial.print(dLOplus);    Serial.print("+");
  //Serial.print(dLOmin);  Serial.print("-");
  Serial.print(dOutput);
  Serial.println();
}




void dataLOPrint() {
  Serial.print(dLOplus_LAYellow);    Serial.print("LA");
  Serial.print(dLOmin_RARed);  Serial.print("RA");
  Serial.println();
}

void dataSignalPrint() {
  Serial.print(dOutput);
  Serial.println();
}



//--------------------------------#MY AVERAGE
#define n_my_data 5
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
