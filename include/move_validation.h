#ifndef MOVE_VALIDATION_H
#define MOVE_VALIDATION_H

#include "constants.h"

bool isOnTheSameDiagonal(int firstSquare, int secondSquare) {
    // check whether two pieces on the same diagonal such as a1-h8
    return abs(firstSquare / 8 - secondSquare / 8) == abs(firstSquare % 8 - secondSquare % 8);
}

bool isOnTheSameFirstDiagonal(int firstSquare, int secondSquare) {
    // check whether two pieces on the same diagonal such as a1-h8
    return isOnTheSameDiagonal(firstSquare, secondSquare) && firstSquare % 9 == secondSquare % 9;
}

bool isOnTheSameSecondDiagonal(int firstSquare, int secondSquare) {
    // check whether two pieces on the same diagonal such as h1-a8
    return isOnTheSameDiagonal(firstSquare, secondSquare) && firstSquare % 7 == secondSquare % 7;
}

bool isOnTheSameRank(int firstSquare, int secondSquare) {
    return (firstSquare / 8) == (secondSquare / 8);
}

bool isOnTheSameFile(int firstSquare, int secondSquare) {
    return (firstSquare % 8) == (secondSquare % 8);
}

bool withinTheBoard(int square) {
    if (square < 0 || square > 63) {
        return false;
    }
    return true;
}

bool isPseudoLegalKnightMove(int fromSquare, int toSquare) {
    int fromRank = fromSquare / 8;
    int toRank = toSquare / 8;
    int fromFile = fromSquare % 8;
    int toFile = toSquare % 8;

    int rankDiff = abs(fromRank - toRank);
    int fileDiff = abs(fromFile - toFile);

    // Check if the destination square is reachable from the source square
    if ((rankDiff == 2 && fileDiff == 1) || (rankDiff == 1 && fileDiff == 2)) {
        return withinTheBoard(toSquare);
    }

    // If no matching offset is found, the move is not pseudo-legal
    return false;
}

bool isPseudoLegalPawnMove(int fromSquare, int toSquare) {
    int rankDifference = (toSquare / 8) - (fromSquare / 8);
    int fileDifference = (toSquare % 8) - (fromSquare % 8);
    int target = BIT_BOARD[toSquare];
    int pawn = BIT_BOARD[fromSquare];
    if (pawn & WHITE && !target) {
        if (rankDifference == 1 && fileDifference == 0) {
            return true;  // Move one square forward
        }
        if (fromSquare / 8 == 1 && rankDifference == 2 && fileDifference == 0 && !BIT_BOARD[toSquare - 8]) {
            return true;  // Move two squares forward from the starting position
        }
    }
    if (pawn & BLACK && !target) {
        if (rankDifference == -1 && fileDifference == 0) {
            return true;  // Move one square forward
        }
        if (fromSquare / 8 == 6 && rankDifference == -2 && fileDifference == 0 && !BIT_BOARD[toSquare + 8]) {
            return true;  // Move two squares forward from the starting position
        }
    }
    if ((pawn & WHITE) && rankDifference == 1 && abs(fileDifference) == 1 &&
        ((target & BLACK) || toSquare == PositionDynamics.enPassantSquare)) {
        return true;
    }
    if ((pawn & BLACK) && rankDifference == -1 && abs(fileDifference) == 1 &&
        ((target & WHITE) || toSquare == PositionDynamics.enPassantSquare)) {
        return true;
    }
    return false;
}

bool isPseudoLegalKingMove(int fromSquare, int toSquare) {
    int fromRank = fromSquare / 8;
    int toRank = toSquare / 8;
    int fromFile = fromSquare % 8;
    int toFile = toSquare % 8;

    // Check if the destination square is reachable from the source square
    if (abs(fromRank - toRank) <= 1 && abs(fromFile - toFile) <= 1) {
        return withinTheBoard(toSquare);
    }

    // If no matching offset is found, the move is not pseudo-legal
    return false;
}

bool isPseudoLegalBishopMove(int fromSquare, int toSquare) {
    if (isOnTheSameFirstDiagonal(fromSquare, toSquare)) {
        int loopEnd = abs(fromSquare / 8 - toSquare / 8);
        int lookUpDirection = (fromSquare > toSquare) ? -1 : 1;

        for (int i = 1; i < loopEnd; i++) {
            if (BIT_BOARD[fromSquare + i * 9 * lookUpDirection]) {
                return false;
            }
        }

        return true;
    }

    if (isOnTheSameSecondDiagonal(fromSquare, toSquare)) {
        int loopEnd = abs(fromSquare / 8 - toSquare / 8);
        int lookUpDirection = (fromSquare > toSquare) ? -1 : 1;

        for (int i = 1; i < loopEnd; i++) {
            if (BIT_BOARD[fromSquare + i * 7 * lookUpDirection]) {
                return false;
            }
        }

        return true;
    }

    return false;
}

bool isPseudoLegalRookMove(int fromSquare, int toSquare) {
    if (isOnTheSameFile(fromSquare, toSquare)) {
        int rookRankIndex = fromSquare / 8;
        int targetRankIndex = toSquare / 8;

        int loopEnd = abs(rookRankIndex - targetRankIndex);
        int lookUpDirection = (fromSquare > toSquare) ? -1 : 1;

        for (int i = 1; i < loopEnd; i++) {
            if (BIT_BOARD[fromSquare + i * 8 * lookUpDirection]) {
                return false;
            }
        }
        return true;
    }

    if (isOnTheSameRank(fromSquare, toSquare)) {
        int rookFileIndex = fromSquare % 8;
        int targetFileIndex = toSquare % 8;

        int loopEnd = abs(rookFileIndex - targetFileIndex);
        int lookUpDirection = (fromSquare > toSquare) ? -1 : 1;

        for (int i = 1; i < loopEnd; i++) {
            if (BIT_BOARD[fromSquare + i * lookUpDirection]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool isPseudoLegalQueenMove(int fromSquare, int toSquare) {
    return isPseudoLegalBishopMove(fromSquare, toSquare) || isPseudoLegalRookMove(fromSquare, toSquare);
}

bool isPseudoLegalMove(int fromSquare, int toSquare) {
    if (!withinTheBoard(fromSquare)) {
        return false;
    }

    if (!withinTheBoard(toSquare)) {
        return false;
    }

    // check if on the target square a piece of the same color
    if ((BIT_BOARD[toSquare] & 3) == (BIT_BOARD[fromSquare] & 3)) {
        return false;
    }

    // Pseudo-legal moves may still be illegal if they leave the own king in check
    int piece = BIT_BOARD[fromSquare] &
                252; // here we take only 6 bit of piece 'cause first 2 bits are used for piece color
    bool isValid = false;
    switch (piece) {
        case BISHOP:
            return isPseudoLegalBishopMove(fromSquare, toSquare);
        case QUEEN:
            return isPseudoLegalQueenMove(fromSquare, toSquare);
        case ROOK:
            return isPseudoLegalRookMove(fromSquare, toSquare);
        case PAWN:
            return isPseudoLegalPawnMove(fromSquare, toSquare);
        case KNIGHT:
            return isPseudoLegalKnightMove(fromSquare, toSquare);
        case KING:
            return isPseudoLegalKingMove(fromSquare, toSquare);
    }
    return isValid;
}

bool canAttackTheKing(int pieceSquare, int kingSquare) {
    if (!withinTheBoard(pieceSquare)) {
        return false;
    }

    if (!withinTheBoard(kingSquare)) {
        return false;
    }

    // check if on the target square a piece of the same color
    if ((BIT_BOARD[pieceSquare] & 3) == (BIT_BOARD[kingSquare] & 3)) {
        return false;
    }

    int piece = BIT_BOARD[pieceSquare] &
                252; // here we take only 6 bit of piece 'cause first 2 bits are used for piece color
    int rankDifference = (kingSquare / 8) - (pieceSquare / 8);
    int fileDifference = (kingSquare % 8) - (pieceSquare % 8);

    switch (piece) {
        case BISHOP:
            return isPseudoLegalBishopMove(pieceSquare, kingSquare);
        case QUEEN:
            return isPseudoLegalQueenMove(pieceSquare, kingSquare);
        case ROOK:
            return isPseudoLegalRookMove(pieceSquare, kingSquare);
        case PAWN:
            if ((BIT_BOARD[kingSquare] & WHITE) && rankDifference == -1 && abs(fileDifference) == 1) {
                return true;
            }
            if ((BIT_BOARD[kingSquare] & BLACK) && rankDifference == 1 && abs(fileDifference) == 1) {
                return true;
            }
            break;
        case KNIGHT:
            return isPseudoLegalKnightMove(pieceSquare, kingSquare);
        case KING:
            return isPseudoLegalKingMove(pieceSquare, kingSquare);
    }
    return false;
}

CheckInformation getChecksInformation(int kingPosition) {
    CheckInformation checksInformation;
    int kingColor = BIT_BOARD[kingPosition] & 3;

    const int UP = 1;
    const int DOWN = -1;
    const int DIRECTIONS[] = {UP, DOWN};
    const int loopEnd = 7;

    // Check along the same rank (left and right)
    for (int direction: DIRECTIONS) {
        for (int i = 1; i <= loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction;

            if (!withinTheBoard(enemyPiecePosition)) {
                break;
            }

            if (BIT_BOARD[enemyPiecePosition]) {
                if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                    if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                        checksInformation.check = true;
                        checksInformation.rankCheck = true;
                        checksInformation.checksNumber += 1;
                        checksInformation.checkingPieceSquare = enemyPiecePosition;
                    }
                }
                break;  // Break even if it's the same color
            }
        }
    }

    // check if there is an enemy piece on the same file
    for (int direction: DIRECTIONS) {
        for (int i = 1; i <= loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction * 8;

            if (!withinTheBoard(enemyPiecePosition)) {
                break;
            }

            if (BIT_BOARD[enemyPiecePosition]) {
                if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                    if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                        checksInformation.check = true;
                        checksInformation.fileCheck = true;
                        checksInformation.checksNumber += 1;
                        checksInformation.checkingPieceSquare = enemyPiecePosition;
                    }
                }
                break;  // Break even if it's the same color
            }
        }
    }

    // check if there is an enemy piece on the same first diagonal
    for (int direction: DIRECTIONS) {
        for (int i = 1; i <= loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction * 9;

            if (!withinTheBoard(enemyPiecePosition) || !isOnTheSameFirstDiagonal(kingPosition, enemyPiecePosition)) {
                break;
            }

            if (BIT_BOARD[enemyPiecePosition]) {
                if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                    if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                        checksInformation.check = true;
                        checksInformation.firstDiagonalCheck = true;
                        checksInformation.checksNumber += 1;
                        checksInformation.checkingPieceSquare = enemyPiecePosition;
                    }
                }
                break;  // Break even if it's the same color
            }
        }
    }

    // check if there is an enemy piece on the same second diagonal
    for (int direction: DIRECTIONS) {
        for (int i = 1; i <= loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction * 7;
            if (!withinTheBoard(enemyPiecePosition) || !isOnTheSameSecondDiagonal(kingPosition, enemyPiecePosition)) {
                break;
            }

            if (BIT_BOARD[enemyPiecePosition]) {
                if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                    if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                        checksInformation.check = true;
                        checksInformation.secondDiagonalCheck = true;
                        checksInformation.checksNumber += 1;
                        checksInformation.checkingPieceSquare = enemyPiecePosition;
                    }
                }
                break;  // Break even if it's the same color
            }
        }
    }

    // check if there is an enemy knight to give a check
    const int knightMoveOffsets[] = {17, -17, 15, -15, 10, -10, 6, -6};

    // Check if the destination square is reachable from the source square
    for (int offset: knightMoveOffsets) {
        int enemyPiecePosition = kingPosition + offset;
        if (!withinTheBoard(enemyPiecePosition)) {
            continue;
        }
        if (BIT_BOARD[enemyPiecePosition]) {
            if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                    checksInformation.check = true;
                    checksInformation.knightCheck = true;
                    checksInformation.checksNumber += 1;
                    checksInformation.checkingPieceSquare = enemyPiecePosition;
                }
            }
        }
    }

    return checksInformation;
}

bool isKingInCheck(int kingPosition) {
    return getChecksInformation(kingPosition).check;
}

bool isLegalMove(int fromSquare, int toSquare, int ownKingSquare) {
    // Check if the squares are within the board
    if (!withinTheBoard(fromSquare) || !withinTheBoard(toSquare)) {
        return false;
    }

    // Check if the squares are the same
    if (fromSquare == toSquare) {
        return false;
    }

    // Preserve the current state of the board
    Piece fromSquarePiece = BIT_BOARD[fromSquare];
    Piece toSquarePiece = BIT_BOARD[toSquare];

    // Update the ownKingSquare if the moving piece is the own king
    if (fromSquare == ownKingSquare) {
        ownKingSquare = toSquare;
    }

    // Check if the move is pseudo-legal
    bool isPseudoLegal = isPseudoLegalMove(fromSquare, toSquare);

    // Simulate the move
    BIT_BOARD[fromSquare] = EMPTY;
    BIT_BOARD[toSquare] = fromSquarePiece;

    // Check if the move results in the own king being in check
    bool isLegal = isPseudoLegal && !isKingInCheck(ownKingSquare);

    // Restore the original state of the board
    BIT_BOARD[fromSquare] = fromSquarePiece;
    BIT_BOARD[toSquare] = toSquarePiece;

    return isLegal;
}

bool kingHasLegalMoves(int kingPosition) {
    const int kingMoveOffsets[8] = {1, -1, 8, -8, 7, -7, 9, -9};
    for (int offset: kingMoveOffsets) {
        int testPosition = kingPosition + offset;
        if (testPosition < 0 || testPosition > 64) {
            continue;
        }
        if (isLegalMove(kingPosition, testPosition, kingPosition)) {
            return true;
        }
    }
    return false;
}

#endif
