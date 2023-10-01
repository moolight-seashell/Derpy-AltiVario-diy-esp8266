#include "eeprom.h"

#define EEPROM_SIZE 32

#define eeprom_location_sound 0
#define eeprom_location_bluetooth 1
#define eeprom_location_is_autodefil 2
#define eeprom_location_del_autodefil 3
#define eeprom_location_sea_pressure 7
#define eeprom_location_fly_hour 11
#define eeprom_location_fly_min 15
#define eeprom_location_alti 19
#define eeprom_location_vario_max 23
#define eeprom_location_vario_min 27

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

void save_to_eeprom_flytime(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(eeprom_location_fly_hour, g_total_fly_hour);
    EEPROM.put(eeprom_location_fly_min, g_total_fly_min);
    EEPROM.end();
}

void save_to_eeprom_alti(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(eeprom_location_alti, g_total_alti);
    EEPROM.end();
}

void save_to_eeprom_var_max(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(eeprom_location_vario_max, g_total_vario_max);
    EEPROM.end();
}

void save_to_eeprom_var_min(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(eeprom_location_vario_min, g_total_vario_min);
    EEPROM.end();
}


void retore_eeprom_settings(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(eeprom_location_sound, sound);
    EEPROM.get(eeprom_location_bluetooth, bluetooth);
    EEPROM.get(eeprom_location_is_autodefil, g_is_autodefil);
    EEPROM.get(eeprom_location_del_autodefil, g_delay_autodefil);
    EEPROM.get(eeprom_location_sea_pressure, g_current_sea_pressure_level);
    EEPROM.get(eeprom_location_fly_hour,g_total_fly_hour);
    EEPROM.get(eeprom_location_fly_min,g_total_fly_min);
    EEPROM.get(eeprom_location_alti,g_total_alti);
    EEPROM.get(eeprom_location_vario_max,g_total_vario_max);
    EEPROM.get(eeprom_location_vario_min,g_total_vario_min);
    EEPROM.end();
}