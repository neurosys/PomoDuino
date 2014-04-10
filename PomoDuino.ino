#include <LiquidCrystal.h>
 
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/*
Bugs:
    Clock stops at xx:xx:59
    Pause doesn't work anymore
    
*/

class PomoTime
{
    private:
    enum
    { 
        stopped,
        started,
        paused,
    }status;
    long int start_moment;
    long int time_passed_in_pause;
    long int total_duration;
    long int target_duration;
    long int last_request;
    char buf[9];

    public:
    PomoTime(int hours = 0, int mins = 25, int secs = 0);
    void Start();
    void Pause();
    void Resume();
    void Stop();
    void SetTargetDuration(byte hours, byte minutes, byte seconds);
    const char* GetTimeStr();
    int GetPercentPassed();
    long int GetPassedTime();
    long int Timestamp();
    bool IsFinished();
    bool IsChanged();
    bool IsStarted();
    bool IsPaused();
    bool IsStopped();
    int GetSeconds();
    int GetMinutes();
    int GetHours(); 
    char* GetStateStr();
    void SplitInDigits(byte val, char* buf);
    void Print();
};

PomoTime::PomoTime(int hours, int mins, int secs)
{
    start_moment = 0;
    time_passed_in_pause = 0;
    total_duration = 0;
    last_request = 0;
    status = stopped;
    SetTargetDuration(hours, mins, secs);
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
    last_request = 0;
    status = started;
}

void PomoTime::Pause()
{
    Serial.print("Pause:\n");
    if (IsStarted() && !IsPaused())
    {
        Serial.print("Pause:\n");
        time_passed_in_pause = Timestamp();
        total_duration += time_passed_in_pause - start_moment;
        status = paused;
    }
}

bool PomoTime::IsStopped()
{
    return (status == stopped);
}

char* PomoTime::GetStateStr()
{
    switch (status)
    {
        case stopped:
            return "Stopped";
        case started:
            return "Started";
        case paused:
            return "Paused ";
    }
}

bool PomoTime::IsPaused()
{
    return (status == paused);
    //return is_paused;
}

void PomoTime::Resume()
{
    if (IsPaused() && IsStarted())
    {
        start_moment = Timestamp();
        //is_paused = false;
        status = started;
    }
}

void PomoTime::Stop()
{
    status = stopped;
}

int PomoTime::GetPercentPassed()
{
    return (GetPassedTime() * 100 / target_duration);
}

long int PomoTime::GetPassedTime()
{
    long int now = Timestamp() - start_moment + total_duration;
    if (now >= target_duration)
    {
        //Pause();
        Stop();
    }
    return now;
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
    return (status > stopped);
    //return is_started;
}

const char* PomoTime::GetTimeStr()
{
    Serial.print("GetTimeStr(): is_started = ");
    Serial.print( IsStarted());
    Serial.print(" IsPaused() = ");
    Serial.print( IsPaused() );
    Serial.print("\n"); 
    if (!IsStarted() || IsPaused())
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

    Serial.print(" percent_passed = ");
    Serial.print(GetPercentPassed());

    Serial.print(" last_request = ");
    Serial.print(last_request);

    Serial.print(" IsPaused() = ");
    Serial.print(IsPaused());

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


void PrintProgressBar(int done)
{
    if (!timer.IsStarted() || timer.IsPaused())
    {
        return;
    }
    int percent_done = done / 10;
    static int old_percent = 0;
    if (old_percent != percent_done)
    {
        old_percent = percent_done;
        lcd.setCursor(5 + percent_done, 0);
        lcd.print('#');
    }
}


void PrintPercentCounter(int done)
{
    int pos = 0;
    if (!timer.IsStarted() || timer.IsPaused())
    {
        return;
    }
    if (done == 100)
    {
        pos = 0;
    }
    else if (done >= 10)
    {
        pos = 1; 
    }
    else
    {
        pos = 2;
    }
    lcd.setCursor(pos,0);
    lcd.print(done);
    lcd.print('%');
}


void setup()
{
    lcd.begin(16, 2);              // start the library
    Serial.begin(9600);
    timer.SetTargetDuration(0, 1, 0);
}

void PrintStatus()
{
    lcd.setCursor(0, 1);
    lcd.print(timer.GetStateStr());
}

  
void loop()
{
    if (timer.IsStarted() && timer.IsChanged())
    {
        PrintPercentCounter(timer.GetPercentPassed());
        PrintProgressBar(timer.GetPercentPassed());
        //timer.Print();
        lcd.setCursor(8,1);
        lcd.print(timer.GetTimeStr());
    }
    
    
    
    lcd_key = read_LCD_buttons();  // read the buttons
    switch(lcd_key)
    {
        case btnSELECT:
        {
            Serial.print("btnSELECT\n");
            if (!timer.IsPaused())
            {
                timer.Pause();
                //timer.Print();
                PrintStatus();
            }
        }
        break;
        case btnLEFT:
        {
            //Serial.print("btnLeft\n");
            timer.Start();
            PrintStatus();
        }
        break;
        case btnRIGHT:
        {
            //Serial.print("btnRight\n");
            if (timer.IsStarted())
            {
                timer.Resume();
                PrintStatus();
            }
        }
    }
}
