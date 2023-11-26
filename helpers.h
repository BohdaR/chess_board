#ifndef HELPERS_H
#define HELPERS_H

#include "constants.h"

int getOppositeColorKingSquare() {
    if (PositionDynamics.whoseMove == WHITE) {
        return blackKingSquare;
    }
    return whiteKingSquare;
}

int getOwnKingSquare() {
    if (PositionDynamics.whoseMove == WHITE) {
        return whiteKingSquare;
    }
    return blackKingSquare;
}

void toggleWhoseMove() {
    if (PositionDynamics.whoseMove == WHITE) {
        PositionDynamics.whoseMove = BLACK;
    }
    else {
        PositionDynamics.whoseMove = WHITE;
    }
}

void showStalemateMessage() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Draw!");
    lcd.setCursor(0, 1);
    lcd.print("1/2 - 1/2");
}

void showWhoseMove() {
    lcd.clear();
    lcd.setCursor(0, 0);
    if (PositionDynamics.whoseMove == WHITE) {
        lcd.print("White to move!");
        return;
    }
    lcd.print("Black to move!");
}

void resetPositionDynamics() {
    PositionDynamics.pickedSquare = -1;
    PositionDynamics.attackedPieceSquare = -1;
    PositionDynamics.moveTypes.capture = false;
    PositionDynamics.moveTypes.check = false;
    PositionDynamics.moveTypes.promotion = false;
    PositionDynamics.moveTypes.checkmate = false;
}

#endif
