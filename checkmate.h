#ifndef CHECKMATE_H
#define CHECKMATE_H

#include "constants.h"
#include "move_validation.h"

bool canAttackTheKing(int pieceSquare, int kingSquare) {
    if (!withingTheBoard(pieceSquare)) {
        return false;
    }

    if (!withingTheBoard(kingSquare)) {
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
    int kingFile = kingPosition % 8;
    int kingRank = kingPosition / 8;

    const int UP = 1;
    const int DOWN = -1;
    const int DIRECTIONS[] = {UP, DOWN};

    // Check along the same rank (left and right)
    for (int direction: DIRECTIONS) {
        int loopEnd = (direction == DOWN) ? kingFile + 1 : 8 - kingFile;
        for (int i = 1; i < loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction;

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
        int loopEnd = (direction == DOWN) ? kingRank + 1 : 8 - kingRank;
        for (int i = 1; i < loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction * 8;

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
        int upLoopEnd = (kingFile > kingRank) ? kingFile : kingRank;
        int downLoopEnd = 8 - ((kingFile < kingRank) ? kingFile : kingRank);
        int loopEnd = (direction == DOWN) ? downLoopEnd : upLoopEnd;
        for (int i = 1; i <= loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction * 9;

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
        int upLoopEnd = (kingFile < kingRank) ? kingFile : kingRank;
        int downLoopEnd = 8 - ((kingFile > kingRank) ? kingFile : kingRank);
        int loopEnd = (direction == DOWN) ? upLoopEnd : downLoopEnd;
        for (int i = 1; i <= loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction * 7;

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
        if (!withingTheBoard(enemyPiecePosition)) {
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
    bool isLegal = false;
    Piece fromSquarePiece = BIT_BOARD[fromSquare];
    Piece toSquarePiece = BIT_BOARD[toSquare];
    if (isPseudoLegalMove(fromSquare, toSquare)) {
        BIT_BOARD[fromSquare] = EMPTY;
        BIT_BOARD[toSquare] = fromSquarePiece;
        isLegal = !isKingInCheck(ownKingSquare);
    }
    BIT_BOARD[fromSquare] = fromSquarePiece;
    BIT_BOARD[toSquare] = toSquarePiece;
    return isLegal;  // The move is not legal
}

bool canBlockFileCheck(int pieceSquare, int kingSquare, int attackingPieceSquare) {
    int pieceDefender = BIT_BOARD[pieceSquare] & 252;
    int defenderRank = pieceSquare / 8;
    int defenderFile = pieceSquare % 8;

    int kingRank = kingSquare / 8;
    int kingFile = kingSquare % 8;

    int attackerRank = attackingPieceSquare / 8;
    int attackerFile = attackingPieceSquare % 8;

    int filesBetweenAttackerAndDefender = attackerFile - defenderFile;

    if (pieceDefender & (ROOK + QUEEN)) {
        int intersectionSquare = pieceSquare + filesBetweenAttackerAndDefender;
        if (attackingPieceSquare > kingSquare) {
            if (intersectionSquare <= attackingPieceSquare && intersectionSquare > kingSquare &&
                isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                return true;
            }
        } else {
            if (intersectionSquare >= attackingPieceSquare && intersectionSquare < kingSquare &&
                isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                return true;
            }
        }
    }
    if (pieceDefender & (BISHOP + QUEEN)) {
        int firstIntersectionSquare =
                pieceSquare + filesBetweenAttackerAndDefender + (filesBetweenAttackerAndDefender * 8);
        int secondIntersectionSquare = firstIntersectionSquare - (2 * filesBetweenAttackerAndDefender * 8);
        if (withingTheBoard(firstIntersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (firstIntersectionSquare <= attackingPieceSquare && firstIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (firstIntersectionSquare >= attackingPieceSquare && firstIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }

        if (withingTheBoard(secondIntersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (secondIntersectionSquare <= attackingPieceSquare && secondIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (secondIntersectionSquare >= attackingPieceSquare && secondIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
    }
    if (pieceDefender & KNIGHT) {
        const int knightMoveOffsets[] = {17, -17, 15, -15, 10, -10, 6, -6};
        int intersectionSquare;

        for (int offset: knightMoveOffsets) {
            intersectionSquare = pieceSquare + offset;
            if (attackerFile == intersectionSquare % 8 && isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                if (attackingPieceSquare > kingSquare) {
                    if (intersectionSquare <= attackingPieceSquare && intersectionSquare > kingSquare) {
                        return true;
                    }
                } else {
                    if (intersectionSquare >= attackingPieceSquare && intersectionSquare < kingSquare) {
                        return true;
                    }
                }
            }
        }
    }
    if (pieceDefender & PAWN && isLegalMove(pieceSquare, attackingPieceSquare, kingSquare)) {
        return true;
    }
    return false;
}

bool canBlockRankCheck(int pieceSquare, int kingSquare, int attackingPieceSquare) {
    int pieceDefender = BIT_BOARD[pieceSquare] & 252;
    int defenderRank = pieceSquare / 8;
    int defenderFile = pieceSquare % 8;

    int kingRank = kingSquare / 8;
    int kingFile = kingSquare % 8;

    int attackerRank = attackingPieceSquare / 8;
    int attackerFile = attackingPieceSquare % 8;

    int ranksBetweenAttackerAndDefender = attackerRank - defenderRank;

    if (pieceDefender & (ROOK + QUEEN)) {
        int intersectionSquare = pieceSquare + ranksBetweenAttackerAndDefender * 8;
        if (attackingPieceSquare > kingSquare) {
            if (intersectionSquare <= attackingPieceSquare && intersectionSquare > kingSquare &&
                isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                return true;
            }
        } else {
            if (intersectionSquare >= attackingPieceSquare && intersectionSquare < kingSquare &&
                isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                return true;
            }
        }
    }
    if (pieceDefender & (BISHOP + QUEEN)) {
        int firstIntersectionSquare =
                pieceSquare + ranksBetweenAttackerAndDefender + ranksBetweenAttackerAndDefender * 8;
        int secondIntersectionSquare = firstIntersectionSquare - 2 * ranksBetweenAttackerAndDefender;
        if (withingTheBoard(firstIntersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (firstIntersectionSquare <= attackingPieceSquare && firstIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (firstIntersectionSquare >= attackingPieceSquare && firstIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }

        if (withingTheBoard(secondIntersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (secondIntersectionSquare <= attackingPieceSquare && secondIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (secondIntersectionSquare >= attackingPieceSquare && secondIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
    }
    if (pieceDefender & KNIGHT) {
        const int knightMoveOffsets[] = {17, -17, 15, -15, 10, -10, 6, -6};
        int intersectionSquare;

        for (int offset: knightMoveOffsets) {
            intersectionSquare = pieceSquare + offset;
            if (attackerRank == intersectionSquare / 8 && isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                if (attackingPieceSquare > kingSquare) {
                    if (intersectionSquare <= attackingPieceSquare && intersectionSquare > kingSquare) {
                        return true;
                    }
                } else {
                    if (intersectionSquare >= attackingPieceSquare && intersectionSquare < kingSquare) {
                        return true;
                    }
                }
            }
        }
    }
    if (pieceDefender & PAWN) {
        int intersectionSquare = pieceSquare + 8 * ranksBetweenAttackerAndDefender;
        if (isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (intersectionSquare < attackingPieceSquare && intersectionSquare > kingSquare) {
                    return true;
                }
            } else {
                if (intersectionSquare > attackingPieceSquare && intersectionSquare < kingSquare) {
                    return true;
                }
            }
        }
        if (isLegalMove(pieceSquare, attackingPieceSquare, kingSquare)) {
            return true;
        }
    }

    return false;
}

bool canBlockFirstDiagonalCheck(int pieceSquare, int kingSquare, int attackingPieceSquare) {
    int pieceDefender = BIT_BOARD[pieceSquare] & 252;
    int defenderRank = pieceSquare / 8;
    int defenderFile = pieceSquare % 8;

    int kingRank = kingSquare / 8;
    int kingFile = kingSquare % 8;

    int attackerRank = attackingPieceSquare / 8;
    int attackerFile = attackingPieceSquare % 8;

    int ranksBetweenAttackerAndDefender = attackerRank - defenderRank;
    int filesBetweenAttackerAndDefender = attackerFile - defenderFile;

    if (pieceDefender & (ROOK + QUEEN)) {
        int firstIntersectionSquare =
                pieceSquare + (ranksBetweenAttackerAndDefender - filesBetweenAttackerAndDefender) * 8;
        int secondIntersectionSquare = pieceSquare + filesBetweenAttackerAndDefender - ranksBetweenAttackerAndDefender;
        if (withingTheBoard(firstIntersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (firstIntersectionSquare <= attackingPieceSquare && firstIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (firstIntersectionSquare >= attackingPieceSquare && firstIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
        if (withingTheBoard(secondIntersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (secondIntersectionSquare <= attackingPieceSquare && secondIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (secondIntersectionSquare >= attackingPieceSquare && secondIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
    }
    if (pieceDefender & (BISHOP + QUEEN)) {
        // opposite color bishop/queen can not block the check
        if ((defenderFile + defenderRank) % 2 != (attackerFile + attackerRank) % 2) {
            return false;
        }
        int intersectionSquare =
                pieceSquare - (filesBetweenAttackerAndDefender - ranksBetweenAttackerAndDefender) / 2 * 7;
        if (withingTheBoard(intersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (intersectionSquare <= attackingPieceSquare && intersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (intersectionSquare >= attackingPieceSquare && intersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
    }
    if (pieceDefender & KNIGHT) {
        const int knightMoveOffsets[] = {17, -17, 15, -15, 10, -10, 6, -6};
        int intersectionSquare;

        for (int offset: knightMoveOffsets) {
            intersectionSquare = pieceSquare + offset;
            if (isOnTheSameFirstDiagonal(intersectionSquare, attackingPieceSquare) &&
                isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                if (attackingPieceSquare > kingSquare) {
                    if (intersectionSquare <= attackingPieceSquare && intersectionSquare > kingSquare) {
                        return true;
                    }
                } else {
                    if (intersectionSquare >= attackingPieceSquare && intersectionSquare < kingSquare) {
                        return true;
                    }
                }
            }
        }
    }
    if (pieceDefender & PAWN) {
        int firstIntersectionSquare = pieceSquare + 8;
        int secondIntersectionSquare = pieceSquare + 8 * 2;
        if (BIT_BOARD[pieceSquare] == BLACK_PAWN) {
            firstIntersectionSquare = pieceSquare - 8;
            secondIntersectionSquare = pieceSquare - 8 * 2;
        }
        if (withingTheBoard(firstIntersectionSquare) &&
            isOnTheSameFirstDiagonal(firstIntersectionSquare, attackingPieceSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (firstIntersectionSquare < attackingPieceSquare && firstIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (firstIntersectionSquare > attackingPieceSquare && firstIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
        if (withingTheBoard(secondIntersectionSquare) &&
            isOnTheSameFirstDiagonal(secondIntersectionSquare, attackingPieceSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (secondIntersectionSquare < attackingPieceSquare && secondIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (secondIntersectionSquare > attackingPieceSquare && secondIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
        if (isLegalMove(pieceSquare, attackingPieceSquare, kingSquare)) {
            return true;
        }
    }

    return false;
}

bool canBlockSecondDiagonalCheck(int pieceSquare, int kingSquare, int attackingPieceSquare) {
    int pieceDefender = BIT_BOARD[pieceSquare] & 252;
    int defenderRank = pieceSquare / 8;
    int defenderFile = pieceSquare % 8;

    int kingRank = kingSquare / 8;
    int kingFile = kingSquare % 8;

    int attackerRank = attackingPieceSquare / 8;
    int attackerFile = attackingPieceSquare % 8;

    int ranksBetweenAttackerAndDefender = attackerRank - defenderRank;
    int filesBetweenAttackerAndDefender = attackerFile - defenderFile;

    if (pieceDefender & (ROOK + QUEEN)) {
        int firstIntersectionSquare =
                pieceSquare + (ranksBetweenAttackerAndDefender - filesBetweenAttackerAndDefender) * 8;
        int secondIntersectionSquare = pieceSquare + filesBetweenAttackerAndDefender - ranksBetweenAttackerAndDefender;
        if (attackingPieceSquare < kingSquare) {
            firstIntersectionSquare =
                    pieceSquare + (ranksBetweenAttackerAndDefender + filesBetweenAttackerAndDefender) * 8;
            secondIntersectionSquare = pieceSquare + filesBetweenAttackerAndDefender + ranksBetweenAttackerAndDefender;
        }

        if (withingTheBoard(firstIntersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (firstIntersectionSquare <= attackingPieceSquare && firstIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (firstIntersectionSquare >= attackingPieceSquare && firstIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
        if (withingTheBoard(secondIntersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (secondIntersectionSquare <= attackingPieceSquare && secondIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (secondIntersectionSquare >= attackingPieceSquare && secondIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
    }
    if (pieceDefender & (BISHOP + QUEEN)) {
        // opposite color bishop/queen can not block the check
        if ((defenderFile + defenderRank) % 2 != (attackerFile + attackerRank) % 2) {
            return false;
        }
        int intersectionSquare =
                pieceSquare - (filesBetweenAttackerAndDefender - ranksBetweenAttackerAndDefender) / 2 * 7;
        if (withingTheBoard(intersectionSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (intersectionSquare <= attackingPieceSquare && intersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (intersectionSquare >= attackingPieceSquare && intersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
    }
    if (pieceDefender & KNIGHT) {
        const int knightMoveOffsets[] = {17, -17, 15, -15, 10, -10, 6, -6};
        int intersectionSquare;

        for (int offset: knightMoveOffsets) {
            intersectionSquare = pieceSquare + offset;
            if (isOnTheSameSecondDiagonal(intersectionSquare, attackingPieceSquare) &&
                isLegalMove(pieceSquare, intersectionSquare, kingSquare)) {
                if (attackingPieceSquare > kingSquare) {
                    if (intersectionSquare <= attackingPieceSquare && intersectionSquare > kingSquare) {
                        return true;
                    }
                } else {
                    if (intersectionSquare >= attackingPieceSquare && intersectionSquare < kingSquare) {
                        return true;
                    }
                }
            }
        }
    }
    if (pieceDefender & PAWN) {
        int firstIntersectionSquare = pieceSquare + 8;
        int secondIntersectionSquare = pieceSquare + 8 * 2;
        if (BIT_BOARD[pieceSquare] == BLACK_PAWN) {
            firstIntersectionSquare = pieceSquare - 8;
            secondIntersectionSquare = pieceSquare - 8 * 2;
        }
        if (withingTheBoard(firstIntersectionSquare) &&
            isOnTheSameSecondDiagonal(firstIntersectionSquare, attackingPieceSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (firstIntersectionSquare < attackingPieceSquare && firstIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (firstIntersectionSquare > attackingPieceSquare && firstIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, firstIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
        if (withingTheBoard(secondIntersectionSquare) &&
            isOnTheSameSecondDiagonal(secondIntersectionSquare, attackingPieceSquare)) {
            if (attackingPieceSquare > kingSquare) {
                if (secondIntersectionSquare < attackingPieceSquare && secondIntersectionSquare > kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            } else {
                if (secondIntersectionSquare > attackingPieceSquare && secondIntersectionSquare < kingSquare &&
                    isLegalMove(pieceSquare, secondIntersectionSquare, kingSquare)) {
                    return true;
                }
            }
        }
        if (isLegalMove(pieceSquare, attackingPieceSquare, kingSquare)) {
            return true;
        }
    }

    return false;
}

bool kingHasLegalMoves(int kingPosition) {
    const int kingMoveOffsets[] = {1, -1, 8, -8, 7, -7, 9, -9};
    Piece king = BIT_BOARD[kingPosition];
    for (int offset: kingMoveOffsets) {
        int testPosition = kingPosition + offset;
        Piece testPiece = BIT_BOARD[testPosition];
        if (testPosition < 0 || testPosition > 64) {
            continue;
        }
        if (isPseudoLegalMove(kingPosition, testPosition)) {
            BIT_BOARD[kingPosition] = EMPTY;
            BIT_BOARD[testPosition] = king;
            if (!(isKingInCheck(testPosition))) {
                BIT_BOARD[kingPosition] = king;
                BIT_BOARD[testPosition] = testPiece;
                return true;
            }
            BIT_BOARD[kingPosition] = king;
            BIT_BOARD[testPosition] = testPiece;
        }
    }
    return false;
}

bool isCheckMate(int kingPosition) {
    int king = BIT_BOARD[kingPosition];
    int kingColor = king & 3;
    CheckInformation checkInformation = getChecksInformation(kingPosition);
    if (!(isKingInCheck(kingPosition)) || kingHasLegalMoves(kingPosition)) {
        return false;
    }
    // if double check and king has no moves it's mate
    if (checkInformation.checksNumber > 1 && !kingHasLegalMoves(kingPosition)) {
        return true;
    }

    for (int i = 0; i < 64; i++) {
        int piece = BIT_BOARD[i];
        if (piece & kingColor) {
            if (checkInformation.rankCheck && canBlockRankCheck(i, kingPosition, checkInformation.checkingPieceSquare)) {
                return false;
            }

            if (checkInformation.fileCheck && canBlockFileCheck(i, kingPosition, checkInformation.checkingPieceSquare)) {
                return false;
            }

            if (checkInformation.firstDiagonalCheck && canBlockFirstDiagonalCheck(i, kingPosition, checkInformation.checkingPieceSquare)) {
                return false;
            }

            if (checkInformation.secondDiagonalCheck && canBlockSecondDiagonalCheck(i, kingPosition, checkInformation.checkingPieceSquare)) {
                return false;
            }

            if (checkInformation.knightCheck && isLegalMove(i, checkInformation.checkingPieceSquare, kingPosition)) {
                return false;
            }
        }
    }

    return true;
}

#endif
