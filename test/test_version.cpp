#include "iostream"
#include "../move_validation.h"

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
int main() {
    BIT_BOARD[56] = WHITE_QUEEN;
    blackKingSquare = 57;
    BIT_BOARD[60] = EMPTY;
    BIT_BOARD[8] = EMPTY;
    BIT_BOARD[blackKingSquare] = BLACK_KING;
    PositionDynamics.whoseMove = BLACK;
    std::cout << isLegalMove(blackKingSquare, 56, blackKingSquare);
}
