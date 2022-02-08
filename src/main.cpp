#include <Arduino.h>

#include "buttonHandler.hpp"
#include "display.hpp"
#include "time.hpp"

#define MILLIS_IN_SECOND 1000
#define DIGIT_DELAY 1

enum SetTimeSteps {
    NOT_SETTING,
    SET_HOURS,
    SET_MINUTES
};
enum ClockStages {
    CLOCK_RUNNING,
    TIME_SETTING,
    ALARM_SETTING
};

Time currentTime;
unsigned long lastMillis = 0;
uint8_t setTimeStep = SET_HOURS;
uint8_t clockStage = CLOCK_RUNNING;

void clockRoutine();
void handleButtons();
void handleClockSetting(ButtonsStatus status);
void setClockRoutine();

void setup() {
    initDisplay();
    initTime(13, 51, 0);
    Serial.begin(9600);
    currentTime = getTime();
    initButtons();
}

void loop() {
    // put your main code here, to run repeatedly:

    switch (clockStage) {
        case CLOCK_RUNNING:
            clockRoutine();
            break;
        case TIME_SETTING:
            setClockRoutine();
            break;
        case ALARM_SETTING:
            break;
    }
    handleButtons();
}

void clockRoutine() {
    if (abs(millis() - lastMillis) >= MILLIS_IN_SECOND) {
        currentTime = getTime();
        lastMillis = millis();
        blinkWithDots(currentTime.seconds);
    }
    showTime(currentTime.hours, currentTime.mins, DIGIT_DELAY);
}

void handleButtons() {
    ButtonsStatus status = getButtonsStatus();
    // TODO add snooze and set alarm
    switch (clockStage) {
        case CLOCK_RUNNING:
            if (status.setTimeClicked) {
                clockStage = TIME_SETTING;
                prepareSettingsTime();
            } else if (status.setAlarmClicked) {
                clockStage = ALARM_SETTING;
            }
            break;
        case TIME_SETTING:
            handleClockSetting(status);
            break;
        case ALARM_SETTING:
            break;
    }
}

void handleClockSetting(ButtonsStatus status) {
    if (status.setAlarmClicked) {
        clockStage = ALARM_SETTING;
        return;
    }

    switch (setTimeStep) {
        case SET_HOURS:
            if (status.setTimeClicked) {
                setTimeStep = SET_MINUTES;
            } else if (status.timePlusClicked) {
                incrementHour();
            } else if (status.timeMinusClicked) {
                decrementHour();
            }
            break;
        case SET_MINUTES:
            if (status.setTimeClicked) {
                setTimeStep = SET_HOURS;
                clockStage = CLOCK_RUNNING;
                setTime(getSettingsTime());
                currentTime = getTime();
            } else if (status.timePlusClicked) {
                incrementMinute();
            } else if (status.timeMinusClicked) {
                decrementMinute();
            }
            break;
    }
}

void setClockRoutine() {
    turnOffDots();
    if (abs(millis() - lastMillis) >= MILLIS_IN_SECOND) {
        currentTime = getTime();
        lastMillis = millis();
    }
    Time settingsTime = getSettingsTime();
    switch (setTimeStep) {
        case SET_HOURS:
            if (currentTime.seconds % 2 == 0) {
                showHours(settingsTime.hours, DIGIT_DELAY);
            } else {
                turnOffAllDigits();
                delay(DIGIT_DELAY * 2);
            }
            showMinutes(settingsTime.mins, DIGIT_DELAY);
            break;
        case SET_MINUTES:
            showHours(settingsTime.hours, DIGIT_DELAY);
            if (currentTime.seconds % 2 == 0) {
                showMinutes(settingsTime.mins, DIGIT_DELAY);
            } else {
                turnOffAllDigits();
                delay(DIGIT_DELAY * 2);
            }
            break;
    }
}
