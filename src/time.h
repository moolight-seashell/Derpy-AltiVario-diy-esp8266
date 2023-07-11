#include <Arduino.h>
#include <U8g2lib.h>
#include <DS3231.h>
#include <Wire.h>

class DS3231;

extern DS3231 myRTC;

extern byte g_month;
extern byte g_date;
extern byte g_hour;
extern byte g_minute;
extern byte g_years;

extern int tmp_dates [5];



void set_time();

void get_time();