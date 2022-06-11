#include "time.hpp"
#include <EEPROM.h>
#include <Wire.h>
/**
 * @brief Číslo, které se uloží na 0 adresu v paměti EEPROM, abychom věděli, že tam jsou již uložené naše data
 * 
 */
#define EEPROM_MAGIC_NUMBER 23
/**
 * @brief Výčet adres, kde máme uložená data v paměti EEPROM
 * 
 */
enum EepromSaveAddresses {
    MAGIC_NUMBER_ADDRESS,
    ALARM_STATUS_ADDRESS,
    ALARM_HOURS_ADDRESS,
    ALARM_MINUTES_ADDRESS
};
/**
 * @brief Objekt, pomocí kterého můžeme komunikovat s čipem reálných hodin na desce
 */
DS3231 rtc;

/**
 * @brief Datová struktura, který si v sobě uchovává data od uživatele, když nastavuje čas
 */
Time settingsTime;

/**
 * @brief Datová struktura na udržení informací o buzení uživatele
 */
AlarmSettings alarmSettings;

/**
 * @brief Logická hodnota, zdali má sirénka na buzení pískat, či nikoliv
 */
bool alarmRinging;

/**
 * @brief Inicializuje čip reálných hodin a nastaví na něm správný čas
 * 
 * @param hours Hodiny, které chceme nastavit na čip
 * @param mins Minuty, které chceme nastavit na čip
 * @param seconds Sekundy, které chceme nastavit na čip
 */
void initTime(uint8_t hours, uint8_t mins, uint8_t seconds) {
    Wire.begin();
    rtc.setHour(hours);
    rtc.setMinute(mins);
    rtc.setSecond(seconds);
}

/**
 * @brief Zjistí a vrátí čas z čipu reálného času
 * 
 * @return Aktuální čas hodin
 */
Time getTime() {
    bool h12Flag, pmFlag;
    Time currentTime = {
        .hours = rtc.getHour(h12Flag, pmFlag),
        .mins = rtc.getMinute(),
        .seconds = rtc.getSecond()};
    return currentTime;
}
/**
 * @brief Připravý data na nastavení času uživatelem
 * 
 * @param forAlarmSetting True pokud chceme nastavovat čas zvonění alarmu, false pokud cheme nastavit hodiny
 */

void prepareSettingsTime(bool forAlarmSetting) {
    if (forAlarmSetting) {
        settingsTime = alarmSettings.ringTime;
    } else {
        settingsTime = getTime();
    }
    settingsTime.seconds = 0;
}

/**
 * @brief Zvýší aktuální hodinu o 1
 */
void incrementHour() {
    settingsTime.hours = settingsTime.hours + 1 < HOURS_IN_DAY ? settingsTime.hours + 1 : 0;
}

/**
 * @brief Sníží aktuální hodinu o 1
 */
void decrementHour() {
    settingsTime.hours = settingsTime.hours == 0 ? HOURS_IN_DAY - 1 : settingsTime.hours - 1;
}
/**
 * @brief Zvýší aktuální minutu o 1
 */
void incrementMinute() {
    settingsTime.mins = settingsTime.mins + 1 < MINUTES_IN_HOUR ? settingsTime.mins + 1 : 0;
}

/**
 * @brief Sníží aktuální minutu o 1
 */
void decrementMinute() {
    settingsTime.mins = settingsTime.mins == 0 ? MINUTES_IN_HOUR - 1 : settingsTime.mins - 1;
}
/**
 * @brief Vrátí čas, který nastavit uživatel
 * @return Čas nastavený uživatelem
 */
Time getSettingsTime() {
    return settingsTime;
}
/**
 * @brief Nastaví čas na desce a v čipu reálného času
 * 
 * @param time Čas, který cheme nastavi
 */
void setTime(Time time) {
    rtc.setHour(time.hours);
    rtc.setMinute(time.mins);
    rtc.setSecond(time.seconds);
}
/**
 * @brief Inicializuje nastavení budíku v paměti EEPROM
 * 
 */
void initAlarmSettings() {
    uint8_t magicNumber = EEPROM.read(MAGIC_NUMBER_ADDRESS);
    if (magicNumber == EEPROM_MAGIC_NUMBER) {
        // Data are saved in eeprom, time to load them
        alarmSettings = {
            .ringTime = {
                .hours = EEPROM.read(ALARM_HOURS_ADDRESS),
                .mins = EEPROM.read(ALARM_MINUTES_ADDRESS),
                .seconds = 0},
            .on = EEPROM.read(ALARM_STATUS_ADDRESS) == 1};
    } else {
        // Data are not saved in eeprom, time to prepare EEPROM and generateData
        alarmSettings = {
            .ringTime = {
                .hours = 0,
                .mins = 0,
                .seconds = 0},
            .on = false};
        for (uint16_t i = 0; i < EEPROM.length(); i++) {
            // erase eeprom
            EEPROM.write(i, 0);
        }
        // now is eeprom prepared for write and reading
        EEPROM.write(MAGIC_NUMBER_ADDRESS, EEPROM_MAGIC_NUMBER);
    }
    alarmRinging = false;
    pinMode(ALARM_PIN, OUTPUT);
}
/**
 * @brief Vrací nastavení alarmu
 * @return Nastavení alarmu
 */
AlarmSettings getAlarmSettings() {
    return alarmSettings;
}
/**
 * @brief Nastaví čas, kdy alarm bude vyzvánět
 * @param time Čas vyzvánění alarmu
 */

void setAlarmTime(Time time) {
    alarmSettings.ringTime = time;
    EEPROM.write(ALARM_HOURS_ADDRESS, time.hours);
    EEPROM.write(ALARM_MINUTES_ADDRESS, time.mins);
}
/**
 * @brief Změní stav alarmu na z on na off a z off na on
 * 
 */

void toggleAlarmStatus() {
    alarmSettings.on = !alarmSettings.on;
    EEPROM.write(ALARM_STATUS_ADDRESS, alarmSettings.on);
}
/**
 * @brief Kontroluje periodicky, zdali je čas zapnout alarm či nikoliv 
 * @param currentTime 
 */
void checkAlarm(Time currentTime) {
    if (!alarmSettings.on || currentTime.seconds != 0) {
        return;
    }
    if (currentTime.hours == alarmSettings.ringTime.hours && currentTime.mins == alarmSettings.ringTime.mins) {
        digitalWrite(ALARM_PIN, HIGH);
        alarmRinging = true;
    }
}
/**
 * @brief Data o tom, zdali alarm právě zvoní
 * @return true Pokud zvoní
 * @return false Pokud nezvoní
 */
bool isAlarmRinging(){
    return alarmRinging;
}

/**
 * @brief Deaktivuje právě zvonící alarm
 * 
 */
void turnOffAlarm() {
    digitalWrite(ALARM_PIN, LOW);
    alarmRinging = false;
}
