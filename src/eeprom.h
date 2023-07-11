#include <Arduino.h>
#include <EEPROM.h>

extern bool sound;
extern bool bluetooth;
extern bool g_is_autodefil;
extern float g_delay_autodefil;
extern int g_current_sea_pressure_level;

void save_to_eeprom_sound_blu();
void save_to_eeprom_autodefil();
void save_to_eeprom_sealevel();
void retore_eeprom_settings();