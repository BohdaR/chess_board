#include <LiquidCrystal_I2C.h>
#ifndef constants_h
#define constants_h

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F for 16 chars and 2 line display

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
    SHORT_CASTLING = 32,
    LONG_CASTLING = 64
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

#endif