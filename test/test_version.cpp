#include "iostream"
#include "../constants.h"
#include "../move_validation.h"
#include "../checkmate.h"

char getFileLetter(int squareIndex) {
    char fileLetter = 'a' + squareIndex % 8;
    return fileLetter;
}

int getRankNumber(int squareIndex) {
    return squareIndex / 8 + 1;
}

int getPieceColor(int piece) {
    return piece & 3; // we take first 2 bits which are use for piece color
}

void clearSquare(int squareIndex) {
    BIT_BOARD[squareIndex] = EMPTY;
}

void updatePosition(int fromSquare, int toSquare) {
    Piece movedPiece = BIT_BOARD[fromSquare];
    clearSquare(fromSquare);
    BIT_BOARD[toSquare] = movedPiece;
}

void capturePiece(int attackingPiece, int capturedPiece) {
    updatePosition(attackingPiece, capturedPiece);

}

int getSquareColor(int square) {
    // return 0 for black square and 1 for white square
    return (square % 8 + square / 8) % 2;
}

void castle() {

}

int main() {
    whiteKingSquare = 4;
    updatePosition(1, 50);

    std::cout << isKingInCheck(blackKingSquare);
    PositionDynamics.whoseMove = BLACK;
}
