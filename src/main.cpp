#include <Arduino.h>

#include "buttons/buttonHandler.hpp"
#include "display/display.hpp"
#include "time/time.hpp"

#define MILLIS_IN_SECOND 1000

enum SetTimeSteps {
    SET_HOURS,
    SET_MINUTES
};
/**
 * @brief Jednotlivé stavy hodin
 * CLOCK_RUNNING - hodiny ukazují čas
 * TIME_SETTING - hodiny jsou v módu nastavování času
 * ALARM_SETTING - hodiny jsou v módu nastavování alarmu
 */
enum ClockStages {
    CLOCK_RUNNING,
    TIME_SETTING,
    ALARM_SETTING
};
/**
 * @brief Datová struktura na udržování aktuálního času
 * 
 */
Time currentTime;
unsigned long lastMillis = 0;
uint8_t setTimeStep = SET_HOURS;
/**
 * @brief Aktuální mód hodin, ve kterém se hodiny nacházejí
 * 
 */
uint8_t clockStage = CLOCK_RUNNING;

void clockRoutine();
void handleButtons();
void handleClockSetting(ButtonsStatus status);
void setClockRoutine();
void setAlarmRoutine();
void handleAlarmSettings(ButtonsStatus status);

/**
 * @brief První, ze dvou hlavních funkcí, zde dojde k inicializaci hodin
 */
void setup() {
    //Zde budeme doplňovat inicializaci programu
}
/**
 * @brief Hlavní smyčka programu 
 */

void loop() {
    //Zde budeme doplňovat hlavní smyčku programu
}
/**
 * @brief Funkce, která se stará o normální běh hodin
 */

void clockRoutine() {
    if (abs(millis() - lastMillis) >= MILLIS_IN_SECOND) {
        currentTime = getTime();
        lastMillis = millis();
        blinkWithDots(currentTime.seconds);
        checkAlarm(currentTime);
    }
    showTime(currentTime.hours, currentTime.mins, DIGIT_DELAY);
}

/**
 * @brief Funkce, která se stará o obsluhu tlačítek
 * 
 */
void handleButtons() {
    ButtonsStatus status = getButtonsStatus();
    
    if (status.setAlarmClicked || status.setTimeClicked || status.timeMinusClicked || status.timePlusClicked) {
        if (isAlarmRinging()) {
            turnOffAlarm();
            return;
        }
    }

    switch (clockStage) {
        case CLOCK_RUNNING:
            if (status.setTimeClicked) {
                clockStage = TIME_SETTING;
                prepareSettingsTime(false);
            } else if (status.setAlarmClicked) {
                prepareSettingsTime(true);
                clockStage = ALARM_SETTING;
            }
            break;
        case TIME_SETTING:
            handleClockSetting(status);
            break;
        case ALARM_SETTING:
            handleAlarmSettings(status);
            break;
    }
}
/**
 * @brief Stará se o nastavení času na hodinách
 * @param status Stav všech tlačítek na desce
 */

void handleClockSetting(ButtonsStatus status) {
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

/**
 * @brief Nastavení desky na mód nastavení času
 */

void setClockRoutine() {
    turnOffDots();
    if (abs(millis() - lastMillis) >= MILLIS_IN_SECOND) {
        currentTime = getTime();
        lastMillis = millis();
        checkAlarm(currentTime);
    }
    Time settingsTime = getSettingsTime();
    switch (setTimeStep) {
        case SET_HOURS:
            showBlinkingHours(currentTime, settingsTime);
            break;
        case SET_MINUTES:
            showBlinkingMinutes(currentTime, settingsTime);
            break;
    }
}

/**
 * @brief Funkce na nastavení alarmu
 * @param status Stav tlačítek na desce
 */

void handleAlarmSettings(ButtonsStatus status) {
    switch (setTimeStep) {
        case SET_HOURS:
            if (status.setAlarmClicked) {
                setTimeStep = SET_MINUTES;
            } else if (status.timePlusClicked) {
                incrementHour();
            } else if (status.timeMinusClicked) {
                decrementHour();
            } else if (status.setTimeClicked) {
                toggleAlarmStatus();
            }
            break;
        case SET_MINUTES:
            if (status.setAlarmClicked) {
                setTimeStep = SET_HOURS;
                clockStage = CLOCK_RUNNING;
                setAlarmTime(getSettingsTime());
                currentTime = getTime();
            } else if (status.timePlusClicked) {
                incrementMinute();
            } else if (status.timeMinusClicked) {
                decrementMinute();
            } else if (status.setTimeClicked) {
                toggleAlarmStatus();
            }
            break;
    }
}
/**
 * @brief Nastaví stav hodin na mód nastavení alarmu
 */

void setAlarmRoutine() {
    turnOffDots();
    if (abs(millis() - lastMillis) >= MILLIS_IN_SECOND) {
        currentTime = getTime();
        lastMillis = millis();
        checkAlarm(currentTime);
    }
    Time settingsTime = getSettingsTime();
    switch (setTimeStep) {
        case SET_HOURS:
            if (getAlarmSettings().on) {
                showBlinkingHours(currentTime, settingsTime);
            } else {
                showBlinkingDashes(currentTime, true);
            }
            break;
        case SET_MINUTES:
            if (getAlarmSettings().on) {
                showBlinkingMinutes(currentTime, settingsTime);
            } else {
                showBlinkingDashes(currentTime, false);
            }
            break;
    }
}
