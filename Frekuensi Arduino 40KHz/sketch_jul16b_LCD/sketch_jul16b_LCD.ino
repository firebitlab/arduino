#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,20,4);


int hitung_icr, frekuensi, duty_cycle, dataf ;
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(11, OUTPUT);
  // Timer/Counter 1 initialization
  TCCR1A = 0xA2;
  TCCR1B = 0x19;
  TCNT1H = 0x00;
  TCNT1L = 0x00;

  duty_cycle = 60; //0-100
  hitung_icr = 399; //lihat tabel
  frekuensi = 16000 / (1 + hitung_icr);

  Serial.print("D(%)   = "); Serial.println(duty_cycle);
  Serial.print("ICR (D)= "); Serial.println(hitung_icr);
  Serial.print("F(kHz) = "); Serial.println(frekuensi);

  lcd.setCursor(0,0);
  lcd.print("Function Generator");
  lcd.setCursor(0,1);
  lcd.print("D = "); lcd.print(duty_cycle);  lcd.print(" % ");
  lcd.setCursor(0,2);
  lcd.print("F = "); lcd.print(frekuensi); lcd.print(" kHz ");
  lcd.setCursor(0,3);
  lcd.print("P = 11"); 
  
  

  ICR1H = hitung_icr >> 8;
  ICR1L = hitung_icr & 0x00ff;
  // Register atur Duty Clycle 
  OCR1A = hitung_icr * (duty_cycle / 100.0);

}

void loop() {
  // put your main code here, to run repeatedly:

}
