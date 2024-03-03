#ifndef CASTLING_H
#define CASTLING_H

#include "checkmate.h"
#include "stalemate.h"
#include "helpers.h"
#include "clock.h"
#include "threefold_repetition.h"
#include "api.h"

bool canCastleTheKing(MoveType castlingType) {
    int kingPosition = (PositionDynamics.whoseMove == WHITE) ? WHITE_KING_START_SQUARE : BLACK_KING_START_SQUARE;
    if (isKingInCheck(kingPosition)) {
        return false;
    }

    Piece king = BIT_BOARD[kingPosition];
    bool canCastle = true;
    int lookupDirection = (castlingType == LONG_CASTLING) ? -1 : 1;

    if ((king & WHITE) && !(PositionDynamics.castlingRights & (castlingType >> 3))) {
        return false;
    }

    if ((king & BLACK) && !(PositionDynamics.castlingRights & (castlingType >> 5))) {
        return false;
    }

    for (int i = 1; i <= 2; i++) {
        if (BIT_BOARD[kingPosition + lookupDirection * i]) {
            return false;
        }
        BIT_BOARD[kingPosition] = EMPTY;
        BIT_BOARD[kingPosition + lookupDirection * i] = king;

        canCastle = !isKingInCheck(kingPosition + lookupDirection * i);

        BIT_BOARD[kingPosition] = king;
        BIT_BOARD[kingPosition + lookupDirection * i] = EMPTY;

        if (!canCastle) {
            break;
        }
    }

    return canCastle;
}

String getCastlingNotation(MoveType castlingType, MOVE_TYPES moveTypes) {
    String notation = "O-O";
    if (castlingType == LONG_CASTLING) {
        notation = "O-O-O";
    }

    if (moveTypes.checkmate) {
        notation += "#";
        return notation;
    }

    if (moveTypes.check) {
        notation += "+";
    }
    return notation;
}

void performCastling(MoveType castlingType, int rookToSquare, int rookFromSquare) {
    // Wait for the rook to appear on the target square
    while (readMux(rookToSquare) < peakValue) {
        updateClock();
        delay(90);
        // Wait for the rook to leave the square
        while (readMux(rookFromSquare) > peakValue) {
            updateClock();
            delay(90);
        }
    }

    updatePosition(rookFromSquare, rookToSquare);
    PositionDynamics.moveTypes.check = isKingInCheck(getOppositeColorKingSquare());
    PositionDynamics.moveTypes.checkmate = isCheckMate(getOppositeColorKingSquare());
    if (isStalemate(getOppositeColorKingSquare())) {
        PositionDynamics.moveTypes.draw = true;
        showDrawMessage("Stalemate!");
        return;
    }

    applyIncrement();
    toggleWhoseMove();
    savePosition();
    showWhoseMove();
//    lcd.setCursor(0, 1);
//    lcd.print(getCastlingNotation(castlingType, PositionDynamics.moveTypes));
    String castlingNotation = getCastlingNotation(castlingType, PositionDynamics.moveTypes);
//    Serial.println(castlingNotation);
    if (rookFromSquare > rookToSquare) {
        sendMove(castlingNotation, rookToSquare - 1, rookToSquare + 1, true);
    } else {
        sendMove(castlingNotation, rookToSquare + 1, rookToSquare - 1, true);
    }

    resetPositionDynamics();
}

#endif
