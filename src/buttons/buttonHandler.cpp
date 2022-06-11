#include "buttonHandler.hpp"

#include <Arduino.h>

#define BUTTON_CLICKED 0

bool timeSetBtnClicked = false;
bool timePlusBtnClicked = false;
bool timeMinusBtnClicked = false;
bool alarmSetBtnClicked = false;
bool snoozeBtnClicked = false;

/**
 * @brief Inicializuje tlačítka a připravý je na vstupní signály
 *
 */
void initButtons() {
    pinMode(TIME_PLUS_BUTTON, INPUT_PULLUP);
    pinMode(TIME_MINUS_BUTTON, INPUT_PULLUP);
    pinMode(TIME_SET_BUTTON, INPUT_PULLUP);
    pinMode(ALERT_SET_BUTTON, INPUT_PULLUP);
    pinMode(SNOOZE_BUTTON, INPUT_PULLUP);
}

/**
 * @brief Podívá se, jestli je dané tlačítko stisknuté a pokud ano vrátí hodnotu true, jinak vrátí hodnotu false
 * 
 * @param button Pin, ke kterému je tlačítko připojeno
 * @param buttonStatus ukazatel na stav tlačítka v naší datové struktuře
 * @return true pokud je tlačítko stisknuté
 * @return false pokud není tlačítko stisknuto
 */
bool isButtonClicked(int button, bool* buttonStatus) {
    bool retValue = false;
    if (digitalRead(button) == BUTTON_CLICKED) {
        if (!*buttonStatus) {
            retValue = true;
        }
        *buttonStatus = true;
    } else {
        *buttonStatus = false;
    }
    return retValue;
}

/**
 * @brief Vrací stav všech tlačítek po přečtení vstupů Arduina
 * 
 * @return ButtonsStatus datová struktura obsahujíçí data o tom, která tlačítka jsou stisknuta a která nikoliv
 */

ButtonsStatus getButtonsStatus() {
    ButtonsStatus status = {
        .setTimeClicked = isButtonClicked(TIME_SET_BUTTON, &timeSetBtnClicked),
        .timePlusClicked = isButtonClicked(TIME_PLUS_BUTTON, &timePlusBtnClicked),
        .timeMinusClicked = isButtonClicked(TIME_MINUS_BUTTON, &timeMinusBtnClicked),
        .setAlarmClicked = isButtonClicked(ALERT_SET_BUTTON, &alarmSetBtnClicked),
        .snoozeClicked = isButtonClicked(SNOOZE_BUTTON, &snoozeBtnClicked)};

    return status;
}
