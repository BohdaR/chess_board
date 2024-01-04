#ifndef DISAMBIGUATION_H
#define DISAMBIGUATION_H

#include "constants.h"
#include "move_validation.h"
#include "helpers.h"

enum lookupDirection {
    UP = 1,
    DOWN = -1
};

const int LOOKUP_DIRECTIONS[] = {UP, DOWN};

enum lineDirection {
    CHESS_FILE = 1,
    RANK = 2,
    FIRST_DIAGONAL = 4, // diagonal such as, a1-h8
    SECOND_DIAGONAL = 8 // diagonal such as, h1-a8
};

enum lookupOffset {
    FILE_OFFSET = 8,
    RANK_OFFSET = 1,
    FIRST_DIAGONAL_OFFSET = 9,
    SECOND_DIAGONAL_OFFSET = 7
};

enum disambiguation {
    FILE_DISAMBIGUATION = 1,
    RANK_DISAMBIGUATION = 2,
    FULL_DISAMBIGUATION = 3,
    NO_DISAMBIGUATION = 0
};

const int findClosestPiece(int direction, int lookupDirection, int square) {
    // This function return the closest piece square on the same line e.g. on the same file

    const int loopEnd = 8;
    int offset;

    switch (direction) {
        case CHESS_FILE:
            offset = FILE_OFFSET;
            break;
        case RANK:
            offset = RANK_OFFSET;
            break;
        case FIRST_DIAGONAL:
            offset = FIRST_DIAGONAL_OFFSET;
            break;
        case SECOND_DIAGONAL:
            offset = SECOND_DIAGONAL_OFFSET;
            break;
        default:
            // Handle unexpected direction
            return -1;
    }

    for (int i = 1; i < loopEnd; i++) {
        int testSquare = square + i * lookupDirection * offset;

        if (!withinTheBoard(testSquare)) {
            break;
        }

        // In case of finding a piece we return the square, otherwise we return -1
        if (BIT_BOARD[testSquare]) {
            return testSquare;
        }
    }

    return -1;
}

int getSlidePieceMoveDisambiguation(int fromSquare, int toSquare) {
    Piece piece = BIT_BOARD[fromSquare];
    bool rankDisambiguated = false;
    bool fileDisambiguated = false;

    int lookupLinesSliceStart, lookupLinesSliceEnd;
    int lookupLines[] = {CHESS_FILE, RANK, FIRST_DIAGONAL, SECOND_DIAGONAL};

    int disambiguation = NO_DISAMBIGUATION;

    switch (piece & 252) {
        case ROOK:
            lookupLinesSliceStart = 0;
            lookupLinesSliceEnd = 2;
            break;
        case BISHOP:
            lookupLinesSliceStart = 2;
            lookupLinesSliceEnd = 4;
            break;
        case QUEEN:
            lookupLinesSliceStart = 0;
            lookupLinesSliceEnd = 4;
            break;
        default:
            return NO_DISAMBIGUATION;
    }

    for (int i = lookupLinesSliceStart; i < lookupLinesSliceEnd; i++) {
        int line = lookupLines[i];
        for (int direction: LOOKUP_DIRECTIONS) {
            int closestPieceSquare = findClosestPiece(line, direction, toSquare);
            // We skip the iteration if found piece is our moved piece or piece is not found
            if (!withinTheBoard(closestPieceSquare) || fromSquare == closestPieceSquare ||
                BIT_BOARD[closestPieceSquare] != piece ||
                !isLegalMove(closestPieceSquare, toSquare, getOwnKingSquare())) {
                continue;
            }

            if (isOnTheSameFile(fromSquare, closestPieceSquare) && !fileDisambiguated) {
                fileDisambiguated = true;
            }
            else if (!rankDisambiguated) {
                rankDisambiguated = true;
            }

            // We return full disambiguation if both rank and file disambiguations are required
            if (rankDisambiguated && fileDisambiguated) return FULL_DISAMBIGUATION;
        }
    }
    if (fileDisambiguated) disambiguation = disambiguation + FILE_DISAMBIGUATION;
    if (rankDisambiguated) disambiguation = disambiguation + RANK_DISAMBIGUATION;

    return disambiguation;
}

int getKnightMoveDisambiguation(int fromSquare, int toSquare) {
    Piece piece = BIT_BOARD[fromSquare];
    bool rankDisambiguated = false;
    bool fileDisambiguated = false;
    const int knightMoveOffsets[] = {17, -17, 15, -15, 10, -10, 6, -6};

    int disambiguation = NO_DISAMBIGUATION;

    for (int offset: knightMoveOffsets) {
        int testSquare = toSquare + offset;
        if (!withinTheBoard(testSquare) || fromSquare == testSquare ||
            BIT_BOARD[testSquare] != piece || BIT_BOARD[testSquare] == EMPTY ||
            !isLegalMove(testSquare, toSquare, getOwnKingSquare())) {
            continue;
        }

        if (isOnTheSameFile(fromSquare, testSquare) && !fileDisambiguated) {
            fileDisambiguated = true;
        }
        else if (!rankDisambiguated) {
            rankDisambiguated = true;
        }

        // We return full disambiguation if both rank and file disambiguations are required
        if (rankDisambiguated && fileDisambiguated) return FULL_DISAMBIGUATION;
    }

    if (fileDisambiguated) disambiguation = disambiguation + FILE_DISAMBIGUATION;
    if (rankDisambiguated) disambiguation = disambiguation + RANK_DISAMBIGUATION;

    return disambiguation;
}

int getDisambiguation(int fromSquare, int toSquare) {
    // This function needs to disambiguate a move if two or more pieces can move to the same square
    // and returns disambiguation notation e.g. "b" for Nbd1
    Piece piece = BIT_BOARD[fromSquare];
    switch (piece & 252) {
        case KNIGHT:
            return getKnightMoveDisambiguation(fromSquare, toSquare);
        case KING:
            return NO_DISAMBIGUATION;
        case EMPTY:
            return NO_DISAMBIGUATION;
        default:
            return getSlidePieceMoveDisambiguation(fromSquare, toSquare);
    }
}

String getDisambiguationNotation(int disambiguation, int fromSquare) {
    switch (disambiguation) {
        case FILE_DISAMBIGUATION:
            return String(getRankNumber(fromSquare));
        case RANK_DISAMBIGUATION:
            return String(getFileLetter(fromSquare));
        case FULL_DISAMBIGUATION:
            return String(getFileLetter(fromSquare)) + String(getFileLetter(fromSquare));
        default:
            return "";
    }
}

#endif
