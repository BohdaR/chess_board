#include "Arduino.h"
#include "../include/constants.h"
#include "../include/helpers.h"
#include "../include/move_validation.h"
#include "../include/checkmate.h"
#include "../include/castling.h"
#include "../include/stalemate.h"
#include "../include/threefold_repetition.h"
#include "../include/disambiguation.h"

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
    return val * 3.3 / 4095;
}

String formatTime(unsigned long time) {
    int minutes = time / 60000;
    int seconds = time / 1000 % 60;
    int miliseconds = time % 1000 / 100;
    char formattedTime[6];
    if (time > 60000) {
        sprintf(formattedTime, "%02d:%02d", minutes, seconds);
    } else {
        sprintf(formattedTime, "%02d.%d", seconds, miliseconds);
    }
    return String(formattedTime);
}

void decrementTime() {
    if (PositionDynamics.whoseMove == BLACK) blackPlayerTime = blackPlayerTime - 100;
    if (PositionDynamics.whoseMove == WHITE) whitePlayerTime = whitePlayerTime - 100;
}

void updateClock() {
    String whiteFormattedTime = formatTime(whitePlayerTime);
    String blackFormattedTime = formatTime(blackPlayerTime);

    lcd.setCursor(0, 1);
    lcd.print(whiteFormattedTime);
    lcd.setCursor(11, 1);
    lcd.print(blackFormattedTime);

    decrementTime();
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
    Piece movedPiece = BIT_BOARD[fromSquare];
    clearSquare(fromSquare);
    BIT_BOARD[toSquare] = movedPiece;
}

void capturePiece(int attackingPiece, int capturedPiece) {
    updatePosition(attackingPiece, capturedPiece);

}

String getSquare(int squareIndex) {
    return String(getFileLetter(squareIndex)) + String(getRankNumber(squareIndex));
}

String getMoveNotation(int fromSquare, int toSquare, MOVE_TYPES moveTypes, int disambiguation) {
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

    String disambiguationNotation = getDisambiguationNotation(disambiguation, fromSquare);

    square = String(square[0]) + disambiguationNotation + square.substring(1, -1);

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
    int disambiguation = getDisambiguation(fromSquare, toSquare);

    if (moveTypes.capture || (BIT_BOARD[fromSquare] & PAWN)) {
        resetPositionTracking();
    }
    updatePosition(fromSquare, toSquare);
    savePosition();

    if (currentPositionIndex == MAX_POSITIONS) {
        PositionDynamics.moveTypes.draw = true;
        showDrawMessage("50 moves rule!");
        return;
    }

    if (isTreeFoldRepetition()) {
        PositionDynamics.moveTypes.draw = true;
        showDrawMessage("Threefold!");
        return;
    }
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
        PositionDynamics.moveTypes.checkmate = moveTypes.checkmate;
        whitePlayerTime = whitePlayerTime + timeIncrement;
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
        PositionDynamics.moveTypes.checkmate = moveTypes.checkmate;
        blackPlayerTime = blackPlayerTime + timeIncrement;
    }

    if (isStalemate(getOppositeColorKingSquare())) {
        PositionDynamics.moveTypes.draw = true;
        showDrawMessage("Stalemate!");
        return;
    }

    if (moveTypes.checkmate) {
        if (PositionDynamics.whoseMove == WHITE) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("White won! 1-0");
        } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Black won! 0-1");
        }
    }

    toggleWhoseMove();
    if (!moveTypes.checkmate) {
        showWhoseMove();
    }
    String moveNotation = getMoveNotation(fromSquare, toSquare, moveTypes, disambiguation);
//    lcd.setCursor(0, 1);
//    lcd.print(moveNotation);
    Serial.println(moveNotation);

    resetPositionDynamics();
    if (!(isSufficientMaterial(WHITE) || isSufficientMaterial(BLACK))) {
        showDrawMessage("Draw!");
    }
}

void verifyPosition() {
    for (int i = 0; i < 64; i++) {
        float channelValue = readMux(i);
        if (channelValue < peakValue && BIT_BOARD[i] != EMPTY) {
            lcd.setCursor(0, 1);
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
    while (readMux(rookToSquare) < peakValue) {
        updateClock();
        delay(90);
        // Wait for the rook to leave the square
        while (readMux(rookFromSquare) > peakValue) {
            updateClock();
            delay(90);
        }
    }

    updatePosition(rookFromSquare, rookToSquare);
    PositionDynamics.moveTypes.check = isKingInCheck(getOppositeColorKingSquare());
    PositionDynamics.moveTypes.checkmate = isCheckMate(getOppositeColorKingSquare());
    if (isStalemate(getOppositeColorKingSquare())) {
        PositionDynamics.moveTypes.draw = true;
        showDrawMessage("Stalemate!");
        return;
    }

    toggleWhoseMove();
    savePosition();
    showWhoseMove();
//    lcd.setCursor(0, 1);
//    lcd.print(getCastlingNotation(castlingType, PositionDynamics.moveTypes));
    Serial.println(getCastlingNotation(castlingType, PositionDynamics.moveTypes));

    resetPositionDynamics();
}

void handleIllegalMove() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Illegal move!");

    resetPositionDynamics();
    verifyPosition();

    delay(500);
    lcd.clear();
    showWhoseMove();
}

void chooseTimeControl() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Choose");

    lcd.setCursor(0, 1);
    lcd.print("time control");
    bool timeControlChosen = false;
    while(!timeControlChosen) {
        for (int i = 16; i < 48; i++) {
            float squareValue = readMux(i);
            if (squareValue > peakValue) {
                timeControlChosen = true;
                switch(i) {
                    case 16:
                        whitePlayerTime = 180000;
                        blackPlayerTime = 180000;
                        timeIncrement = 0;
                        break;
                    case 24:
                        whitePlayerTime = 180000;
                        blackPlayerTime = 180000;
                        timeIncrement = 2000;
                        break;
                    case 32:
                        whitePlayerTime = 300000;
                        blackPlayerTime = 300000;
                        timeIncrement = 0;
                        break;
                    case 40:
                        whitePlayerTime = 300000;
                        blackPlayerTime = 300000;
                        timeIncrement = 2000;
                        break;
                    case 17:
                        whitePlayerTime = 600000;
                        blackPlayerTime = 600000;
                        timeIncrement = 0;
                        break;
                    case 25:
                        whitePlayerTime = 600000;
                        blackPlayerTime = 600000;
                        timeIncrement = 5000;
                        break;
                    case 33:
                        whitePlayerTime = 900000;
                        blackPlayerTime = 900000;
                        timeIncrement = 0;
                        break;
                    case 41:
                        whitePlayerTime = 900000;
                        blackPlayerTime = 900000;
                        timeIncrement = 10000;
                        break;
                    case 18:
                        whitePlayerTime = 5400000;
                        blackPlayerTime = 5400000;
                        timeIncrement = 30000;
                        break;
                    default:
                        break;
                }
                break;
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
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
    verifyPosition();
    savePosition(); // save initial position
    lcd.setCursor(0, 0);   //Set cursor to character 2 on line 0
    delay(1000);
    chooseTimeControl();
    delay(2000);

    lcd.clear();
    lcd.print("White to move!");
}

void loop() {
    if (!PositionDynamics.moveTypes.checkmate && !PositionDynamics.moveTypes.draw && whitePlayerTime > 0 &&
        blackPlayerTime > 0) {

        updateClock();
        delay(70);

        for (int i = 0; i < 64; i++) {
            float channelValue = readMux(i);
            if (channelValue < peakValue && BIT_BOARD[i] && PositionDynamics.pickedSquare != i &&
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
            if (PositionDynamics.attackedPieceSquare == i && channelValue > peakValue &&
                PositionDynamics.pickedSquare == -1) {
                PositionDynamics.moveTypes.capture = false;
                PositionDynamics.attackedPieceSquare = -1;
                String squareWithPiece = getPieceLatter(BIT_BOARD[i]) + getSquare(i);

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Moved back ");
                lcd.print(squareWithPiece);
                break;
            }
            if (channelValue > peakValue && (BIT_BOARD[i] == EMPTY || PositionDynamics.attackedPieceSquare == i)) {
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
                        } else if (moveOffset == -2 && canCastleTheKing(LONG_CASTLING)) {
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
                        } else if (moveOffset == -2 && canCastleTheKing(LONG_CASTLING)) {
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
                    handleIllegalMove();
                }
            }
        }
    }
}
