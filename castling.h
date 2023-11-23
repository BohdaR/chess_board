#ifndef CASTLING_H
#define CASTLING_H

#include "checkmate.h"

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

#endif
