#ifndef MOVE_REGISTRATION_H
#define MOVE_REGISTRATION_H

#include "constants.h"
#include "helpers.h"
#include "checkmate.h"
#include "stalemate.h"
#include "disambiguation.h"
#include "threefold_repetition.h"
#include "clock.h"

String getMoveNotation(int fromSquare, int toSquare, MOVE_TYPES moveTypes, int disambiguation) {
    int piece = BIT_BOARD[toSquare];
    String square = getPieceLatter(piece) + getSquare(toSquare);

    if (moveTypes.capture) {
        if (piece & PAWN) {
            char previousFileLetter = getFileLetter(fromSquare);
            square = String(previousFileLetter) + "x" + square;
        } else {
            square = String(square[0]) + "x" + square.substring(1, -1);
        }
    }

    String disambiguationNotation = getDisambiguationNotation(disambiguation, fromSquare);

    square = String(square[0]) + disambiguationNotation + square.substring(1, -1);

    if (moveTypes.checkmate) {
        square += "#";
        return square;
    }
    if (moveTypes.promotion) {
        square = square.substring(1, -1) + "=Q";
    }
    if (moveTypes.check) {
        square += "+";
    }

    return square;
}

void makeMove(int fromSquare, int toSquare, MOVE_TYPES moveTypes) {
    int disambiguation = getDisambiguation(fromSquare, toSquare);

    if (moveTypes.capture || (BIT_BOARD[fromSquare] & PAWN)) {
        resetPositionTracking();
    }
    updatePosition(fromSquare, toSquare);
    savePosition();

    if (currentPositionIndex == MAX_POSITIONS) {
        PositionDynamics.moveTypes.draw = true;
        showDrawMessage("50 moves rule!");
        return;
    }

    if (isTreeFoldRepetition()) {
        PositionDynamics.moveTypes.draw = true;
        showDrawMessage("Threefold!");
        return;
    }
    if (PositionDynamics.castlingRights & FULL_CASTLING_RIGHTS) {
        switch (fromSquare) {
            case 0:
                PositionDynamics.castlingRights -= 8;
                break;
            case 7:
                PositionDynamics.castlingRights -= 4;
                break;
            case 56:
                PositionDynamics.castlingRights -= 2;
                break;
            case 63:
                PositionDynamics.castlingRights -= 1;
                break;
        }
    }

    if (BIT_BOARD[toSquare] & KING) {
        if (BIT_BOARD[toSquare] & WHITE) {
            if (PositionDynamics.castlingRights & WHITE_CASTLING_RIGHTS) {
                PositionDynamics.castlingRights -= WHITE_CASTLING_RIGHTS;
            }
            whiteKingSquare = toSquare;
        } else {
            if (PositionDynamics.castlingRights & BLACK_CASTLING_RIGHTS) {
                PositionDynamics.castlingRights -= BLACK_CASTLING_RIGHTS;
            }
            blackKingSquare = toSquare;
        }
    }

    if (PositionDynamics.whoseMove == WHITE) {
        if (BIT_BOARD[toSquare] & PAWN && toSquare / 8 == 7) {
            BIT_BOARD[toSquare] = WHITE_QUEEN;
            moveTypes.promotion = true;
        }
        if (BIT_BOARD[toSquare] & PAWN && (toSquare - fromSquare) == 16) {
            PositionDynamics.enPassantSquare = toSquare - 8;
        }
        moveTypes.check = isKingInCheck(blackKingSquare);
        moveTypes.checkmate = isCheckMate(blackKingSquare);
        PositionDynamics.moveTypes.checkmate = moveTypes.checkmate;
    } else {
        if (BIT_BOARD[toSquare] & PAWN && toSquare / 8 == 0) {
            BIT_BOARD[toSquare] = BLACK_QUEEN;
            moveTypes.promotion = true;
        }
        if (BIT_BOARD[toSquare] & PAWN && (fromSquare - toSquare) == 16) {
            PositionDynamics.enPassantSquare = toSquare + 8;
        }
        moveTypes.check = isKingInCheck(whiteKingSquare);
        moveTypes.checkmate = isCheckMate(whiteKingSquare);
        PositionDynamics.moveTypes.checkmate = moveTypes.checkmate;
    }

    if (isStalemate(getOppositeColorKingSquare())) {
        PositionDynamics.moveTypes.draw = true;
        showDrawMessage("Stalemate!");
        return;
    }

    if (moveTypes.checkmate) {
        if (PositionDynamics.whoseMove == WHITE) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("White won! 1-0");
        } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Black won! 0-1");
        }
    }

    applyIncrement();
    toggleWhoseMove();
    if (!moveTypes.checkmate) {
        showWhoseMove();
    }
    String moveNotation = getMoveNotation(fromSquare, toSquare, moveTypes, disambiguation);
//    lcd.setCursor(0, 1);
//    lcd.print(moveNotation);
    Serial.println(moveNotation);

    resetPositionDynamics();
    if (!(isSufficientMaterial(WHITE) || isSufficientMaterial(BLACK))) {
        showDrawMessage("Draw!");
    }
}

#endif
