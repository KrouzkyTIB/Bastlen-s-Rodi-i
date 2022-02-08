#ifndef __BUTTON__HANDLER__HPP__
#define __BUTTON__HANDLER__HPP__

#define TIME_SET_BUTTON A1
#define TIME_PLUS_BUTTON A0
#define TIME_MINUS_BUTTON A2

struct ButtonsStatus{
    bool setTimeClicked;
    bool timePlusClicked;
    bool timeMinusClicked;
    bool setAlarmClicked;
    bool snoozeClicked;
};



void initButtons();
bool isButtonClicked(int button, bool * buttonStatus);
ButtonsStatus getButtonsStatus();

#endif