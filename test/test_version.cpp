#include "iostream"
#include "../constants.h"
#include "../stalemate.h"

void clearSquare(int squareIndex) {
    BIT_BOARD[squareIndex] = EMPTY;
}

void updatePosition(int fromSquare, int toSquare) {
    Piece movedPiece = BIT_BOARD[fromSquare];
    clearSquare(fromSquare);
    BIT_BOARD[toSquare] = movedPiece;
}

int main() {
    int kingSquare = 0;

//    isKingInCheck(12);
    std::cout << isStalemate(kingSquare) << "\n";
}
