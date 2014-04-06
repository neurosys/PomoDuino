//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
 
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

class PomoTime
{
    private:
    long int start_moment;
    long int time_passed_in_pause;
    long int total_duration;
    long int target_duration;
    long int last_request;
    bool is_paused;
    bool is_started;
    char buf[9];

    public:
    PomoTime();
    void Start();
    void Pause();
    void Resume();
    void SetTargetDuration(byte hours, byte minutes, byte seconds);
    const char* GetTimeStr();
    int GetPercentPassed();
    long int GetPassedTime();
    long int Timestamp();
    bool IsFinished();
    bool IsChanged();
    bool IsStarted();
    int GetSeconds();
    int GetMinutes();
    int GetHours(); 
    void SplitInDigits(byte val, char* buf);
    void Print();
};

PomoTime::PomoTime()
{
    start_moment = 0;
    time_passed_in_pause = 0;
    total_duration = 0;
    is_paused = true;
    last_request = 0;
    is_started = false;
    buf[0] = '0';
    buf[1] = '0';
    buf[2] = ':';
    buf[3] = '0';
    buf[4] = '0';
    buf[5] = ':';
    buf[6] = '0';
    buf[7] = '0';
    buf[8] = '\0';
}

void PomoTime::Start()
{
    start_moment = Timestamp();
    total_duration = 0;
    time_passed_in_pause = 0;
    is_paused = false;
    last_request = 0;
    is_started = true;
}

void PomoTime::Pause()
{
    if (is_paused == false && is_started)
    {
        Serial.print("Pause:\n");
        time_passed_in_pause = Timestamp();
        total_duration += time_passed_in_pause - start_moment;
        is_paused = true;
    }
}

void PomoTime::Resume()
{
    if (is_paused && is_started)
    {
        start_moment = Timestamp();
        is_paused = false;
    }
}

int PomoTime::GetPercentPassed()
{
    return GetPassedTime() / target_duration;
}

long int PomoTime::GetPassedTime()
{
    long int now = Timestamp();
    return now - start_moment + total_duration;
}

long int PomoTime::Timestamp()
{
    return millis() / 1000;
}

void PomoTime::SetTargetDuration(byte hours, byte minutes, byte seconds)
{
    target_duration = hours * 3600 + minutes * 60 + seconds;
}

bool PomoTime::IsFinished()
{
    if (target_duration >= GetPassedTime())
    {
        return true;
    }
    return false;
}

int PomoTime::GetSeconds()
{
    return GetPassedTime() % 60;
}

int PomoTime::GetMinutes()
{
    return (GetPassedTime() / 60) % 60;
}

int PomoTime::GetHours()
{
    return GetPassedTime() / 3600;
}

void PomoTime::SplitInDigits(byte val, char* buf)
{
    buf[1] = val % 10 + '0'; 
    buf[0] = val / 10 + '0';
}

bool PomoTime::IsChanged()
{
    if (last_request == GetPassedTime())
    {
        //Serial.print("IsChanged: Did not change\n");
        return false;
    }
    else
    { 
        //Serial.print("IsChanged::Changed\n");
        last_request = GetPassedTime();
        return true;
    }
}

bool PomoTime::IsStarted()
{
    return is_started;
}

const char* PomoTime::GetTimeStr()
{
    Serial.print("GetTimeStr(): is_started = ");
    Serial.print( is_started);
    Serial.print(" is_paused = ");
    Serial.print( is_paused );
    Serial.print("\n"); 
    if (is_started == false || is_paused == true)
    {
        return buf;
    }
    SplitInDigits(GetHours(), buf); 
    buf[2] = ':';
    SplitInDigits(GetMinutes(), buf + 3); 
    buf[5] = ':';
    SplitInDigits(GetSeconds(), buf + 6); 
    buf[8] = '\0';
    /*
    Serial.print("GetTimeStr(): ");
    Serial.print(buf);
    Serial.print("\n");
    */
    return buf;
}

void PomoTime::Print()
{
    Serial.print(" start_moment = ");
    Serial.print(start_moment);

    Serial.print(" time_passed_in_pause = ");
    Serial.print(time_passed_in_pause);

    Serial.print(" total_duration = ");
    Serial.print(total_duration);

    Serial.print(" target_duration = ");
    Serial.print(target_duration);

    Serial.print(" last_request = ");
    Serial.print(last_request);

    Serial.print(" is_paused = ");
    Serial.print(is_paused);

    Serial.print(" buf = '");
    Serial.print(buf);
    Serial.print("' GetPassedTime() = ");
    Serial.print(GetPassedTime());
    Serial.print("\n");
}
 
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
    adc_key_in = analogRead(0);
    delay(50);
    adc_key_in = analogRead(0);
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

PomoTime timer;

void setup()
{
    lcd.begin(16, 2);              // start the library
    Serial.begin(9600);
    lcd.setCursor(0,1);
    lcd.print("Started"); // print a simple message
}

int counter = 0;
  
void loop()
{
    if (timer.IsStarted() && timer.IsChanged())
    {
        timer.Print();
        lcd.setCursor(8,1);
        lcd.print(timer.GetTimeStr());
        counter++;
        if (counter == 100)
        {
            timer.Pause();
        }
    }
    
    
    lcd_key = read_LCD_buttons();  // read the buttons
    switch(lcd_key)
    {
        case btnSELECT:
        {
            Serial.print("btnSELECT\n");
            if (timer.IsStarted())
            {
                lcd.setCursor(0,0);
                lcd.print("Pauza");
                timer.Pause();
                timer.Print();
            }
        }
        break;
        case btnLEFT:
        {
            Serial.print("btnLeft\n");
            timer.Start();
        }
        break;
        case btnRIGHT:
        {
            Serial.print("btnRight\n");
            if (timer.IsStarted())
            {
                timer.Resume();
            }
        }
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
