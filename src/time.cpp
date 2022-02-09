#include "time.hpp"

#include <EEPROM.h>
#include <Wire.h>

#define EEPROM_MAGIC_NUMBER 23
enum EepromSaveAddresses {
    MAGIC_NUMBER_ADDRESS,
    ALARM_STATUS_ADDRESS,
    ALARM_HOURS_ADDRESS,
    ALARM_MINUTES_ADDRESS
};

DS3231 rtc;
Time settingsTime;
AlarmSettings alarmSettings;
bool alarmRinging;


void initTime(uint8_t hours, uint8_t mins, uint8_t seconds) {
    Wire.begin();
    rtc.setHour(hours);
    rtc.setMinute(mins);
    rtc.setSecond(seconds);
}

Time getTime() {
    bool h12Flag, pmFlag;
    Time currentTime = {
        .hours = rtc.getHour(h12Flag, pmFlag),
        .mins = rtc.getMinute(),
        .seconds = rtc.getSecond()};
    return currentTime;
}

void prepareSettingsTime(bool forAlarmSetting) {
    if (forAlarmSetting) {
        settingsTime = alarmSettings.ringTime;
    } else {
        settingsTime = getTime();
    }
    settingsTime.seconds = 0;
}

void incrementHour() {
    settingsTime.hours = settingsTime.hours + 1 < HOURS_IN_DAY ? settingsTime.hours + 1 : 0;
}

void decrementHour() {
    settingsTime.hours = settingsTime.hours == 0 ? HOURS_IN_DAY - 1 : settingsTime.hours - 1;
}

void incrementMinute() {
    settingsTime.mins = settingsTime.mins + 1 < MINUTES_IN_HOUR ? settingsTime.mins + 1 : 0;
}

void decrementMinute() {
    settingsTime.mins = settingsTime.mins == 0 ? MINUTES_IN_HOUR - 1 : settingsTime.mins - 1;
}

Time getSettingsTime() {
    return settingsTime;
}

void setTime(Time time) {
    rtc.setHour(time.hours);
    rtc.setMinute(time.mins);
    rtc.setSecond(time.seconds);
}

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

AlarmSettings getAlarmSettings() {
    return alarmSettings;
}
void setAlarmTime(Time time) {
    alarmSettings.ringTime = time;
    EEPROM.write(ALARM_HOURS_ADDRESS, time.hours);
    EEPROM.write(ALARM_MINUTES_ADDRESS, time.mins);
}
void toggleAlarmStatus() {
    alarmSettings.on = !alarmSettings.on;
    EEPROM.write(ALARM_STATUS_ADDRESS, alarmSettings.on);
}

void checkAlarm(Time currentTime) {
    if (!alarmSettings.on || currentTime.seconds != 0) {
        return;
    }
    if (currentTime.hours == alarmSettings.ringTime.hours && currentTime.mins == alarmSettings.ringTime.mins) {
        digitalWrite(ALARM_PIN, HIGH);
        alarmRinging = true;
    }
}

bool isAlarmRinging(){
    return alarmRinging;
}


void turnOffAlarm() {
    digitalWrite(ALARM_PIN, LOW);
    alarmRinging = false;
}
