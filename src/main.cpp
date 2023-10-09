#include <Arduino.h>
#include <U8g2lib.h>
#include <DS3231.h>
#include <Wire.h>
#include <DHT.h>
#include <SPI.h> //required for build
#include <Adafruit_BMP085.h>
#include <algorithm>
#include <SoftwareSerial.h>



#include "screen.h"
#include "time.h"
#include "eeprom.h"



/*
Cath Copyright Cyrob 2021
Cyrob Arduino Task helper by Philippe Demerliac

See my presentation video in French : https://youtu.be/aGwHYCcQ3Io
See also for v1.3 : https://youtu.be/ph57EpJPs5E

=====================================================================================
==========================   OPEN SOURCE LICENCE   ==================================
=====================================================================================

Copyright 2021 Philippe Demerliac Cyrob.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

................................................................................................................
Release history
................................................................................................................
Version Date        Author    Comment
1.0     30/08/2021  Phildem   First version tested ok
1.1     05/09/2021  Phildem   Misc fixes, better comments and presentation
1.2     06/09/2021  Phildem   Remove unnecessary Cath:: in Cath class definition, (Warning removed)
1.3     08/09/2021  Phildem   Misc comments/name fixes, Memory optimised, __CathOpt_SmallCounter__ option added
1.4     13/09/2021  Soif      Fixes english comments & indentatio
---     18/06/2023  moonlig   Add func to change call time
*/



//____________________________________________________________________________________________________________
// Start of Cath definition__________________________________________________________________________________
#define kMaxCathTask    12         // Max Number of task instances. MUST BE >= to tasks instancied

//#define __CathOpt_SmallCounter__  // Comment this line to allow 32 bit delay. If not, max period is 65536 ms

#ifdef __CathOpt_SmallCounter__
typedef uint16_t CathCnt;
#else
typedef uint32_t CathCnt;
#endif

class Cath{

public:

    // Derived class MUST implement these 2 methods
    virtual void          SetUp() =0;                 // Called at setup
    virtual void          Loop()  =0;                 // Called periodically

    CathCnt               m_CurCounter;               // Curent number of ms before next Loop call
    CathCnt               m_LoopDelay;                // Default period of Loop call (in ms)

    static uint8_t        S_NbTask;                   // Actual number of task instances
    static Cath*          S_CathTasks[kMaxCathTask];  // Array of task object pointers
    static uint8_t        S_LastMilli;                // Used to call every ms (a byte is enought to detect change)

    //..............................................................
    // Must be called in task constructors to register in the task list
    // WARNING : think to set kMaxCathTask as needed
    // Task   : Pointer to the derivated task to register
    // Period : Loop call Period (in ms). WARNING do not pass 0!
    // Offset : Delay of the first call in ms (1 def). WARNING do not pass 0!
    static void S_Register(Cath* Task,CathCnt Period,CathCnt Offset=1){
        Task->m_LoopDelay=Period;
        Task->m_CurCounter= Offset;
        Cath::S_CathTasks[Cath::S_NbTask++]=Task;
    }

    //..............................................................
    // not from original cath library
    // allow to chancge call period using the task index in S_CathTasks
    // with task index n : the n ieme task that have called S_Register
    // task index start with 0
    static void S_change_period(uint8_t task,CathCnt new_period){
        Cath::S_CathTasks[task]->m_LoopDelay = new_period;
    }


    //..............................................................
    // Must be called once in Arduino setup to call all the task setups
    static void S_SetUp(){
        for(int T=0;T<S_NbTask;T++)
            Cath::S_CathTasks[T]->SetUp();
    }

    //..............................................................
    // Must be called once in Arduino Loop to call all the task loop if needed
    static void S_Loop(){
        uint8_t CurMilli=millis();
        if (CurMilli!=S_LastMilli) {
            S_LastMilli=CurMilli;
            for(int T=0;T<S_NbTask;T++) 
                if ( Cath::S_CathTasks[T]->m_CurCounter--==0) {
                    Cath::S_CathTasks[T]->m_CurCounter=Cath::S_CathTasks[T]->m_LoopDelay;
                    Cath::S_CathTasks[T]->Loop();
                }
        }
    }
};

//Cath static variables definitions 
//(Note set to 0 for code clarity but done by default anyway because they are static)
uint8_t       Cath::S_NbTask=0;
Cath*         Cath::S_CathTasks[kMaxCathTask];
uint8_t       Cath::S_LastMilli=0;

// End of Cath definition ___________________________________________________________________________________
//___________________________________________________________________________________________________________




//****************************************************************************************************************
// I/O Abstraction

#define pin_buzzer 15
#define pin_button_left   12
#define pin_button_center   14
#define pin_button_right   16
#define DHTPIN 2
#define DHTTYPE    DHT11 


//****************************************************************************************************************
// Constants

#define buzz_delay_max          700   //mili sec
#define buzz_delay_min          140
#define buzz_delay_stop_glide   300   //ms
#define buzz_tone_high          1500  //buzzer frequency
#define buzz_tone_low           500
#define buzz_vario_max          7

#define graph_min -5
#define graph_max 5

#define main_screen 0
#define alti_graph 1
#define sumary_screen 7
#define settings_menu 2
#define settings_sons_bluetooth 3
#define settings_altitude 4
#define settings_date 5
#define settings_auto_defil 6

#define m_retour 0
#define m_sons_bluetooth 1
#define m_altitude 2
#define m_date 3
#define m_defil_auto 4


#define DHTPIN 2
#define DHTTYPE    DHT11

#define rxBluePin 0//13//13 // Broche 11 en tant que RX, à raccorder sur TX du HC-05
#define txBluePin 13 // Broche 10 en tant que TX, à raccorder sur RX du HC-05


//****************************************************************************************************************

//================================================================================================================
// Globals var

float g_vario = 0.0;      // m/s
float g_vario_lim5 = 0.0;      //g_vario limited to 5 / -5 for gui

float g_altitude;      //m
float g_prev_altitude;
unsigned long prev_alt_ms;
//unsigned long alt_ms;

//settings
bool sound = false;
bool tmp_sound = sound;

bool bluetooth = false;
bool tmp_bluetooth = bluetooth;

bool g_is_autodefil = false;
float g_delay_autodefil = 1.0;

bool tmp_is_autodefil = false;
float tmp_delay_autodefil = 1.0;

int g_batt = 0;

int buzz_delay = 100;
int sound_freq = buzz_tone_low, old_sound_freq = buzz_tone_low;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//siulate altitude section 

float sa_tmp_vario = 0;
int sa_max_vario = 7;
int sa_min_vario = -7;
int sa_max_altitude = 600;
int sa_min_altitude = 500;
bool sa_up = true;
unsigned long sa_prev_ms;

//history
float g_vario_history_5[6] = {0,0,0,0,0,0};

//menu and screen
int current_menu = 0;

int menu_selected = 0;
int sub_menu_selected = 0;

DHT dht(DHTPIN, DHTTYPE);
int g_temperature;
int g_humidity;



DS3231 myRTC;

byte g_month;
byte g_date;
byte g_hour;
byte g_minute;
byte g_years;

int tmp_dates [5];

int g_altitude_history[100];
int g_altitude_hmax;
int g_altitude_hmin;

Adafruit_BMP085 bmp;
int g_current_sea_pressure_level = 102000;

EspSoftwareSerial::UART myBlueSerial(rxBluePin, txBluePin);

int g_total_fly_hour;
int g_total_fly_min;
byte g_last_minute;
int g_total_alti;
float g_total_vario_max;
float g_total_vario_min;


//================================================================================================================
// Globals func

// equivalent to map() with floats
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  float ret = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

    //check to not output out of ranges
    if(out_min < out_max){
        if(ret > out_max){
            return out_max;
        } else if(ret < out_min){
            return out_min;
        }
    } else {
        if(ret < out_max){
            return out_max;
        } else if(ret > out_min){
            return out_min;
        }
    }
    return ret;
}


void set_sound(int new_sound){
    old_sound_freq = sound_freq;
    sound_freq = new_sound;
}


void set_g_vario_lim5(){
    g_vario_lim5 = g_vario;

    if(g_vario_lim5 > 5){
        g_vario_lim5 = 5;
    }
    if(g_vario_lim5 < -5){
        g_vario_lim5 = -5;
    }
}

// little music played at the boot
void boot_tone(){
    tone(pin_buzzer,369,200);
    delay(198);
    tone(pin_buzzer,440,200);
    delay(198);
    tone(pin_buzzer,369,200);
    delay(198);
    tone(pin_buzzer,329,200);
    delay(198);
    tone(pin_buzzer,440,200);
    delay(1000);
}




//================================================================================================================

//..........................................................................................
// Tasks 
class Buzz: public Cath{

public:

    Buzz(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
    }

    void Loop(){
        if(!sound){
            return;
        }
        if(g_vario < 0){
            old_sound_freq = sound_freq;
            return;
        }
        
        set_sound(  mapfloat(g_vario,0,buzz_vario_max,buzz_tone_low,buzz_tone_high)  );

        if(sound_freq == old_sound_freq or buzz_delay < buzz_delay_stop_glide){
            tone(pin_buzzer,sound_freq,90);
        } else {
            int tmp_sound;
            int len = map(buzz_delay,buzz_delay_min,buzz_delay_stop_glide,10,20);
            for(int i = 0; i < len ; i ++){
                tmp_sound = map(i,0,len,old_sound_freq,sound_freq);
                tone(pin_buzzer,tmp_sound,12);
                delay(10);
        }
        }
    }
};


//..........................................................................................
class AltiSimu: public Cath{

    public:

    AltiSimu(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
        sa_prev_ms = millis();

        g_altitude = 450;

        g_prev_altitude = g_altitude;
        prev_alt_ms =  millis();
    }

    void Loop(){
        //non linear altitude variation
        float var = (sa_tmp_vario * 0.001) * (millis() - sa_prev_ms);
        sa_prev_ms = millis();

        if(sa_up){
            if(sa_tmp_vario < sa_max_vario){
                sa_tmp_vario += 0.1;
            }
            g_altitude += var;
        } else {
            if(sa_tmp_vario > sa_min_vario){
                sa_tmp_vario -= 0.1;
            }
            g_altitude += var;
        }
        
        if(g_altitude > sa_max_altitude){
            sa_up = false;
        }
        if(g_altitude < sa_min_altitude){
            sa_up = true;
        }

        //recompute vario from altitude and milis
        

        // gvarion = (delta alti) / (delta milis * 0.001)
        g_vario = (g_altitude-g_prev_altitude) / ((millis() - prev_alt_ms) * 0.001);

        set_g_vario_lim5();
        buzz_delay = mapfloat(g_vario,0,5,buzz_delay_max,buzz_delay_min);
        Cath::S_change_period(0,buzz_delay);
        g_prev_altitude = g_altitude;
        prev_alt_ms =  millis();
    }
};

//..........................................................................................
class Alti: public Cath{

    public:

    Alti(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){

        bmp.begin(BMP085_ULTRAHIGHRES);
        sa_prev_ms = millis();

        float tmp_alti = 0;
        for(int i = 0; i < 9 ; i ++){
            tmp_alti += bmp.readAltitude(g_current_sea_pressure_level);
            //delay(10);
        }
        g_altitude = tmp_alti/9;

        g_prev_altitude = g_altitude;
        prev_alt_ms =  millis();
    }

    void Loop(){
        
        float tmp_alti = 0;
        for(int i = 0; i < 9 ; i ++){
            tmp_alti += bmp.readAltitude(g_current_sea_pressure_level);
            //delay(10);
        }
        g_altitude = tmp_alti/9;

        //compute vario from altitude and milis
        if(g_altitude > g_total_alti){
            g_total_alti = g_altitude;
            save_to_eeprom_alti();
        }
        

        // gvarion = (delta alti) / (delta milis * 0.001)
        g_vario = (g_altitude-g_prev_altitude) / ((millis() - prev_alt_ms) * 0.001);

        if(g_vario > g_total_vario_max){
            g_total_vario_max = g_vario;
            save_to_eeprom_var_max();
        }
        if(g_vario < -g_total_vario_min){
            g_total_vario_min = -g_vario;
            save_to_eeprom_var_min();
        }

        set_g_vario_lim5();
        buzz_delay = mapfloat(g_vario,0,5,buzz_delay_max,buzz_delay_min);
        Cath::S_change_period(0,buzz_delay);
        g_prev_altitude = g_altitude;
        prev_alt_ms =  millis();
    }
};


//history
//float g_vario_history_5[6] = {0,0,0,0,0,0};
//..........................................................................................
class HistoVario: public Cath{

    public:

    HistoVario(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
    }

    void Loop(){
        for(int i = 4; i >= 0; i --){
            g_vario_history_5[i+1] = g_vario_history_5[i];
        }
        g_vario_history_5[0] = g_vario_lim5;
    }
};

//..........................................................................................
class HistoAlti: public Cath{

    public:

    HistoAlti(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
    }

    void Loop(){
        for(int i = 98; i >= 0; i --){
            g_altitude_history[i+1] = g_altitude_history[i];
        }
        g_altitude_history[0] = g_altitude;

        g_altitude_hmax = g_altitude;
        g_altitude_hmin = g_altitude;

        for(int i = 0 ; i < 100; i ++){
            if(g_altitude_history[i] > g_altitude_hmax and g_altitude_history[i] != 0){
                g_altitude_hmax = g_altitude_history[i];
            }
            if(g_altitude_history[i] < g_altitude_hmin and g_altitude_history[i] != 0){
                g_altitude_hmin = g_altitude_history[i];
            }
        }

    }
};
//..........................................................................................
//===== button interrupt section
class Buttons: public Cath{

    public:

    Buttons(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
          // put your setup code here, to run once:

    }

    void Loop(){
        if(!digitalRead(pin_button_left)){
            switch(current_menu){
                case settings_menu:
                    menu_selected -= 1;
                    if(menu_selected < 0){
                        menu_selected = 4;
                    }
                    break;
                case settings_sons_bluetooth:
                    if(sub_menu_selected == 0){
                        tmp_sound = true;
                    } else {
                        tmp_bluetooth = true;
                    }
                case settings_date:
                    tmp_dates[sub_menu_selected] -= 1;
                    if(sub_menu_selected == 1){
                        tmp_dates[1] = tmp_dates[1] % 13;
                    }
                    if(sub_menu_selected == 2){
                        tmp_dates[2] = tmp_dates[1] % 32;
                    }
                    if(sub_menu_selected == 3){
                        tmp_dates[3] = tmp_dates[1] % 24;
                    }
                    if(sub_menu_selected == 4){
                        tmp_dates[4] = tmp_dates[1] % 60;
                    }
                    break;
                    break;
                case main_screen:
                    current_menu = sumary_screen;
                    break;
                case alti_graph:
                    current_menu = main_screen;
                    break;
                case sumary_screen:
                    current_menu = alti_graph;
                    break;
                case settings_auto_defil:
                    if(sub_menu_selected == 0){
                        tmp_is_autodefil = !tmp_is_autodefil ;
                    } else {
                        tmp_delay_autodefil -= 0.1;
                    }
                    break;
                case settings_altitude:
                    g_current_sea_pressure_level -= 100;
                    break;
            }
            
        }
        if(!digitalRead(pin_button_center)){
            switch(current_menu){
                case main_screen:
                case alti_graph:
                case sumary_screen:
                    current_menu = 2;
                    break;
                case settings_menu:
                    switch(menu_selected){
                        case m_retour:
                            current_menu = main_screen;
                            break;
                        case m_sons_bluetooth:
                            current_menu = settings_sons_bluetooth;
                            tmp_bluetooth = bluetooth;
                            tmp_sound = sound;
                            break;
                        case m_altitude:
                            current_menu = settings_altitude;
                            break;
                        case m_date:
                            current_menu = settings_date;
                            tmp_dates[0] = g_years;
                            tmp_dates[1] = g_month;
                            tmp_dates[2] = g_date;
                            tmp_dates[3] = g_hour;
                            tmp_dates[4] = g_minute;
                            break;
                        case m_defil_auto:
                            current_menu = settings_auto_defil;
                            tmp_delay_autodefil = g_delay_autodefil;
                            tmp_is_autodefil = g_is_autodefil;
                            break;
                    }
                    break;
                case settings_sons_bluetooth:
                    if(sub_menu_selected == 0){
                        sub_menu_selected = 1;
                    } else {
                        current_menu = settings_menu;
                        sound = tmp_sound;
                        bluetooth = tmp_bluetooth;
                        sub_menu_selected = 0;
                        save_to_eeprom_sound_blu();
                    }
                    //todo save
                    break;
                case settings_date:
                    sub_menu_selected += 1;
                    if(sub_menu_selected == 5){
                        current_menu = settings_menu;
                        set_time();
                        //get time ?
                        sub_menu_selected = 0;
                    }
                    break;
                case settings_auto_defil:
                    if(sub_menu_selected == 0){
                        sub_menu_selected = 1;
                    } else {
                        current_menu = settings_menu;
                        g_is_autodefil = tmp_is_autodefil;
                        g_delay_autodefil = tmp_delay_autodefil;
                        Cath::S_change_period(7,(int)(float)(g_delay_autodefil * 1000));
                        sub_menu_selected = 0;
                        save_to_eeprom_autodefil();
                    }
                    break;
                case settings_altitude:
                    current_menu = settings_menu;
                    save_to_eeprom_sealevel();
                    break;
            }
        }
        if(!digitalRead(pin_button_right)){
            switch(current_menu){
                case settings_menu:
                    menu_selected += 1;
                    menu_selected = menu_selected % 5;
                    break;
                case settings_sons_bluetooth:
                    if(sub_menu_selected == 0){
                        tmp_sound = false;
                    } else {
                        tmp_bluetooth = false;
                    }
                    break;
                case settings_date:
                    tmp_dates[sub_menu_selected] += 1;
                    if(sub_menu_selected == 1){
                        tmp_dates[1] = tmp_dates[1] % 13;
                    }
                    if(sub_menu_selected == 2){
                        tmp_dates[2] = tmp_dates[1] % 32;
                    }
                    if(sub_menu_selected == 3){
                        tmp_dates[3] = tmp_dates[1] % 24;
                    }
                    if(sub_menu_selected == 4){
                        tmp_dates[4] = tmp_dates[1] % 60;
                    }
                    break;
                case sumary_screen:
                    current_menu = main_screen;
                    break;
                case main_screen:
                    current_menu = alti_graph;
                    break;
                case alti_graph:
                    current_menu = sumary_screen;
                    break;
                case settings_auto_defil:
                    if(sub_menu_selected == 0){
                        tmp_is_autodefil = !tmp_is_autodefil ;
                    } else {
                        tmp_delay_autodefil += 0.1;
                    }
                    break;
                case settings_altitude:
                    g_current_sea_pressure_level += 100;
                    break;
            }
        }
    }
};


//..........................................................................................
class Clock: public Cath{

public:
    Clock(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
        get_time();
        g_last_minute = g_minute;
    }

    void Loop(){
        get_time();
    }
};


//..........................................................................................
class TempHum: public Cath{

public:
    TempHum(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
        dht.begin();
    }

    void Loop(){
        g_temperature = int(dht.readTemperature());
        g_humidity = int(dht.readHumidity());
    }
};

//..........................................................................................
class Gui: public Cath{

    public:

    Gui(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
        
    }

    void Loop(){
        draw_main_screen();
    }

};


//..........................................................................................
class Autodefil: public Cath{

    public:

    Autodefil(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
    }

    void Loop(){
        if(!g_is_autodefil){
            return;
        }
        if(current_menu == main_screen){
            current_menu = alti_graph;
        } else if(current_menu == alti_graph){
            current_menu = main_screen;
        }
    }
};

//..........................................................................................
class Battery: public Cath{

    public:

    Battery(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
    }

    void Loop(){
        int reading = analogRead(A0);
        float tmp_batt = mapfloat(reading,0,1024,0,5) - 0.1;
        g_batt = int(mapfloat(tmp_batt,3.8,5,0,100));
    }

};

//..........................................................................................
char     altitude_arr[6];            //wee need this array to translate float to string
char     vario_arr[5]; 
class Bluetooth: public Cath{

    public:

    Bluetooth(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
        pinMode(rxBluePin, INPUT);
        pinMode(txBluePin, OUTPUT);
        myBlueSerial.begin(9600);
    }

    void Loop(){
        if(!bluetooth){
            return;
        }
              //creating now NMEA serial output for LK8000. LK8EX1 protocol format:
        //$LK8EX1,pressure,altitude,vario,temperature,battery,*checksum
        String str_out = String("LK8EX1" + String(",999999") + String(",") + String(dtostrf(g_altitude, 0, 0, altitude_arr)) + String(",") +
                    String(dtostrf((g_vario * 100), 0, 0, vario_arr)) + String(",") + String(g_temperature, DEC) + String(",") + String(g_batt + 1000, DEC) + String(","));

        unsigned int checksum_end, ai, bi;                                               // Calculating checksum for data string
        for (checksum_end = 0, ai = 0; ai < str_out.length(); ai++)
        {
        bi = (unsigned char)str_out[ai];
        checksum_end ^= bi;
        }
        myBlueSerial.print("$");                     //print first sign of NMEA protocol
        myBlueSerial.print(str_out);                 // print data string
        myBlueSerial.print("*");                     //end of protocol string
        myBlueSerial.println(checksum_end, HEX);     //print calculated checksum on the end of the string in HEX
        
    }
};

//..........................................................................................
class TotalTime: public Cath{

    public:

    TotalTime(unsigned long Period,unsigned long Offset=1){
        Cath::S_Register(this,Period,Offset);
    }

    void SetUp(){
    }

    void Add(int delay){
        g_total_fly_min += delay;
        g_last_minute = g_minute;
        if(g_total_fly_min > 60){
            g_total_fly_hour += 1;
            g_total_fly_min -= 60;
        }
        save_to_eeprom_flytime();
    }

    void Loop(){
        Serial.print("curr : ");
        Serial.print(int(g_minute));
        Serial.print(" last : ");
        Serial.println(int(g_last_minute));

        if(g_last_minute > g_minute){
            if(( g_minute + 60 ) - g_last_minute >= 10){
                Add(( g_minute + 60 ) - g_last_minute);
            }
        } else {
            if(g_minute  - g_last_minute >= 10){
                Add(g_minute  - g_last_minute);
            }
        }
    }
};


// ****************************************************************************************************************
// Global tasks instanciation

Buzz            MainBuzz(buzz_delay,50);
Gui             gui(200,100);
Alti            AV(600,5);
//AltiSimu        AS(200,5);
HistoVario      HV(5000,150);
Buttons         Bt(190,200);
Clock           Clk(1000,75);
TempHum         TH(2000,80);
Autodefil       AF(3000,3000);
HistoAlti       HA(5000,25);
Battery         BT(5000,300);
Bluetooth       blu(1000,400);
TotalTime       tt(30000,30000);//very inacurate 30s (1min)
//DebugPrint dp(500,150);




//-----------------------------------------------------------------------------------------------------------------
void setup() {
  // Start the I2C interface
  //Wire.begin();
  //get_time(); // prevent time reset
    pinMode(pin_button_left, INPUT);
    pinMode(pin_button_center, INPUT);
    pinMode(pin_button_right, INPUT);

    Serial.begin(9600);

    memset(g_altitude_history, 0, sizeof(g_altitude_history));
    retore_eeprom_settings();
    Cath::S_change_period(7,(int)(float)(g_delay_autodefil * 1000));

    Wire.begin();

    u8g2.begin();
    u8g2.setColorIndex(1);
    draw_start_screen();
    boot_tone();        // boot melody
    draw_main_screen();
    Cath::S_SetUp();    // Just ask Cath to call the task's setup
}

//-----------------------------------------------------------------------------------------------------------------
void loop() {
    Cath::S_Loop();    // Just ask Cath to call the task's loop
}