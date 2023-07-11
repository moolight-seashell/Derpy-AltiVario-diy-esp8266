#include "eeprom.h"

#define EEPROM_SIZE 12

#define eeprom_location_sound 0
#define eeprom_location_bluetooth 1
#define eeprom_location_is_autodefil 2
#define eeprom_location_del_autodefil 3
#define eeprom_location_sea_pressure 7

//settings


void save_to_eeprom_sound_blu(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(eeprom_location_sound, sound);
    EEPROM.put(eeprom_location_bluetooth, bluetooth);
    EEPROM.end();
}

void save_to_eeprom_autodefil(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(eeprom_location_is_autodefil, g_is_autodefil);
    EEPROM.put(eeprom_location_del_autodefil, g_delay_autodefil);
    EEPROM.end();
}
void save_to_eeprom_sealevel(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(eeprom_location_sea_pressure, g_current_sea_pressure_level);
    EEPROM.end();
}

void retore_eeprom_settings(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(eeprom_location_sound, sound);
    EEPROM.get(eeprom_location_bluetooth, bluetooth);
    EEPROM.get(eeprom_location_is_autodefil, g_is_autodefil);
    EEPROM.get(eeprom_location_del_autodefil, g_delay_autodefil);
    EEPROM.get(eeprom_location_sea_pressure, g_current_sea_pressure_level);
    EEPROM.end();
}