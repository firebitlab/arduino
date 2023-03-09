#define pBuzzer   15
#define pLedKonek 2
#define pBatLow  4
int flagBuzzer = 0;


#define pMaju   33
#define pMundur 25
#define pKanan  26
#define pKiri   27

int flagMaju = 0;
int flagMundur = 0;
int flagKanan = 0;
int flagKiri = 0;


#include <Ps3Controller.h>

int player = 0;
int battery = 0;

void notify()
{
  //--- Digital cross/square/triangle/circle button events ---
  if ( Ps3.event.button_down.cross )
  {
    flagMaju = 1;
    Serial.println("Started pressing the cross button");
  }
  if ( Ps3.event.button_up.cross )
  {
    flagMaju = 0;
    Serial.println("Released the cross button");
  }

  if ( Ps3.event.button_down.square )
  {
    if (flagBuzzer == 0)
      flagBuzzer = 1;
    else
      flagBuzzer = 0;
    Serial.println("Started pressing the square button");
  }
  if ( Ps3.event.button_up.square )
  {
    Serial.println("Released the square button");
  }

  //--------------- Digital D-pad button events --------------


  if ( Ps3.event.button_down.right )
  {
    flagKanan = 1;
    Serial.println("Started pressing the right button");
  }
  if ( Ps3.event.button_up.right )
  {
    flagKanan = 0;
    Serial.println("Released the right button");
  }

  if ( Ps3.event.button_down.down )
  {
    flagMundur = 1;
    Serial.println("Started pressing the down button");
  }
  if ( Ps3.event.button_up.down )
  {
    flagMundur = 0;
    Serial.println("Released the down button");
  }

  if ( Ps3.event.button_down.left )
  {
    flagKiri = 1;
    Serial.println("Started pressing the left button");
  }
  if ( Ps3.event.button_up.left )
  {
    flagKiri = 0;
    Serial.println("Released the left button");
  }





  //---------------------- Battery events ---------------------
  if ( battery != Ps3.data.status.battery ) {
    battery = Ps3.data.status.battery;
    Serial.print("The controller battery is ");
    if ( battery == ps3_status_battery_charging )      Serial.println("charging");
    else if ( battery == ps3_status_battery_full )     Serial.println("FULL");
    else if ( battery == ps3_status_battery_high )
    {
      Serial.println("HIGH");
      digitalWrite(pBatLow, HIGH);
    }
    else if ( battery == ps3_status_battery_low)
    {
      Serial.println("LOW");
      digitalWrite(pBatLow, LOW);
    }
    else if ( battery == ps3_status_battery_dying )    Serial.println("DYING");
    else if ( battery == ps3_status_battery_shutdown ) Serial.println("SHUTDOWN");
    else Serial.println("UNDEFINED");
  }



}

void onConnect() {
  Serial.println("Connected.");
  digitalWrite(pLedKonek, HIGH);
}

void setup()
{

  pinMode(pBuzzer, OUTPUT);
  pinMode(pLedKonek, OUTPUT);
  pinMode(pBatLow, OUTPUT);

  pinMode(pMaju, OUTPUT);
  pinMode(pMundur, OUTPUT);
  pinMode(pKanan, OUTPUT);
  pinMode(pKiri, OUTPUT);


  Serial.begin(115200);

  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("01:02:03:04:05:06");

  Serial.println("Ready.");
}

void loop()
{
  if (!Ps3.isConnected())
  {
    digitalWrite(pLedKonek, LOW);
    return;
  }

  digitalWrite(pBuzzer, flagBuzzer);
  digitalWrite(pMaju, flagMaju);
  digitalWrite(pMundur, flagMundur);
  digitalWrite(pKanan, flagKanan);
  digitalWrite(pKiri, flagKiri);

}
