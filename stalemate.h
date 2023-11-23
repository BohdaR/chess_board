#ifndef STALEMATE_H
#define STALEMATE_H

#include "move_validation.h"
#include "iostream"

bool pawnHasLegalMoves(int pawnPosition, int ownKingPosition) {
    Piece pawn = BIT_BOARD[pawnPosition];
    int direction = (pawn & WHITE_PAWN) ? 1 : -1;
    const int pawnMoveOffsets[4] = {8, 16, 7, 9};

    for (int offset: pawnMoveOffsets) {
        if (isLegalMove(pawnPosition, pawnPosition + offset * direction, ownKingPosition)) {
            return true;
        }
    }
    return false;
}

bool knightHasLegalMoves(int knightPosition, int ownKingPosition) {
    const int knightMoveOffsets[8] = {17, -17, 15, -15, 10, -10, 6, -6};

    for (int offset: knightMoveOffsets) {
        if (isLegalMove(knightPosition, knightPosition + offset, ownKingPosition)) {
            return true;
        }
    }
    return false;
}

bool rookHasLegalMoves(int rookPosition, int ownKingPosition) {
    int rookFile = rookPosition % 8;
    int rookRank = rookPosition / 8;
    for (int i = 0; i < 8; i++) {
        // Check legal moves along the file
        if (isLegalMove(rookPosition, rookRank * 8 + i, ownKingPosition)) {
            return true;
        }
        // Check legal moves along the rank
        if (isLegalMove(rookPosition, i * 8 + rookFile, ownKingPosition)) {
            return true;
        }
    }
    return false;
}

bool bishopHasLegalMoves(int bishopPosition, int ownKingPosition) {
    // Check legal moves along the first diagonal (upward)
    for (int i = 1; i < 8; i++) {
        int testSquare = bishopPosition + i * 9;
        if (!isOnTheSameFirstDiagonal(bishopPosition, testSquare) || !withinTheBoard(testSquare)) {
            break;
        }
        if (isLegalMove(bishopPosition, testSquare, ownKingPosition)) {
            return true;
        }
    }

    // Check legal moves along the first diagonal (downward)
    for (int i = 1; i < 8; i++) {
        int testSquare = bishopPosition - i * 9;
        if (!isOnTheSameFirstDiagonal(bishopPosition, testSquare) || !withinTheBoard(testSquare)) {
            break;
        }
        if (isLegalMove(bishopPosition, testSquare, ownKingPosition)) {
            return true;
        }
    }

    // Check legal moves along the second diagonal (upward)
    for (int i = 1; i < 8; i++) {
        int testSquare = bishopPosition + i * 7;
        if (!isOnTheSameSecondDiagonal(bishopPosition, testSquare) || !withinTheBoard(testSquare)) {
            break;
        }
        if (isLegalMove(bishopPosition, testSquare, ownKingPosition)) {
            return true;
        }
    }

    // Check legal moves along the second diagonal (downward)
    for (int i = 1; i < 8; i++) {
        int testSquare = bishopPosition - i * 7;
        if (!isOnTheSameSecondDiagonal(bishopPosition, testSquare) || !withinTheBoard(testSquare)) {
            break;
        }
        if (isLegalMove(bishopPosition, testSquare, ownKingPosition)) {
            return true;
        }
    }
    return false;
}

bool pieceHasLegalMoves(int piecePosition) {
    int ownKingPosition = whiteKingSquare;
    Piece piece = BIT_BOARD[piecePosition];
    if (piece & BLACK) {
        ownKingPosition = blackKingSquare;
    }
    switch(piece & 252) {
        case PAWN:
            return pawnHasLegalMoves(piecePosition, ownKingPosition);
        case KNIGHT:
            return knightHasLegalMoves(piecePosition, ownKingPosition);
        case BISHOP:
            return bishopHasLegalMoves(piecePosition, ownKingPosition);
        case ROOK:
            return rookHasLegalMoves(piecePosition, ownKingPosition);
        case QUEEN:
            return rookHasLegalMoves(piecePosition, ownKingPosition) || bishopHasLegalMoves(piecePosition, ownKingPosition);
        case KING:
            return kingHasLegalMoves(piecePosition);
        case EMPTY:
            return false;
    }
}

bool isStalemate(int kingPosition) {
    int kingColor = BIT_BOARD[kingPosition] & 3;
    // if the king is in check or has legal moves, therefore it can't be stalemate
    if (isKingInCheck(kingPosition) || kingHasLegalMoves(kingPosition)) {
        return false;
    }

    // check whether other pieces have legal moves
    for (int i = 0; i < 64; i++) {
        Piece piece = BIT_BOARD[i];
        int pieceColor = BIT_BOARD[i] & 3;
        // we will skip iteration if current piece is the king, square is empty or piece belongs to the opposite color
        if (i == kingPosition || !(pieceColor & kingColor) || piece == EMPTY) {
            continue;
        }
        // Check if the current piece has legal moves
        if (pieceHasLegalMoves(i)) {
            std::cout << i << " - " << piece << "\n";
            return false; // If any piece has legal moves, it's not stalemate
        }
    }
    // If no piece has legal moves, it is stalemate
    return true;
}

#endif
