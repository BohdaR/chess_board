#include "iostream"

//Mux control pins
int s0 = 7;
int s1 = 8;
int s2 = 9;
int s3 = 10;
int s4 = 11;
int s5 = 12;

//Mux in "SIG" pin
int SIG_pin = 1;

enum Piece {
    EMPTY = 0,
    PAWN = 4,
    KNIGHT = 8,
    BISHOP = 16,
    ROOK = 32,
    QUEEN = 64,
    KING = 128,

    WHITE_PAWN = 5,
    WHITE_KNIGHT = 9,
    WHITE_BISHOP = 17,
    WHITE_ROOK = 33,
    WHITE_QUEEN = 65,
    WHITE_KING = 129,

    BLACK_PAWN = 6,
    BLACK_KNIGHT = 10,
    BLACK_BISHOP = 18,
    BLACK_ROOK = 34,
    BLACK_QUEEN = 66,
    BLACK_KING = 130
};

enum Color {
    WHITE = 1,
    BLACK = 2
};

enum MoveType {
    MOVE = 1,
    CAPTURE = 2,
    CHECK = 4,
    CHECKMATE = 8,
    PROMOTION = 16,
    LONG_CASTLING = 32,
    SHORT_CASTLING = 64
};

struct MOVE_TYPES {
    bool capture = false;
    bool check = false;
    bool checkmate = false;
    bool promotion = false;
};

enum KING_CONSTANT {
    WHITE_CASTLING_RIGHTS = 12,
    BLACK_CASTLING_RIGHTS = 3,
    FULL_CASTLING_RIGHTS = WHITE_CASTLING_RIGHTS + BLACK_CASTLING_RIGHTS,

    WHITE_KING_START_SQUARE = 4,
    BLACK_KING_START_SQUARE = 60,
};

int whiteKingSquare = WHITE_KING_START_SQUARE;
int blackKingSquare = BLACK_KING_START_SQUARE;

struct {
    int whoseMove = WHITE; // 1 for white, 2 for black
    int pickedSquare = -1;
    int enPassantSquare = -1;
    int attackedPieceSquare = -1;
    int castlingRights = FULL_CASTLING_RIGHTS; // binary representation of castling rights
    MOVE_TYPES moveTypes;
} PositionDynamics;

Piece BIT_BOARD[64] = {
        WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK,
        WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN,
        BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK
};

char getFileLetter(int squareIndex) {
    char fileLetter = 'a' + squareIndex % 8;
    return fileLetter;
}

int getRankNumber(int squareIndex) {
    return squareIndex / 8 + 1;
}

void clearSquare(int squareIndex) {
    BIT_BOARD[squareIndex] = EMPTY;
}

void updatePosition(int fromSquare, int toSquare) {
    int movedPiece = BIT_BOARD[fromSquare];
    clearSquare(fromSquare);
    BIT_BOARD[toSquare] = static_cast<Piece>(movedPiece);
}

void capturePiece(int attackingPiece, int capturedPiece) {
    updatePosition(attackingPiece, capturedPiece);

}

bool withingTheBoard(int square) {
    if (square < 0 || square > 63) {
        return false;
    }
    return true;
}

void castle() {

}

int getSquareColor(int square) {
    // return 0 for black square and 1 for white square
    return (square % 8 + square / 8) % 2;
}

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

// move validation

bool isPseudoLegalKnightMove(int fromSquare, int toSquare) {
    int fromRank = fromSquare / 8;
    int toRank = toSquare / 8;
    int fromFile = fromSquare % 8;
    int toFile = toSquare % 8;

    int rankDiff = abs(fromRank - toRank);
    int fileDiff = abs(fromFile - toFile);

    // Check if the destination square is reachable from the source square
    if ((rankDiff == 2 && fileDiff == 1) || (rankDiff == 1 && fileDiff == 2)) {
        return withingTheBoard(toSquare);
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
        return withingTheBoard(toSquare);
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
    if (!withingTheBoard(fromSquare)) {
        return false;
    }

    if (!withingTheBoard(toSquare)) {
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
    // check if on the target square a piece of the same color
    if ((BIT_BOARD[pieceSquare] & 3) == (BIT_BOARD[kingSquare] & 3) || pieceSquare > 64) {
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

// new functionality

struct CheckInformation {
    bool check = false;
    int checksNumber = 0;
    int checkingPieceSquare = -1;
    bool checkmate = false;
    bool rankCheck = false;
    bool fileCheck = false;
    bool firstDiagonalCheck = false;
    bool secondDiagonalCheck = false;
    bool knightCheck = false;
};

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
    Piece fromSquarePiece = BIT_BOARD[fromSquare];
    Piece toSquarePiece = BIT_BOARD[toSquare];
    BIT_BOARD[fromSquare] = EMPTY;
    BIT_BOARD[toSquare] = fromSquarePiece;
    if (isPseudoLegalMove(fromSquare, toSquare)) {
        BIT_BOARD[fromSquare] = fromSquarePiece;
        BIT_BOARD[toSquare] = toSquarePiece;
        return isKingInCheck(ownKingSquare);
    }
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
    for (int offset: kingMoveOffsets) {
        int testPosition = kingPosition + offset;
        Piece testPiece = BIT_BOARD[testPosition];
        if (testPosition < 0 || testPosition > 64) {
            continue;
        }
        if (isPseudoLegalMove(kingPosition, testPosition)) {
            updatePosition(kingPosition, testPosition);
            if (!(isKingInCheck(testPosition))) {
                updatePosition(testPosition, kingPosition);
                BIT_BOARD[testPosition] = testPiece;
                return true;
            }
            updatePosition(testPosition, kingPosition);
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

            if (checkInformation.knightCheck && isPseudoLegalMove(i, checkInformation.checkingPieceSquare)) {
                return false;
            }
        }
    }

    return true;
}

bool isRankCheck(int kingPosition) {
    int kingColor = BIT_BOARD[kingPosition] & 3;
    int kingFile = kingPosition % 8;

    const int UP = 1;
    const int DOWN = -1;
    const int DIRECTIONS[2] = {UP, DOWN};

    for (int direction: DIRECTIONS) {
        int loopEnd = (direction == DOWN) ? kingFile + 1 : 8 - kingFile;
        for (int i = 1; i < loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction;

            if (BIT_BOARD[enemyPiecePosition]) {
                if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                    if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                        return true;
                    }
                }
                break;  // Break even if it's the same color
            }
        }
    }
    return false;
}

bool isFileCheck(int kingPosition) {
    int kingColor = BIT_BOARD[kingPosition] & 3;
    int kingRank = kingPosition / 8;

    const int UP = 1;
    const int DOWN = -1;
    const int DIRECTIONS[] = {UP, DOWN};

    for (int direction: DIRECTIONS) {
        int loopEnd = (direction == DOWN) ? kingRank + 1 : 8 - kingRank;
        for (int i = 1; i < loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction * 8;

            if (BIT_BOARD[enemyPiecePosition]) {
                if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                    if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                        return true;
                    }
                }
                break;  // Break even if it's the same color
            }
        }
    }
    return false;
}

bool isFirstDiagonalCheck(int kingPosition) {
    int kingColor = BIT_BOARD[kingPosition] & 3;
    int kingFile = kingPosition % 8;
    int kingRank = kingPosition / 8;

    const int UP = 1;
    const int DOWN = -1;
    const int DIRECTIONS[] = {UP, DOWN};

    for (int direction: DIRECTIONS) {
        int upLoopEnd = (kingFile > kingRank) ? kingFile : kingRank;
        int downLoopEnd = 8 - ((kingFile < kingRank) ? kingFile : kingRank);
        int loopEnd = (direction == DOWN) ? downLoopEnd : upLoopEnd;
        for (int i = 1; i <= loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction * 9;

            if (BIT_BOARD[enemyPiecePosition]) {
                if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                    if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                        return true;
                    }
                }
                break;  // Break even if it's the same color
            }
        }
    }
    return false;
}

bool isSecondDiagonalCheck(int kingPosition) {
    int kingColor = BIT_BOARD[kingPosition] & 3;
    int kingFile = kingPosition % 8;
    int kingRank = kingPosition / 8;

    const int UP = 1;
    const int DOWN = -1;
    const int DIRECTIONS[] = {UP, DOWN};

    for (int direction: DIRECTIONS) {
        int upLoopEnd = (kingFile < kingRank) ? kingFile : kingRank;
        int downLoopEnd = 8 - ((kingFile > kingRank) ? kingFile : kingRank);
        int loopEnd = (direction == DOWN) ? upLoopEnd : downLoopEnd;
        for (int i = 1; i <= loopEnd; i++) {
            int enemyPiecePosition = kingPosition + i * direction * 7;

            if (BIT_BOARD[enemyPiecePosition]) {
                if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                    if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                        return true;
                    }
                }
                break;  // Break even if it's the same color
            }
        }
    }
    return false;
}

bool isKnightCheck(int kingPosition) {
    int kingColor = BIT_BOARD[kingPosition] & 3;
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
                    return true;
                }
            }
        }
    }
    return false;
}

int main() {
    whiteKingSquare = 4;
    updatePosition(13, 14);
    updatePosition(15, 14);
    updatePosition(0, 12);
    updatePosition(56, 20);
    updatePosition(59, 22);

//    std::cout << isPseudoLegalKingMove(7, 0);
    std::cout << isCheckMate(whiteKingSquare);
//    std::cout << isCheckMate(whiteKingSquare);
}
