#include "iostream"
#include "../checkmate.h"
#include "../castling.h"
#include "../helpers.h"

void clearSquare(int squareIndex) {
    BIT_BOARD[squareIndex] = EMPTY;
}

void updatePosition(int fromSquare, int toSquare) {
    Piece movedPiece = BIT_BOARD[fromSquare];
    clearSquare(fromSquare);
    BIT_BOARD[toSquare] = movedPiece;
}

int main() {
    int kingSquare = 21;
    int attackerSquare = 56;
    updatePosition(49, 51);
    updatePosition(58, attackerSquare);
    updatePosition(4, kingSquare);

//    isKingInCheck(12);
    std::cout << isKingInCheck(kingSquare) << "\n";
}
