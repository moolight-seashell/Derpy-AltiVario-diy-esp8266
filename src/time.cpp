#include "time.h"

#include <DS3231.h>
#include <Wire.h>


bool century = false;
bool h12Flag;
bool pmFlag;


void set_time(){
    myRTC.setYear(tmp_dates[0]);
    myRTC.setMonth(tmp_dates[1]);
    myRTC.setDate(tmp_dates[2]);
    //myRTC.setDoW(dOW);
    myRTC.setHour(tmp_dates[3]);
    myRTC.setMinute(tmp_dates[4]);
    //myRTC.setSecond(second);
    myRTC.setSecond(0);
}

void get_time(){
    g_month = myRTC.getMonth(century);
    g_date = myRTC.getDate();
    //day of week 
    g_hour = myRTC.getHour(h12Flag, pmFlag); //24h , no am / pm
    g_minute = myRTC.getMinute();
    g_years = myRTC.getYear();
}