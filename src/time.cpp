#include "time.hpp"

#include <Wire.h>
DS3231 rtc;

Time settingsTime;
AlarmSettings alarmSettings;
bool alarmRinging = false;

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
    alarmSettings = {
        .ringTime = {
            .hours = 0,
            .mins = 0,
            .seconds = 0},
        .on = false};
    alarmRinging = false;
    pinMode(ALARM_PIN, OUTPUT);
}

AlarmSettings getAlarmSettings() {
    return alarmSettings;
}
void setAlarmTime(Time time) {
    alarmSettings.ringTime = time;
}
void toggleAlarmStatus() {
    alarmSettings.on = !alarmSettings.on;
}

void checkAlarm(Time currentTime) {
    if (!alarmSettings.on || currentTime.seconds != 0) {
        return;
    }
    if (currentTime.hours == alarmSettings.ringTime.hours && currentTime.mins == alarmSettings.ringTime.mins) {
        digitalWrite(ALARM_PIN, HIGH);
    }
}

void turnOffAlarm() {
    digitalWrite(ALARM_PIN, LOW);
}
