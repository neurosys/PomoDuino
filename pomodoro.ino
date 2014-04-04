//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
 
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
 
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
 
// read the buttons
int read_LCD_buttons()
{
    adc_key_in = analogRead(0);      // read the value from the sensor 
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 195)  return btnUP; 
    if (adc_key_in < 380)  return btnDOWN; 
    if (adc_key_in < 555)  return btnLEFT; 
    if (adc_key_in < 790)  return btnSELECT;   
    return btnNONE;  // when all others fail, return this...
}

long int GetSecondsPassed()
{
    return millis() / 1000;
}

long int GetSeconds()
{
    return GetSecondsPassed() % 60;
}

long int GetMinutes()
{
    return (GetSecondsPassed() / 60) % 60;
}

long int GetHours()
{
    return GetSecondsPassed() / 60 / 60;
}

char* GetTime()
{
    static char time_str[9]; // 23:59:59\0
   
    int seconds = GetSeconds();
    time_str[8] = '\0';
    time_str[2] = ':';
    time_str[5] = ':';
    if (seconds < 10)
    {
        time_str[7] = seconds + '0'; 
        time_str[6] = '0';
    }
    else
    { 
        time_str[7] = seconds % 10 + '0';
        time_str[6] = seconds / 10 + '0';
    }
    int minutes = GetMinutes();
    if (minutes < 10)
    {
        time_str[4] = minutes + '0'; 
        time_str[3] = '0';
    }
    else
    { 
        time_str[4] = minutes % 10 + '0';
        time_str[3] = minutes / 10 + '0';
    }
    int hours = GetHours();
    if (hours < 10)
    {
        time_str[1] = hours + '0'; 
        time_str[0] = '0';
    }
    else
    { 
        time_str[1] = hours % 10 + '0';
        time_str[0] = hours / 10 + '0';
    }
    Serial.print(time_str);
    Serial.print("\n");
    return time_str;
}
 
void setup()
{
 lcd.begin(16, 2);              // start the library
 Serial.begin(9600);
 lcd.setCursor(0,1);
 lcd.print("Started"); // print a simple message
}
  
void loop()
{
 lcd.setCursor(8,1);
 static int total_seconds = 0;
 if (total_seconds != GetSecondsPassed())
 {
     lcd.print(GetTime());      // display seconds elapsed since power-up 
 }
 
 
 /*
 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_LCD_buttons();  // read the buttons
 
 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     lcd.print("RIGHT ");
     break;
     }
   case btnLEFT:
     {
     lcd.print("LEFT   ");
     break;
     }
   case btnUP:
     {
     lcd.print("UP    ");
     break;
     }
   case btnDOWN:
     {
     lcd.print("DOWN  ");
     break;
     }
   case btnSELECT:
     {
     lcd.print("SELECT");
     break;
     }
     case btnNONE:
     {
     lcd.print("NONE-x  ");
     break;
     }
 }
 */
 
}
