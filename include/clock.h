#ifndef CLOCK_H
#define CLOCK_H

#include "Arduino.h"
#include "constants.h"
#include "helpers.h"

void chooseTimeControl() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Choose");

    lcd.setCursor(0, 1);
    lcd.print("time control");
    bool timeControlChosen = false;
    while (!timeControlChosen) {
        for (int i = 16; i < 48; i++) {
            float squareValue = readMux(i);
            if (squareValue > peakValue) {
                timeControlChosen = true;
                switch (i) {
                    case 16:
                        whitePlayerTime = 180000;
                        blackPlayerTime = 180000;
                        timeIncrement = 0;
                        break;
                    case 24:
                        whitePlayerTime = 180000;
                        blackPlayerTime = 180000;
                        timeIncrement = 2000;
                        break;
                    case 32:
                        whitePlayerTime = 300000;
                        blackPlayerTime = 300000;
                        timeIncrement = 0;
                        break;
                    case 40:
                        whitePlayerTime = 300000;
                        blackPlayerTime = 300000;
                        timeIncrement = 2000;
                        break;
                    case 17:
                        whitePlayerTime = 600000;
                        blackPlayerTime = 600000;
                        timeIncrement = 0;
                        break;
                    case 25:
                        whitePlayerTime = 600000;
                        blackPlayerTime = 600000;
                        timeIncrement = 5000;
                        break;
                    case 33:
                        whitePlayerTime = 900000;
                        blackPlayerTime = 900000;
                        timeIncrement = 0;
                        break;
                    case 41:
                        whitePlayerTime = 900000;
                        blackPlayerTime = 900000;
                        timeIncrement = 10000;
                        break;
                    case 18:
                        whitePlayerTime = 5400000;
                        blackPlayerTime = 5400000;
                        timeIncrement = 30000;
                        break;
                    default:
                        break;
                }
                break;
            }
        }
    }
}

String formatTime(unsigned long time) {
    int minutes = time / 60000;
    int seconds = time / 1000 % 60;
    int miliseconds = time % 1000 / 100;
    char formattedTime[6];
    if (time >= 60000) {
        sprintf(formattedTime, "%02d:%02d", minutes, seconds);
    } else {
        sprintf(formattedTime, "%02d.%d", seconds, miliseconds);
    }
    return String(formattedTime);
}

void decrementTime() {
    if (PositionDynamics.whoseMove == BLACK) blackPlayerTime -= 100;
    if (PositionDynamics.whoseMove == WHITE) whitePlayerTime -= 100;
}

void applyIncrement() {
    if (PositionDynamics.whoseMove == BLACK) blackPlayerTime += timeIncrement;
    if (PositionDynamics.whoseMove == WHITE) whitePlayerTime += timeIncrement;
}

void updateClock() {
    String whiteFormattedTime = formatTime(whitePlayerTime);
    String blackFormattedTime = formatTime(blackPlayerTime);

    lcd.setCursor(0, 1);
    lcd.print(whiteFormattedTime);
    lcd.setCursor(11, 1);
    lcd.print(blackFormattedTime);

    decrementTime();
}

#endif
