#include "../include/constants.h"
#include "../include/helpers.h"
#include "../include/move_validation.h"
#include "../include/checkmate.h"
#include "../include/castling.h"
#include "../include/stalemate.h"
#include "../include/threefold_repetition.h"
#include "../include/disambiguation.h"
#include "../include/clock.h"
#include "../include/move_registration.h"
#include "../include/api.h"
#include "../include/sd_card.h"

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
    connectToWiFi();
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }

    if (CONNECTED_TO_INTERNET) createNewGame(); // API call
    createDir(SD, "/games");
    createNewPGN(SD);
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
