/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Evaluation
Automatic Program Generator
© Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 12/13/2015
Author  : Freeware, for evaluation and non-commercial use only
Company : 
Comments: 


Chip type               : ATmega16
Program type            : Application
AVR Core Clock frequency: 16.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*****************************************************/


#include <mega16.h>
#include <delay.h>
// Standard Input/Output functions
#include <stdio.h>

//-------------------------  VARIABEL
#define ON    1
#define OFF   0

#define PUMP        PORTA.4

#define AERATOR     PORTA.3
#define COOLER      PORTA.2
#define BuzzerON    PORTC.4=0;
#define BuzzerOFF   PORTC.4=1;

#define tombolUp    PINB.0
#define tombolDown  PINB.1
#define tombolOk    PINB.2

#define ditekan         0
#define tidakDitekan    1

#define inTGSdata    PIND.7
#define inLeveldata  read_adc(6)
 
//variabel regresi
#define regresiA    -18.047
#define regresiB    0.047
float leveldataCm;
// regresi leveldataCm=Ax+B

//-------------------
#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)



// USART Receiver buffer
#define RX_BUFFER_SIZE 8
char rx_buffer[RX_BUFFER_SIZE];

#if RX_BUFFER_SIZE <= 256
unsigned char rx_wr_index,rx_rd_index,rx_counter;
#else
unsigned int rx_wr_index,rx_rd_index,rx_counter;
#endif

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;

// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
char status,data;
status=UCSRA;
data=UDR;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer[rx_wr_index++]=data;
#if RX_BUFFER_SIZE == 256
   // special case for receiver buffer size=256
   if (++rx_counter == 0)
      {
#else
   if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
   if (++rx_counter == RX_BUFFER_SIZE)
      {
      rx_counter=0;
#endif
      rx_buffer_overflow=1;
      }
   }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter==0);
data=rx_buffer[rx_rd_index++];
#if RX_BUFFER_SIZE != 256
if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
#endif
#asm("cli")
--rx_counter;
#asm("sei")
return data;
}
#pragma used-
#endif




// Alphanumeric LCD Module functions
#include <alcd.h>
unsigned char lcdBuff[33];

//------------------------------------------------------//
// FUNGSI-FUNGSI UNTUK DS18B0 V1.1 ATO
// 1.LIHAT KODE ADDRESS SENSOR DS NYA DENGAN FUNGSI CHECK
// 2.GANTI addess sesuai yang tertera
// 3.
//------------------------------------------------------//

// 1 Wire Bus interface functions
#include <1wire.h>
// DS1820 Temperature Sensor functions
#include <ds18b20.h>

//char addressDSWATER[9]={0x28,0xff,0xfC,0x30,0x90,0x15,0x03,0x5E};    
char addressDSWATER[9]={0x28,0xff,0xe6,0x25,0x90,0x15,0x03,0x26};    

char addressDSTGS[9]={0x28,0xDD,0x9a,0x1e,0x00,0x00,0x80,0x3b};


//char addressDSTGS[9]={0x28,0xff,0x19,0x72,0x90,0x15,0x03,0xb1};

float suhuAirTC, suhuAirDC;

/* maximum number of DS18B20 connected to the 1 Wire bus */
#define MAX_DEVICES 8
/* DS18B20 devices ROM code storage area */
unsigned char rom_code[MAX_DEVICES][9];
unsigned char i,j,devices;


//DS18B20_9BIT_RES 0  // 9 bit thermometer resolution
//DS18B20_10BIT_RES 1 // 10 bit thermometer resolution
//DS18B20_11BIT_RES 2 // 11 bit thermometer resolution
//DS18B20_12BIT_RES 3 // 12 bit thermometer resolution
#define DS18B20_TEMPLOWER   20
#define DS18B20_TEMPUPPER   30

void ds18b20Init()
{

    /* detect how many DS18B20 devices
    are connected to the 1 Wire bus */
    devices=w1_search(0xf0,rom_code);
    /* configure each DS18B20 device for 12 bit temperature
   measurement resolution */
    //NOT WORK
    //ds18b20_init(addressDSWATER,DS18B20_TEMPLOWER,DS18B20_TEMPUPPER,DS18B20_12BIT_RES);
    //ds18b20_init(addressDSWATER,DS18B20_TEMPLOWER,DS18B20_TEMPUPPER,DS18B20_12BIT_RES);

}

void ds18b20CheckAddress()
{
    /* display the ROM codes for each device */
if (devices)
   {
   for (i=0;i<devices;i++)
       {
       sprintf(lcdBuff,"#%u is:",i+1);
       lcd_clear();
       lcd_puts(lcdBuff);
       delay_ms(2000);
       lcd_clear();
       for (j=0;j<8;j++)
           {
           sprintf(lcdBuff,"%02X ",rom_code[i][j]);
           lcd_puts(lcdBuff);
           if (j==3) lcd_gotoxy(0,1);
           };
       delay_ms(5000);
       };
   }
else
while (1); /* stop here if no devices were found */
}

void ds18b20GetData()
{
    /* measure and display the temperature(s) */ 
    
     
    suhuAirTC= ds18b20_temperature(addressDSTGS);
    if(suhuAirTC<-10)
    suhuAirTC=0;
     
    suhuAirDC= ds18b20_temperature(addressDSWATER);
    if(suhuAirDC<-10)
    suhuAirDC=0;
    

    
    // Butuh delay
    //delay_ms(1000);
}
//------------------------------------------------------//
//DS18B20 SELESAI
//------------------------------------------------------//


//------------------------------------------------------//
//ADC REGION 
//------------------------------------------------------//

#define ADC_VREF_TYPE 0x00

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

//------------------------------------------------------//
//ADC END REGION
//------------------------------------------------------//



#define BATAS_BAWAH_AIR     5   //464 //5cm
#define BATAS_ATAS_AIR      6   //536 //7cm

//Sensor HC-SR04
#define echo        PINA.6
#define trigger     PORTA.7

#define Timer0_1usSTART     TCCR0=0x01
#define Timer0_1usSTOP      TCCR0=0x00
  
unsigned int jarak;

void ukur_jarak()
{
    unsigned int i;
    jarak=0;
    delay_us(100);
    
    trigger=1;
    delay_us(15);
    trigger=0;
    
    delay_us(100);
    while(!echo);       //progam berulang disini hingga echo==1
//    
//    TCNT0=0xF0;
//    Timer0_1usSTART;
//    while (jarak<5800); //pembatas jarak maksimal           58uS/cm                   
//    
//    Timer0_1usSTOP;
    
                        //jika echo==1 maka hitung panjang pulsa echo
    for (i=0;i<=500;i++)
    {
        if (echo) 
        {
            jarak++;
        }
        delay_us(50);
    }
    
    if(jarak==501)       //pembatas jarak maksimal
    jarak=0;
    
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Reinitialize Timer 0 value
TCNT0=0xF0;
// Place your code here
jarak++;
}

unsigned LvlData;
float masukan=0;
float DeltaError=0;
float PWM;

float and(float a1, float a2)
{
  return(a1<a2?a1:a2);
}

float or(float a1, float a2)
{
  return(a1>a2?a1:a2);
}

float MF(float input, float a, float b, float c)
{
  if(input<a||input>c) return 0;
  else if(input<b) return (input-a)/(b-a);  //menghitung DOM jika nilai i<titik tengah segitiga
  else return (c-input)/(c-b);              //menghitung DOM jika nilai i>titik tengah segitiga
}

float Fuzzy(float e)
{
  /*
  Program Kontroler Logika Fuzzy, EruP(c)2011-2015
  */
  static float LastError=0;
  float de;                  // delta error
  float ePB, ePS, eZ, eNS, eNB;
  float dPB, dPS, dZ, dNS, dNB;
  float oNB, oNM, oNS, oZ, oPS, oPM, oPB;
  float Out;
  
  de = e - LastError;
  LastError = e;
         
//  lcd_gotoxy(0,1);
//  sprintf(lcdBuff,"DE=%.1f",de);
//  lcd_puts(lcdBuff); 
  
  // calculate fuzzy value using fuzzyfication based on membership function
  
  // calculate membership function for error signal
  // float MF(float input, float a, float b, float c)  
  #define pePB 2        // point Error Positive Big
  #define pePS 1        // point Error Positive Small
  #define peZ  0        // point Error Zero
  #define peNS -1       // point Error Negative Small
  #define peNB -2       // point Error Negative Big
  
  ePB=MF(e,pePS,pePB,10);    // Fungsi keanggotaan Positive Big untuk Error
  ePS=MF(e,peZ,pePS,pePB);   // Fungsi keanggotaan Positive Small untuk Error
  eZ =MF(e,peNS,peZ,pePS);   // Fungsi keanggotaan Zero untuk Error
  eNS=MF(e,peNB,peNS,peZ);   // Fungsi keanggotaan Negative Small untuk Error
  eNB=MF(e,-10,peNB,peNS);    // Fungsi keanggotaan Negative Big untuk Error
   
  // calculate membership function for delta error  
   
  #define pdPB 2      // point Delta Error Positive Big
  #define pdPS 1      // point Delta Error Positive Small
  #define pdZ  0      // point Delta Error Zero
  #define pdNS -1      // point Delta Error Negative Small
  #define pdNB -2      // point Delta Error Negative Big 
  
  dPB=MF(de,pdPS,pdPB,30);    // Fungsi keanggotaan Positive Big untuk Delta Error
  dPS=MF(de,pdZ,pdPS,pdPB);   // Fungsi keanggotaan Positive Small untuk Delta Error
  dZ =MF(de,pdNS,pdZ,pdPS);   // Fungsi keanggotaan Zero untuk Delta Error
  dNS=MF(de,pdNB,pdNS,pdZ);   // Fungsi keanggotaan Negative Small untuk Delta Error
  dNB=MF(de,-30,pdNB,pdNS);   // Fungsi keanggotaan Negative Big untuk Delta Error
  
  #define poPB 1000      // point of Output Positive Big tone
  #define poPM 980      // point of Output Positive Medium tone
  #define poPS 950       // point of Output Positive Small tone
  #define poZ 0         // point of Output Zero tone
  #define poNS -950      // point of Output Negative Small tone
  #define poNM -980     // point of Output Negative Medium tone
  #define poNB -1000     // point of Output Negative Big tone
  // do inference engine using IF-THEN rule base
  
  oPB = or(and(ePB,dPB),or(and(ePS,dPB),and(ePB,dPS)));
  oPM = or(and(eZ,dPB),or(and(ePS,dPS),and(ePB,dZ)));
  oPS = or(and(eNS,dPB),or(and(eZ,dPS),or(and(ePS,dZ),and(ePB,dNS))));
  oZ  = or(and(ePB,dNB),or(and(ePS,dNS),or(and(eZ,dZ),or(and(eNS,dPS),and(eNB,dPB)))));
  oNS = or(and(eNB,dPS),or(and(eNS,dZ),or(and(eZ,dNS),and(ePS,dNB))));
  oNM = or(and(eNB,dZ),or(and(eNS,dNS),and(eZ,dNB)));
  oNB = or(and(eNB,dNS),or(and(eNS,dNB),and(eNB,dNB)));
  
  
  // calculate crisp value using defuzzyfication
  Out=(oNB*poNB+oNM*poNM+oNS*poNS+oZ*poZ+oPS*poPS+oPM*poPM+oPB*poPB)/(oNB+oNM+oNS+oZ+oPS+oPM+oPB);
  return Out;
}


void ioInit()
{

PORTA=0x00;
DDRA=0x9F;

PORTB=0xFF;
DDRB=0x00;

PORTC=0x10;
DDRC=0x10;

PORTD=0x00;
DDRD=0xFF;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 16000.000 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
//TCCR0=0x01;
TCNT0=0xF0;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 2000.000 kHz
// Mode: Fast PWM top=ICR1
// OC1A output: Non-Inv.
// OC1B output: Non-Inv.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off

TCCR1A=0xA2;
TCCR1B=0x1A;
TCNT1H=0x00;
TCNT1L=0x00;

//TOP 1000
ICR1H=0x03;
ICR1L=0xE8;

OCR1A=0;
OCR1B=0;


// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=0x00;


// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x01;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=0x00;
UCSRB=0x98;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x67;


// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC Clock frequency: 1000.000 kHz
// ADC Voltage Reference: AREF pin
// ADC Auto Trigger Source: ADC Stopped
ADMUX=ADC_VREF_TYPE & 0xff;
ADCSRA=0x84;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
// TWI disabled
TWCR=0x00;

// 1 Wire Bus initialization
// 1 Wire Data port: PORTA
// 1 Wire Data bit: 0
// Note: 1 Wire port settings must be specified in the
// Project|Configure|C Compiler|Libraries|1 Wire IDE menu.

    w1_init();
    

// Alphanumeric LCD initialization
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 7
// RD - PORTC Bit 6
// EN - PORTC Bit 5
// D4 - PORTC Bit 3
// D5 - PORTC Bit 2
// D6 - PORTC Bit 1
// D7 - PORTC Bit 0
// Characters/line: 16
lcd_init(16);

// Global enable interrupts
#asm("sei")
}


  float SettingPoint=22.5;  //Derajat Celsius
  float ErrorSekarang, DeltaError, ErrorLama=0;
  float Output;
  float OutFuzzy;
  unsigned char iBuzzer=0,flagPUMP=0;;
void Mymain()
{
        ds18b20GetData();  
        
        //suhuAirDC=22.7;
        //suhuAirTC=27.3;
        
        sprintf(lcdBuff,"T%.1f\xdf",suhuAirTC);  
        lcd_gotoxy(0,1);
        lcd_puts(lcdBuff);

        sprintf(lcdBuff,"W%.1f\xdf",suhuAirDC);  
        lcd_gotoxy(0,0);
        lcd_puts(lcdBuff);

        
        ////////FUZZY/////////  
        ErrorSekarang = SettingPoint-suhuAirDC;
        Output=Fuzzy(ErrorSekarang);  
        OutFuzzy=Output*-1;
       
        if (ErrorSekarang<=0.5 && ErrorSekarang>=-0.5 )
        {
            if(iBuzzer<1)
            {
                BuzzerON;
            }
            else
            {
                BuzzerOFF;
            }  
            
            iBuzzer++;
            if(iBuzzer>8)
            iBuzzer=0;
        }
      

            
        PWM=OutFuzzy;
        if(PWM>1000) 
            PWM=1000;
        if(PWM<0)  
            PWM=0;
        OCR1A=PWM;
        OCR1B=PWM;
        
        //OCR1A=1000;
        //OCR1B=1000;

 
        DeltaError=ErrorSekarang-ErrorLama;
        ErrorLama=ErrorSekarang;
   
     lcd_gotoxy(7,0);
     sprintf(lcdBuff,"OC=%3d ", OCR1A);
     lcd_puts(lcdBuff);
 
        
//        lcd_gotoxy(8,0);
//        sprintf(lcdBuff,"OC=%.1f ", PWM);
//        lcd_puts(lcdBuff);  

        
        lcd_gotoxy(10,1);
        sprintf(lcdBuff,"E%.1f",ErrorSekarang);
        lcd_puts(lcdBuff); 
      
 
        
        ////////FUZZY END/////////  
             
//        TGSData=inTGSdata;
//        LvlData=inLeveldata;
//        leveldataCm=regresiA+regresiB*LvlData;
//         
//        
//        sprintf(lcdBuff,"TD=%4d LV=%d",TGSData,LvlData); // String to form the output 
//        lcd_gotoxy(0,0);
//        lcd_puts(lcdBuff);
                

        ukur_jarak();
//        if(jarak==501)
//            jarak=0;
        
        LvlData=14-jarak;
        sprintf (lcdBuff,"L%d ",LvlData);
        lcd_gotoxy (7,1);
        lcd_puts (lcdBuff);
        
        if (LvlData< BATAS_BAWAH_AIR )
        {   
            flagPUMP=1;
            PUMP=ON;
            BuzzerON;
        }
     
        if (LvlData>= BATAS_ATAS_AIR )
        {
            PUMP=OFF;
            if(flagPUMP==1)
                BuzzerOFF;  
            flagPUMP=0;
        }
        
        printf("#_%.1f_%.1f_%.1f_%.1f_%.1f_%d_%d_\n",suhuAirTC,suhuAirDC,SettingPoint,ErrorSekarang,DeltaError,LvlData,OCR1A);                          
        //printf("%.1f_%.1f_%.1f_%.1f_%.1f_%d_",suhuAirTC,suhuAirDC,SettingPoint,ErrorSekarang,DeltaError,LvlData);                          
        
        
        delay_ms(1000);
        
}

void Mymain1()
{
        ds18b20GetData();  
        
        suhuAirDC=22.6;
        suhuAirTC=27.3;
        
        sprintf(lcdBuff,"T%.1f\xdf",suhuAirTC);  
        lcd_gotoxy(0,1);
        lcd_puts(lcdBuff);

        sprintf(lcdBuff,"W%.1f\xdf",suhuAirDC);  
        lcd_gotoxy(0,0);
        lcd_puts(lcdBuff);

        
        ////////FUZZY/////////  
        ErrorSekarang = SettingPoint-suhuAirDC;
        Output=Fuzzy(ErrorSekarang);  
        OutFuzzy=Output*-1;
       
        if (ErrorSekarang<=0.5 && ErrorSekarang>=-0.5 )
        {
            if(iBuzzer<1)
            {
                BuzzerON;
            }
            else
            {
                BuzzerOFF;
            }  
            
            iBuzzer++;
            if(iBuzzer>8)
            iBuzzer=0;
        }
      

            
        PWM=OutFuzzy;
        if(PWM>1000) 
            PWM=1000;
        if(PWM<0)  
            PWM=0;
        OCR1A=PWM;
        OCR1B=PWM;
        
        //OCR1A=1000;
        //OCR1B=1000;

 
        DeltaError=ErrorSekarang-ErrorLama;
        ErrorLama=ErrorSekarang;
   
     lcd_gotoxy(7,0);
     sprintf(lcdBuff,"OC=%3d ", OCR1A);
     lcd_puts(lcdBuff);
 
        
//        lcd_gotoxy(8,0);
//        sprintf(lcdBuff,"OC=%.1f ", PWM);
//        lcd_puts(lcdBuff);  

        
        lcd_gotoxy(10,1);
        sprintf(lcdBuff,"E%.1f",ErrorSekarang);
        lcd_puts(lcdBuff); 
      
 
        
        ////////FUZZY END/////////  
             
//        TGSData=inTGSdata;
//        LvlData=inLeveldata;
//        leveldataCm=regresiA+regresiB*LvlData;
//         
//        
//        sprintf(lcdBuff,"TD=%4d LV=%d",TGSData,LvlData); // String to form the output 
//        lcd_gotoxy(0,0);
//        lcd_puts(lcdBuff);
                

        ukur_jarak();
//        if(jarak==501)
//            jarak=0;
        
        LvlData=14-jarak;
        sprintf (lcdBuff,"L%d ",LvlData);
        lcd_gotoxy (7,1);
        lcd_puts (lcdBuff);
        
        if (LvlData< BATAS_BAWAH_AIR )
        {   
            flagPUMP=1;
            PUMP=ON;
            BuzzerON;
        }
     
        if (LvlData>= BATAS_ATAS_AIR )
        {
            PUMP=OFF;
            if(flagPUMP==1)
                BuzzerOFF;  
            flagPUMP=0;
        }
        
        printf("#_%.1f_%.1f_%.1f_%.1f_%.1f_%d_%d_\n",suhuAirTC,suhuAirDC,SettingPoint,ErrorSekarang,DeltaError,LvlData,OCR1A);                          
        //printf("%.1f_%.1f_%.1f_%.1f_%.1f_%d_",suhuAirTC,suhuAirDC,SettingPoint,ErrorSekarang,DeltaError,LvlData);                          
        
        
        delay_ms(1000);
        
}

void Mymain2()
{
        ds18b20GetData();  
        
        suhuAirDC=22.5;
        suhuAirTC=27.3;
        
        sprintf(lcdBuff,"T%.1f\xdf",suhuAirTC);  
        lcd_gotoxy(0,1);
        lcd_puts(lcdBuff);

        sprintf(lcdBuff,"W%.1f\xdf",suhuAirDC);  
        lcd_gotoxy(0,0);
        lcd_puts(lcdBuff);

        
        ////////FUZZY/////////  
        ErrorSekarang = SettingPoint-suhuAirDC;
        Output=Fuzzy(ErrorSekarang);  
        OutFuzzy=Output*-1;
       
        if (ErrorSekarang<=0.5 && ErrorSekarang>=-0.5 )
        {
            if(iBuzzer<1)
            {
                BuzzerON;
            }
            else
            {
                BuzzerOFF;
            }  
            
            iBuzzer++;
            if(iBuzzer>8)
            iBuzzer=0;
        }
      

            
        PWM=OutFuzzy;
        if(PWM>1000) 
            PWM=1000;
        if(PWM<0)  
            PWM=0;
        OCR1A=PWM;
        OCR1B=PWM;
        
        //OCR1A=1000;
        //OCR1B=1000;

 
        DeltaError=ErrorSekarang-ErrorLama;
        ErrorLama=ErrorSekarang;
   
     lcd_gotoxy(7,0);
     sprintf(lcdBuff,"OC=%3d ", OCR1A);
     lcd_puts(lcdBuff);
 
        
//        lcd_gotoxy(8,0);
//        sprintf(lcdBuff,"OC=%.1f ", PWM);
//        lcd_puts(lcdBuff);  

        
        lcd_gotoxy(10,1);
        sprintf(lcdBuff,"E%.1f",ErrorSekarang);
        lcd_puts(lcdBuff); 
      
 
        
        ////////FUZZY END/////////  
             
//        TGSData=inTGSdata;
//        LvlData=inLeveldata;
//        leveldataCm=regresiA+regresiB*LvlData;
//         
//        
//        sprintf(lcdBuff,"TD=%4d LV=%d",TGSData,LvlData); // String to form the output 
//        lcd_gotoxy(0,0);
//        lcd_puts(lcdBuff);
                

        ukur_jarak();
//        if(jarak==501)
//            jarak=0;
        
        LvlData=14-jarak;
        sprintf (lcdBuff,"L%d ",LvlData);
        lcd_gotoxy (7,1);
        lcd_puts (lcdBuff);
        
        if (LvlData< BATAS_BAWAH_AIR )
        {   
            flagPUMP=1;
            PUMP=ON;
            BuzzerON;
        }
     
        if (LvlData>= BATAS_ATAS_AIR )
        {
            PUMP=OFF;
            if(flagPUMP==1)
                BuzzerOFF;  
            flagPUMP=0;
        }
        
        printf("#_%.1f_%.1f_%.1f_%.1f_%.1f_%d_%d_\n",suhuAirTC,suhuAirDC,SettingPoint,ErrorSekarang,DeltaError,LvlData,OCR1A);                          
        //printf("%.1f_%.1f_%.1f_%.1f_%.1f_%d_",suhuAirTC,suhuAirDC,SettingPoint,ErrorSekarang,DeltaError,LvlData);                          
        
        
        delay_ms(1000);
        
}


void main(void)
{
  
    ioInit();
    PUMP=OFF;
    AERATOR=OFF;
    
//while(1)
//{
//        ukur_jarak();
//        
//        LvlData=jarak;
//        sprintf (lcdBuff,"L%4d",LvlData);
//        lcd_gotoxy (7,1);
//        lcd_puts (lcdBuff);
//
//}
ds18b20Init();

while (1)
      { 
        delay_ms(2000);
        Mymain();
        
        //Mymain1();
        
        //Mymain2();
      
        
//        ds18b20CheckAddress(); 
        
//         ds18b20GetData();  
//        
//        
//        sprintf(lcdBuff,"T%.1f\xdfC ",suhuAirTC);  
//        lcd_gotoxy(8,1);
//        lcd_puts(lcdBuff);
//
//        sprintf(lcdBuff,"W%.1f\xdfC ",suhuAirDC);  
//        lcd_gotoxy(0,0);
//        lcd_puts(lcdBuff);
//        
//        delay_ms(1000);                
      }  
}
