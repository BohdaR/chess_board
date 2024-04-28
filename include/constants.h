#ifndef constants_h
#define constants_h
#define MAX_POSITIONS 100

#include "Arduino.h"

//Mux control pins
int s0 = 32;
int s1 = 33;
int s2 = 25;
int s3 = 26;
int s4 = 27;
int s5 = 14;

//Mux in "SIG" pin
int SIG_pin = 35;

// Peak value to treat sensor as detected
float peakValue = 1.8;

// WiFi constants
const char* SSID = "shushval5G";
const char* PASSWORD = "tp-link08752!";

int GAME_ID;
String currentMove;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x3F for 16 chars and 2 line display
unsigned long positionsHistory[MAX_POSITIONS];

int currentPositionIndex = -1;

// blitz time control 3 | 2
unsigned long whitePlayerTime = 180000;
unsigned long blackPlayerTime = 180000;
int timeIncrement = 2000;

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

const int KNIGHT_MOVE_OFFSETS[8] = {17, -17, 15, -15, 10, -10, 6, -6};
const int KING_MOVE_OFFSETS[8] = {1, -1, 8, -8, 7, -7, 9, -9};

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
    bool draw = false;
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
