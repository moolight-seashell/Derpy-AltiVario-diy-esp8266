#include <Arduino.h>
#include <U8g2lib.h>
#include <cstdio>



extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;
extern float g_vario;
extern float g_vario_lim5;
extern float g_altitude;

extern float g_vario_history_5[6];// = {-5,-2,0,0,2,5};

extern int current_menu;
extern int menu_selected;
extern int sub_menu_selected;

extern bool tmp_sound;
extern bool tmp_bluetooth;


extern byte g_month;
extern byte g_date;
extern byte g_hour;
extern byte g_minute;
extern byte g_years;
extern int g_batt;

extern int tmp_dates [5];

extern int g_temperature;
extern int g_humidity;

extern bool tmp_is_autodefil;
extern float tmp_delay_autodefil;

extern int g_altitude_history[100];
extern int g_altitude_hmax;
extern int g_altitude_hmin;

extern int g_total_fly_hour;
extern int g_total_fly_min;
extern int g_total_alti;
extern float g_total_vario_max;
extern float g_total_vario_min;

void draw_main_screen();

void draw_start_screen();

void draw_header_0();

void draw_screen_0();

void draw_screen_1();

void draw_screen_2();

void draw_header_settings();

void draw_screen_settings();

void draw_screen_settings_sons();

void draw_screen_settings_date();

void draw_screen_settings_autodefil();

void draw_screen_settings_alti();
