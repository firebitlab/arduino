//-------------------------------------------SETTING
#define sensitifity_sensor  0.5 //0.1 -- 0.9 high more sensitif depend on light
#define kecepatan  75


//-------------------------------------------MOTOR

#define pPwmL 5
#define pPwmR 6

int speedka, speedki;

void setup()
{

  Serial.begin(115200);
  pinMode(pPwmL, OUTPUT);
  pinMode(pPwmR, OUTPUT);
  motorLR(0, 0);
  hitung_treshold_sensor();

  //DEBUG MOTOR
  //    while(1){
  //      motorLR(70,90);  //lurus beda 20;
  //      }

  //DEBUG SENSOR
  //  while (1) {
  //    baca_garis();
  //    //baca_sensor();
  //    print_sensor();
  //  }



}


void loop()
{
  jalan();
}

char error;
char lastError;
void jalan()
{

  unsigned char dsensor = baca_garis();

  //dsensor = ~dsensor;
  switch (dsensor)
  {
    case 0b01111: error = 4;   break;
    case 0b00111: error = 3;   break;
    case 0b10111: error = 2;   break;
    case 0b10011: error = 1;   break;
    case 0b11011: error = 0;   break;
    case 0b11001: error = -1;   break;
    case 0b11101: error = -2;  break;
    case 0b11100: error = -3;  break;
    case 0b11110: error = -4;  break;

    case 0b01110: error = 10;  break;
    case 0b00100: error = 10;  break;
    case 0b00000: error = 10;  break;
    //    case 0b01111110: error = 10;  break;
    //    case 0b00000000: error = 10;  break;
    //    case 0b10000001: error = 10;  break;
    //    case 0b11000011: error = 10;  break;
    default : error = lastError; break; //mengetahui terakhir kena sensor kanan apa kiri
  }




  float kp = 15;
  float ki = 0.1;
  float kd = 0.2;
  float pid = kp * error + ki * (error + lastError) + kd * (error - lastError);
  unsigned char maxSpeed = 90;
  unsigned char speedL = maxSpeed - 20; //beda kecepatan motor
  unsigned char speedR = maxSpeed;

  if (error > 5)
  {
    speedL = speedL;
    speedR = speedR;
  }
  else
  {
    if (error > 0)
      speedL = maxSpeed - pid;
    else
      speedR = maxSpeed + pid;
  }


  motorLR(speedL, speedR);



  //DEBUG
  Serial.print(pid); Serial.print(";");
  Serial.print(speedL); Serial.print(";");
  Serial.print(speedR); Serial.print(";");
  Serial.print(error, DEC); Serial.print(";");
  Serial.print(lastError, DEC); Serial.print(";");
  Serial.println();

  lastError = error;



}
