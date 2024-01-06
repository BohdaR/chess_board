#ifndef HELPERS_H
#define HELPERS_H

#include "constants.h"

float readMux(int channel) {
    digitalWrite(s0, channel & 1);
    digitalWrite(s1, channel & 2);
    digitalWrite(s2, channel & 4);
    digitalWrite(s3, channel & 8);
    digitalWrite(s4, channel & 16);
    digitalWrite(s5, channel & 32);

    //read the value at the SIG pin
    int val = analogRead(SIG_pin);

    //return the value
    return val * 3.3 / 4095;
}

char getFileLetter(int squareIndex) {
    char fileLetter = 'a' + squareIndex % 8;
    return fileLetter;
}

int getRankNumber(int squareIndex) {
    return squareIndex / 8 + 1;
}

String getSquare(int squareIndex) {
    return String(getFileLetter(squareIndex)) + String(getRankNumber(squareIndex));
}

int getPieceColor(int piece) {
    return piece & 3; // we take first 2 bits which are use for piece color
}

String getPieceLatter(int piece) {
    piece = piece & 252; // here we take only 6 bit of piece 'cause first 2 bits are used for piece color
    switch (piece) {
        case PAWN:
            return "";
        case ROOK:
            return "R";
        case KNIGHT:
            return "N";
        case BISHOP:
            return "B";
        case QUEEN:
            return "Q";
        case KING:
            return "K";
    }
    return "";
}

void clearSquare(int squareIndex) {
    BIT_BOARD[squareIndex] = EMPTY;
}

void updatePosition(int fromSquare, int toSquare) {
    Piece movedPiece = BIT_BOARD[fromSquare];
    clearSquare(fromSquare);
    BIT_BOARD[toSquare] = movedPiece;
}

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
    } else {
        PositionDynamics.whoseMove = WHITE;
    }
}

void showDrawMessage(const String message = "Draw!") {
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
    for (int i = 0; i < 64; i++) {
        if (BIT_BOARD[i] & color) {
            int piece = BIT_BOARD[i] & 252;
            if (piece & KING) {
                continue;
            }

            if (piece & (PAWN | ROOK | QUEEN)) {
                return true;
            } else {
                materialAmount += piece;
            }
        }
    }
    // 16 is equal one bishop or 2 knights and the material amount is bigger we have sufficient amount of material
    return materialAmount > 16;
}

void clearBoard() {
    for(int i = 0; i < 64; i++) {
        BIT_BOARD[i] = EMPTY;
    }
}

void resetBitBoard() {
    // This function set the BIT_BOARD to starting state
    const Piece whiteFirstRankPieces[] = {WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP,
                                        WHITE_KNIGHT, WHITE_ROOK};
    const Piece blackLastRankPieces[] = {BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP,
                                       BLACK_KNIGHT, BLACK_ROOK};

    for (int i = 0; i < 8; ++i) {
        BIT_BOARD[i] = whiteFirstRankPieces[i];
        BIT_BOARD[56 + i] = blackLastRankPieces[i];
        BIT_BOARD[8 + i] = WHITE_PAWN;
        BIT_BOARD[40 + i] = BLACK_PAWN;
    }
    for (int i = 16; i < 48; i++) {
        BIT_BOARD[i] = EMPTY;
    }
}

#endif
