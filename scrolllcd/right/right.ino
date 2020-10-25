/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // DEPEND ON YOUR CIRCUIT

void setup() 
{  
  lcd.begin(16, 2);
  
  lcd.setCursor(0,0);
//lcd.print("0123456789012345");
  lcd.print("D4 LJ ELIN PENS"); // TEXT WANT TO SCROLL
  delay(2000); //WAIT 2 SECOND OPTIONAL
}

void loop() 
{  
  for (int i = 0; i < (16+10+13); i++)  // 16 + 10 + 15 CHANGE UP TO YOU AND YOU WILL FIND WHY I AM USING THIS NUMBER
  {
    lcd.scrollDisplayRight(); 
    delay(200);  // DELAY EVERY MOVE
  }
  while(1){} //DEBUG / FOR DEBUG HOW MANY SCROLL HAPPEN WITH THAT NUMBER
    
  lcd.clear(); //to reset scroll cursor 
  lcd.setCursor(0,0); // FOR CONTINUE SCROLL NEDD TO SET CURSOR TO START AGAIN 0.0
  lcd.print("D4 LJ ELIN PENS"); // SAME TEXT / LETS TRY
  
}