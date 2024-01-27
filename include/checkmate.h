#ifndef CHECKMATE_H
#define CHECKMATE_H

#include "constants.h"
#include "move_validation.h"

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
        if (withinTheBoard(firstIntersectionSquare)) {
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

        if (withinTheBoard(secondIntersectionSquare)) {
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
        if (withinTheBoard(firstIntersectionSquare)) {
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

        if (withinTheBoard(secondIntersectionSquare)) {
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
        if (withinTheBoard(firstIntersectionSquare)) {
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
        if (withinTheBoard(secondIntersectionSquare)) {
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
        if (withinTheBoard(intersectionSquare)) {
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
        if (withinTheBoard(firstIntersectionSquare) &&
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
        if (withinTheBoard(secondIntersectionSquare) &&
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
                pieceSquare + (ranksBetweenAttackerAndDefender + filesBetweenAttackerAndDefender) * 8;
        int secondIntersectionSquare = pieceSquare + filesBetweenAttackerAndDefender - ranksBetweenAttackerAndDefender;
        if (attackingPieceSquare < kingSquare) {
            firstIntersectionSquare =
                    pieceSquare + (ranksBetweenAttackerAndDefender - filesBetweenAttackerAndDefender) * 8;
            secondIntersectionSquare = pieceSquare + filesBetweenAttackerAndDefender + ranksBetweenAttackerAndDefender;
        }

        if (withinTheBoard(firstIntersectionSquare)) {
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
        if (withinTheBoard(secondIntersectionSquare)) {
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
        if (withinTheBoard(intersectionSquare)) {
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
        if (withinTheBoard(firstIntersectionSquare) &&
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
        if (withinTheBoard(secondIntersectionSquare) &&
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
