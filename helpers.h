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

void showDrawMessage(char message[]="Draw!") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
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
}

bool isSufficientMaterial(Color color) {
    int materialAmount = 0;
    for(int i = 0; i < 64; i++) {
        if (BIT_BOARD[i] & color) {
            int piece = BIT_BOARD[i] & 252;
            if (piece & KING) {
                continue;
            }

            if (piece & (PAWN | ROOK | QUEEN)) {
                return true;
            }
            else {
                materialAmount += piece;
            }
        }
    }
    // 16 is equal one bishop or 2 knights and the material amount is bigger we have sufficient amount of material
    return materialAmount > 16;
}

#endif
