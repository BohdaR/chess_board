#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F for 16 chars and 2 line display
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

float readMux(int channel) {
    digitalWrite(s0, channel & 1);
    digitalWrite(s1, channel & 2);
    digitalWrite(s2, channel & 4);
    digitalWrite(s3, channel & 8);
    digitalWrite(s4, channel & 16);
    digitalWrite(s5, channel & 32);

    //read the value at the SIG pin
    int val = analogRead(SIG_pin);

    //return the value
    return val * 5.0 / 1023;
}

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

String getPieceLatter(int piece) {
    piece = piece & 252; // here we take only 6 bit of piece 'cause first 2 bits are used for piece color
    switch (piece) {
        case PAWN:
            return "";
        case ROOK:
            return "R";
        case KNIGHT:
            return "N";
        case BISHOP:
            return "B";
        case QUEEN:
            return "Q";
        case KING:
            return "K";
    }
}

void clearSquare(int squareIndex) {
    BIT_BOARD[squareIndex] = EMPTY;
}

void updatePosition(int fromSquare, int toSquare) {
    int movedPiece = BIT_BOARD[fromSquare];
    clearSquare(fromSquare);
    BIT_BOARD[toSquare] = movedPiece;
}

void capturePiece(int attackingPiece, int capturedPiece) {
    updatePosition(attackingPiece, capturedPiece);

}

void castle() {

}

String getSquare(int squareIndex) {
    return String(getFileLetter(squareIndex)) + String(getRankNumber(squareIndex));
}

String getMoveNotation(int fromSquare, int toSquare, MOVE_TYPES moveTypes) {
    int piece = BIT_BOARD[toSquare];
    String square = getPieceLatter(piece) + getSquare(toSquare);

    if (moveTypes.capture) {
        if (piece & PAWN) {
            char previousFileLetter = getFileLetter(fromSquare);
            square = String(previousFileLetter) + "x" + square;
        } else {
            square = String(square[0]) + "x" + square.substring(1, -1);
        }
    }

    if (moveTypes.checkmate) {
        square += "#";
        return square;
    }
    if (moveTypes.promotion) {
        square += "=Q";
    }
    if (moveTypes.check) {
        square += "+";
    }

    return square;
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
    const int knightMoveOffsets[] = {17, -17, 15, -15, 10, -10, 6, -6};

    // Check if the destination square is reachable from the source square
    for (int offset: knightMoveOffsets) {
        if (toSquare == fromSquare + offset) {
            return true;
        }
    }

    // If no matching offset is found, the move is not pseudo-legal
    return false;
}

bool isPseudoLegalWhitePawnMove(int fromSquare, int toSquare) {
    int pawn = BIT_BOARD[fromSquare];
    int target = BIT_BOARD[toSquare];

    if ((toSquare == fromSquare + 7 || toSquare == fromSquare + 9) && PositionDynamics.enPassantSquare == toSquare) {
        return true;
    }

    if ((toSquare == fromSquare + 7 || toSquare == fromSquare + 9) && (target & BLACK)) {
        return true;
    }

    if (BIT_BOARD[fromSquare + 8] == EMPTY) {
        if (toSquare == fromSquare + 8 || (getRankNumber(fromSquare) == 2 && toSquare == fromSquare + 16)) {
            return true;
        }
    }
    return false;
}

bool isPseudoLegalBlackPawnMove(int fromSquare, int toSquare) {
    int pawn = BIT_BOARD[fromSquare];
    int target = BIT_BOARD[toSquare];

    if ((toSquare == fromSquare - 7 || toSquare == fromSquare - 9) && PositionDynamics.enPassantSquare == toSquare) {
        return true;
    }

    if ((toSquare == fromSquare - 7 || toSquare == fromSquare - 9) && (target & WHITE)) {
        return true;
    }

    if (BIT_BOARD[fromSquare - 8] == EMPTY) {
        if (toSquare == fromSquare - 8 || (getRankNumber(fromSquare) == 7 && toSquare == fromSquare - 16)) {
            return true;
        }
    }
    return false;
}

bool isPseudoLegalPawnMove(int fromSquare, int toSquare) {
    switch (PositionDynamics.whoseMove) {
        case WHITE:
            return isPseudoLegalWhitePawnMove(fromSquare, toSquare);
        case BLACK:
            return isPseudoLegalBlackPawnMove(fromSquare, toSquare);
    }
    return false;
}

bool isPseudoLegalKingMove(int fromSquare, int toSquare) {
    const int kingMoveOffsets[] = {1, -1, 8, -8, 7, -7, 9, -9};

    // Check if the destination square is reachable from the source square
    for (int offset: kingMoveOffsets) {
        if (toSquare == fromSquare + offset) {
            return true;
        }
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
    switch (piece) {
        case BISHOP:
            return isPseudoLegalBishopMove(pieceSquare, kingSquare);
        case QUEEN:
            return isPseudoLegalQueenMove(pieceSquare, kingSquare);
        case ROOK:
            return isPseudoLegalRookMove(pieceSquare, kingSquare);
        case PAWN:
            if ((kingSquare == pieceSquare + 7 || kingSquare == pieceSquare + 9) && (BIT_BOARD[kingSquare] & BLACK)) {
                return true;
            }
            if ((kingSquare == pieceSquare - 7 || kingSquare == pieceSquare - 9) && (BIT_BOARD[kingSquare] & WHITE)) {
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

int checksNumber(int kingPosition) {
    int checkCount = 0;
    int king = BIT_BOARD[kingPosition];
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
                        checkCount++;
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
                        checkCount++;
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
                        checkCount++;
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
                        checkCount++;
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
        if (BIT_BOARD[enemyPiecePosition]) {
            if ((BIT_BOARD[enemyPiecePosition] & 3) != kingColor) {
                if (canAttackTheKing(enemyPiecePosition, kingPosition)) {
                    checkCount++;
                }
            }
        }
    }

    return checkCount;
}

bool isKingInCheck(int kingPosition) {
    return checksNumber(kingPosition) > 0;
}

bool kingHasLegalMoves(int kingPosition) {
    const int kingMoveOffsets[] = {1, -1, 8, -8, 7, -7, 9, -9};
    for (int offset: kingMoveOffsets) {
        int testPosition = kingPosition + offset;
        if (isPseudoLegalMove(kingPosition, testPosition)) {
            if (!(isKingInCheck(kingPosition))) {
                return true;
            }
        }
    }
    return false;
}

bool isCheckMate(int kingPosition) {
    if (!(isKingInCheck(kingPosition))) {
        return false;
    }

    if (checksNumber(kingPosition) > 1 && !kingHasLegalMoves(kingPosition)) {
        return true;
    }

    return false;
}

void makeMove(int fromSquare, int toSquare, MOVE_TYPES moveTypes) {
    updatePosition(fromSquare, toSquare);
    lcd.clear();
    if (BIT_BOARD[toSquare] & KING) {
        if (BIT_BOARD[toSquare] & WHITE) {
            whiteKingSquare = toSquare;
        } else {
            blackKingSquare = toSquare;
        }
    }

    if (PositionDynamics.whoseMove == WHITE) {
        if (BIT_BOARD[toSquare] & PAWN && toSquare / 8 == 7) {
            BIT_BOARD[toSquare] = WHITE_QUEEN;
            moveTypes.promotion = true;
        }
        if (BIT_BOARD[toSquare] & PAWN && (toSquare - fromSquare) == 16) {
            PositionDynamics.enPassantSquare = toSquare - 8;
        }
        moveTypes.check = isKingInCheck(blackKingSquare);
        PositionDynamics.whoseMove = BLACK;
        lcd.setCursor(0, 0);
        lcd.print("Black to move!");
    } else {
        if (BIT_BOARD[toSquare] & PAWN && toSquare / 8 == 0) {
            BIT_BOARD[toSquare] = BLACK_QUEEN;
            moveTypes.promotion = true;
        }
        if (BIT_BOARD[toSquare] & PAWN && (fromSquare - toSquare) == 16) {
            PositionDynamics.enPassantSquare = toSquare + 8;
        }
        moveTypes.check = isKingInCheck(whiteKingSquare);
        lcd.setCursor(0, 0);
        lcd.print("White to move!");
        PositionDynamics.whoseMove = WHITE;
    }

    String moveNotation = getMoveNotation(fromSquare, toSquare, moveTypes);
    lcd.setCursor(0, 1);
    lcd.print(moveNotation);

    resetPositionDynamics();
}

void resetPositionDynamics() {
    PositionDynamics.pickedSquare = -1;
    PositionDynamics.attackedPieceSquare = -1;
    PositionDynamics.moveTypes.capture = false;
    PositionDynamics.moveTypes.check = false;
    PositionDynamics.moveTypes.promotion = false;
    PositionDynamics.moveTypes.checkmate = false;
}

void verifyStartPosition() {
    for (int i = 0; i < 64; i++) {
        float channelValue = readMux(i);
        if (channelValue < 3.0 && BIT_BOARD[i] != EMPTY) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(getSquare(i));
            lcd.print(" is missing!");
            i--;
            delay(50);
        }
    }
    return;
}

void setup() {
    pinMode(s0, OUTPUT);
    pinMode(s1, OUTPUT);
    pinMode(s2, OUTPUT);
    pinMode(s3, OUTPUT);
    pinMode(s4, OUTPUT);
    pinMode(s5, OUTPUT);

    digitalWrite(s0, LOW);
    digitalWrite(s1, LOW);
    digitalWrite(s2, LOW);
    digitalWrite(s3, LOW);
    digitalWrite(s4, LOW);
    digitalWrite(s5, LOW);

    lcd.init();
    lcd.clear();
    lcd.backlight();      // Make sure backlight is on

    // Print a message on both lines of the LCD.
    verifyStartPosition();
    lcd.setCursor(0, 0);   //Set cursor to character 2 on line 0
    delay(1000);
    lcd.print("White to move!");
}

void loop() {
    for (int i = 0; i < 64; i++) {
        float channelValue = readMux(i);
        if (channelValue < 3.0 && BIT_BOARD[i] && PositionDynamics.pickedSquare != i &&
            PositionDynamics.attackedPieceSquare != i) {
            // enemy piece is picked
            if (!(BIT_BOARD[i] & PositionDynamics.whoseMove)) {
                PositionDynamics.moveTypes.capture = true;
                PositionDynamics.attackedPieceSquare = i;
                String squareWithPiece = getPieceLatter(BIT_BOARD[i]) + getSquare(i);

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Capturing ");
                lcd.print(squareWithPiece);
                break;
            }
            // piece is picked
            PositionDynamics.pickedSquare = i;
            String squareWithPiece = getPieceLatter(BIT_BOARD[i]) + getSquare(i);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(squareWithPiece);
            lcd.print(" is picked!");
        }
        if (channelValue > 3.0 && (BIT_BOARD[i] == EMPTY || PositionDynamics.attackedPieceSquare == i)) {
            if (isPseudoLegalMove(PositionDynamics.pickedSquare, i)) {
                if (PositionDynamics.whoseMove == WHITE && (PositionDynamics.attackedPieceSquare + 8) == i) {
                    clearSquare(PositionDynamics.attackedPieceSquare);
                }
                if (PositionDynamics.whoseMove == BLACK && (PositionDynamics.attackedPieceSquare - 8) == i) {
                    clearSquare(PositionDynamics.attackedPieceSquare);
                }
                PositionDynamics.enPassantSquare = -1;
                makeMove(PositionDynamics.pickedSquare, i, PositionDynamics.moveTypes);
            } else {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Illegal move!");
            }
        }
    }
}
