#include "iostream"
#include "../constants.h"
#include "../stalemate.h"
#include "../checkmate.h"

void clearSquare(int squareIndex) {
    BIT_BOARD[squareIndex] = EMPTY;
}

void updatePosition(int fromSquare, int toSquare) {
    Piece movedPiece = BIT_BOARD[fromSquare];
    clearSquare(fromSquare);
    BIT_BOARD[toSquare] = movedPiece;
}

int main() {
    updatePosition(13, 29);
    updatePosition(14, 30);
    updatePosition(59, 31);

//    isKingInCheck(12);
    for (int i = 0; i < 1000000; i++) {
        isStalemate(whiteKingSquare);
    }
}
