#include <Arduino.h>
#include <EEPROM.h>

extern bool sound;
extern bool bluetooth;
extern bool g_is_autodefil;
extern float g_delay_autodefil;
extern int g_current_sea_pressure_level;

extern int g_total_fly_hour;
extern int g_total_fly_min;
extern int g_total_alti;
extern float g_total_vario_max;
extern float g_total_vario_min;

void save_to_eeprom_sound_blu();
void save_to_eeprom_autodefil();
void save_to_eeprom_sealevel();
void save_to_eeprom_flytime();
void save_to_eeprom_var_max();
void save_to_eeprom_var_min();
void save_to_eeprom_alti();
void retore_eeprom_settings();