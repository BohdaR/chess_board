#include <LiquidCrystal_I2C.h>
#include "constants.h"
#include "helpers.h"
#include "move_validation.h"
#include "checkmate.h"
#include "castling.h"

//Mux control pins
int s0 = 7;
int s1 = 8;
int s2 = 9;
int s3 = 10;
int s4 = 11;
int s5 = 12;

//Mux in "SIG" pin
int SIG_pin = 1;

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
        square = square.substring(1, -1) + "=Q";
    }
    if (moveTypes.check) {
        square += "+";
    }

    return square;
}

String getCastlingNotation(MoveType castlingType, MOVE_TYPES moveTypes) {
    String notation = "O-O";
    if (castlingType == LONG_CASTLING) {
        notation = "O-O-O";
    }

    if (moveTypes.checkmate) {
        notation += "#";
        return notation;
    }

    if (moveTypes.check) {
        notation += "+";
    }

    return notation;
}

int getSquareColor(int square) {
    // return 0 for black square and 1 for white square
    return (square % 8 + square / 8) % 2;
}

void makeMove(int fromSquare, int toSquare, MOVE_TYPES moveTypes) {
    updatePosition(fromSquare, toSquare);
    if (PositionDynamics.castlingRights & FULL_CASTLING_RIGHTS) {
        switch (fromSquare) {
            case 0:
                PositionDynamics.castlingRights -= 8;
                break;
            case 7:
                PositionDynamics.castlingRights -= 4;
                break;
            case 56:
                PositionDynamics.castlingRights -= 2;
                break;
            case 63:
                PositionDynamics.castlingRights -= 1;
                break;
        }
    }

    if (BIT_BOARD[toSquare] & KING) {
        if (BIT_BOARD[toSquare] & WHITE) {
            if (PositionDynamics.castlingRights & WHITE_CASTLING_RIGHTS) {
                PositionDynamics.castlingRights -= WHITE_CASTLING_RIGHTS;
            }
            whiteKingSquare = toSquare;
        } else {
            if (PositionDynamics.castlingRights & BLACK_CASTLING_RIGHTS) {
                PositionDynamics.castlingRights -= BLACK_CASTLING_RIGHTS;
            }
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
        moveTypes.checkmate = isCheckMate(blackKingSquare);
    } else {
        if (BIT_BOARD[toSquare] & PAWN && toSquare / 8 == 0) {
            BIT_BOARD[toSquare] = BLACK_QUEEN;
            moveTypes.promotion = true;
        }
        if (BIT_BOARD[toSquare] & PAWN && (fromSquare - toSquare) == 16) {
            PositionDynamics.enPassantSquare = toSquare + 8;
        }
        moveTypes.check = isKingInCheck(whiteKingSquare);
        moveTypes.checkmate = isCheckMate(whiteKingSquare);
    }

    toggleWhoseMove();
    showWhoseMove();
    String moveNotation = getMoveNotation(fromSquare, toSquare, moveTypes);
    lcd.setCursor(0, 1);
    lcd.print(moveNotation);

    resetPositionDynamics();
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

void performCastling(MoveType castlingType, int rookToSquare, int rookFromSquare) {
    // Wait for the rook to appear on the target square
    while (readMux(rookToSquare) < 3.0) {
        // Wait for the rook to leave the square
        delay(50);
        while (readMux(rookFromSquare) > 3.0) {
            delay(50);
        }
    }

    updatePosition(rookFromSquare, rookToSquare);
    PositionDynamics.moveTypes.check = isKingInCheck(getOppositeColorKingSquare());
    PositionDynamics.moveTypes.checkmate = isCheckMate(getOppositeColorKingSquare());

    toggleWhoseMove();
    showWhoseMove();
    lcd.setCursor(0, 1);
    lcd.print(getCastlingNotation(castlingType, PositionDynamics.moveTypes));

    resetPositionDynamics();
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
        if (PositionDynamics.attackedPieceSquare == i && channelValue > 3.0 && PositionDynamics.pickedSquare == -1) {
            PositionDynamics.moveTypes.capture = false;
            PositionDynamics.attackedPieceSquare = -1;
            String squareWithPiece = getPieceLatter(BIT_BOARD[i]) + getSquare(i);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Moved back ");
            lcd.print(squareWithPiece);
            break;
        }
        if (channelValue > 3.0 && (BIT_BOARD[i] == EMPTY || PositionDynamics.attackedPieceSquare == i)) {
            int moveOffset = i - PositionDynamics.pickedSquare;
            // white's move
            if (PositionDynamics.whoseMove == WHITE) {
                // if picked piece is on the king start square
                if (PositionDynamics.pickedSquare == WHITE_KING_START_SQUARE) {
                    if (moveOffset == 2 && canCastleTheKing(SHORT_CASTLING)) {
                        updatePosition(PositionDynamics.pickedSquare, i);
                        whiteKingSquare = i;
                        performCastling(SHORT_CASTLING, 5, 7);
                        PositionDynamics.castlingRights -= WHITE_CASTLING_RIGHTS;
                        break;
                    }
                    else if (moveOffset == -2 && canCastleTheKing(LONG_CASTLING)) {
                        updatePosition(PositionDynamics.pickedSquare, i);
                        whiteKingSquare = i;
                        performCastling(LONG_CASTLING, 3, 0);
                        PositionDynamics.castlingRights -= WHITE_CASTLING_RIGHTS;
                        break;
                    }
                }

            }
            // black's move
            else {
                // if picked piece is on the king start square
                if (PositionDynamics.pickedSquare == BLACK_KING_START_SQUARE) {
                    if (moveOffset == 2 && canCastleTheKing(SHORT_CASTLING)) {
                        updatePosition(PositionDynamics.pickedSquare, i);
                        blackKingSquare = i;
                        performCastling(SHORT_CASTLING, 61, 63);
                        PositionDynamics.castlingRights -= BLACK_CASTLING_RIGHTS;
                        break;
                    }
                    else if (moveOffset == -2 && canCastleTheKing(LONG_CASTLING)) {
                        updatePosition(PositionDynamics.pickedSquare, i);
                        blackKingSquare = i;
                        performCastling(LONG_CASTLING, 59, 56);
                        PositionDynamics.castlingRights -= BLACK_CASTLING_RIGHTS;
                        break;
                    }
                }
            }


            if (isLegalMove(PositionDynamics.pickedSquare, i, getOwnKingSquare())) {
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
