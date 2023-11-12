#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display
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
  EMPTY  = 0,
  PAWN   = 4,
  KNIGHT = 8,
  BISHOP = 16,
  ROOK   = 32,
  QUEEN  = 64,
  KING   = 128,

  WHITE_PAWN   = 5,
  WHITE_KNIGHT = 9,
  WHITE_BISHOP = 17,
  WHITE_ROOK   = 33,
  WHITE_QUEEN  = 65,
  WHITE_KING   = 129,

  BLACK_PAWN   = 6,
  BLACK_KNIGHT = 10,
  BLACK_BISHOP = 18,
  BLACK_ROOK   = 34,
  BLACK_QUEEN  = 66,
  BLACK_KING   = 130
};

enum Color {
  WHITE = 1,
  BLACK = 2
};

enum MoveType {
  MOVE           = 1,
  CAPTURE        = 2,
  CHECK          = 4,
  CHECKMATE      = 8,
  PROMOTION      = 16,
  LONG_CASTLING  = 32,
  SHORT_CASTLING = 64
};

struct MOVE_TYPES{
  bool capture   = false;
  bool check     = false;
  bool checkmate = false;
  bool promotion = false;
};

enum KING_CONSTANT {
  WHITE_CASTLING_RIGHTS   = 12,
  BLACK_CASTLING_RIGHTS   = 3,
  FULL_CASTLING_RIGHTS    = WHITE_CASTLING_RIGHTS + BLACK_CASTLING_RIGHTS,

  WHITE_KING_START_SQUARE = 4,
  BLACK_KING_START_SQUARE = 60,
};

struct {
  byte whosMove            = WHITE; // 1 for white, 2 for black
  byte pickedSquare        = -1;
  byte enpassantSquare     = -1;
  byte attackedPieceSquare = -1;
  byte castlingRights      = FULL_CASTLING_RIGHTS; // binary representation of castling rights
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

float readMux(int channel){
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

char getFileLetter(byte squareIndex) {
  char fileLetter = 'a' + squareIndex % 8;
  return fileLetter;
}

byte getRankNumber(byte squareIndex) {
  return squareIndex / 8 + 1;
}

byte getPieceColor(byte squareIndex) {
  return BIT_BOARD[squareIndex] & 3; // we take first 2 bits which are use for piece color
}

String getPieceLatter(byte squareIndex) {
  byte piece = BIT_BOARD[squareIndex] & 252; // here we take only 6 bit of piece 'cause firt 2 bits are used for piece color
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

void clearSquare(byte squareIndex) {
  BIT_BOARD[squareIndex] = EMPTY;
}

void updatePosition(byte fromSquare, byte toSquare) {
  byte movedPiece = BIT_BOARD[fromSquare];
  clearSquare(fromSquare);
  BIT_BOARD[toSquare] = movedPiece;
}

void capturePiece(byte attakingPiece, byte capturedPiece) {
  updatePosition(attakingPiece, capturedPiece);

}

void castle() {

}

String getSquare(byte squareIndex) {
  return String(getFileLetter(squareIndex)) + String(getRankNumber(squareIndex));
}

String getMoveNotation(byte fromSquare, byte toSquare, MOVE_TYPES moveTypes) {
  byte piece = BIT_BOARD[fromSquare];
  String square = getPieceLatter(fromSquare) + getSquare(toSquare);

  if (moveTypes.capture) {
    if (piece & PAWN) {
        char previousFileLetter = getFileLetter(fromSquare);
        square = String(previousFileLetter) + "x" + square;
      }
      else {
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

// new functionality

byte whiteKingSquare = WHITE_KING_START_SQUARE;
byte blackKingSquare = BLACK_KING_START_SQUARE;

byte getSquareColor(byte square) {
  // return 0 for black square and 1 for white square
  return (square % 8 + square / 8) % 2;
}

bool isOnTheSameDiagonal(byte firstSquare, byte secondSquare) {
  // check whether two pieces on the same diagonal such as a1-h8
  return abs(firstSquare / 8 - secondSquare / 8) == abs(firstSquare % 8 - secondSquare % 8);
}

bool isOnTheSameFirstDiagonal(byte firstSquare, byte secondSquare) {
  // check whether two pieces on the same diagonal such as a1-h8
  return isOnTheSameDiagonal(firstSquare, secondSquare) && firstSquare % 9 == secondSquare % 9;
}

bool isOnTheSameSecondDiagonal(byte firstSquare, byte secondSquare) {
  // check whether two pieces on the same diagonal such as h1-a8
  return isOnTheSameDiagonal(firstSquare, secondSquare) && firstSquare % 7 == secondSquare % 7;
}

bool isOnTheSameRank(byte firstSquare, byte secondSquare) {
  return (firstSquare / 8) == (secondSquare / 8);
}

bool isOnTheSameFile(byte firstSquare, byte secondSquare) {
  return (firstSquare % 8) == (secondSquare % 8);
}

// move validation

bool isPseudoLegalKnightMove(byte fromSquare, byte toSquare) {
  const byte knightMoveOffsets[] = {17, -17, 15, -15, 10, -10, 6, -6};
  
  // Check if the destination square is reachable from the source square
  for (int offset : knightMoveOffsets) {
    if (toSquare == fromSquare + offset) {
      return true;
    }
  }

  // If no matching offset is found, the move is not pseudo-legal
  return false;
}

bool isPseudoLegalWhitePawnMove(byte fromSquare, byte toSquare) {
  byte pawn = BIT_BOARD[fromSquare];
  byte target = BIT_BOARD[toSquare];

  if ((toSquare == fromSquare + 7 || toSquare == fromSquare + 9) && PositionDynamics.enpassantSquare == toSquare) {
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

bool isPseudoLegalBlackPawnMove(byte fromSquare, byte toSquare) {
  byte pawn = BIT_BOARD[fromSquare];
  byte target = BIT_BOARD[toSquare];

  if ((toSquare == fromSquare - 7 || toSquare == fromSquare - 9) && PositionDynamics.enpassantSquare == toSquare) {
    return true;
  }
  
  if ((toSquare == fromSquare - 7 || toSquare == fromSquare - 9) && (target & BLACK)) {
    return true;
  }

  if (BIT_BOARD[fromSquare-+ 8] == EMPTY) {
    if (toSquare == fromSquare - 8 || (getRankNumber(fromSquare) == 7 && toSquare == fromSquare - 16)) {
      return true;
    }
  }
  return false;
}

bool isPseudoLegalPawnMove(byte fromSquare, byte toSquare) {
  switch (PositionDynamics.whosMove) {
    case WHITE:
      return isPseudoLegalWhitePawnMove(fromSquare, toSquare);
    case BLACK:
      return isPseudoLegalBlackPawnMove(fromSquare, toSquare);
  }
  return false;
}

bool isPseudoLegalKingMove(byte fromSquare, byte toSquare) {
  const byte kingMoveOffsets[] = {1, -1, 8, -8, 7, -7, 9, -9};
  
  // Check if the destination square is reachable from the source square
  for (int offset : kingMoveOffsets) {
    if (toSquare == fromSquare + offset) {
      return true;
    }
  }

  // If no matching offset is found, the move is not pseudo-legal
  return false;
}

bool isPseudoLegalBishopMove(byte fromSquare, byte toSquare) {
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

bool isPreudoLegalRookMove(byte fromSquare, byte toSquare) {
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

bool isPseudoLegalQueenMove(byte fromSquare, byte toSquare) {
  return isPseudoLegalBishopMove(fromSquare, toSquare) || isPreudoLegalRookMove(fromSquare, toSquare);
}

bool isPseudoLegalMove(byte fromSquare, byte toSquare) {
  // check if on the target squere a piece of the same color
  if ((BIT_BOARD[toSquare] & 3) == (BIT_BOARD[fromSquare] & 3)) {
    return false;
  }

  // Pseudo-legal moves may still be illegal if they leave the own king in check
  byte piece = BIT_BOARD[fromSquare] & 252; // here we take only 6 bit of piece 'cause firt 2 bits are used for piece color
  bool isValid = false;
   switch (piece) {
    case BISHOP:
      return isPseudoLegalBishopMove(fromSquare, toSquare);
    case QUEEN:
      return isPseudoLegalQueenMove(fromSquare, toSquare);
    case ROOK:
      return isPreudoLegalRookMove(fromSquare, toSquare);
    case PAWN:
      return isPseudoLegalPawnMove(fromSquare, toSquare);
    case KNIGHT:
      return isPseudoLegalKnightMove(fromSquare, toSquare);
    case KING:
      return isPseudoLegalKingMove(fromSquare, toSquare);
  }
  return isValid;
}

void makeMove(byte fromSquare, byte toSquare, MOVE_TYPES moveTypes) {
  String moveNotation = getMoveNotation(fromSquare, toSquare, moveTypes);

  updatePosition(fromSquare, toSquare);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(moveNotation);

  if (PositionDynamics.whosMove == WHITE) {
    PositionDynamics.whosMove = BLACK;
    lcd.setCursor(0, 1);
    lcd.print("Black to move!");
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("White to move!");
    PositionDynamics.whosMove = WHITE;
  }

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
  for(int i = 0; i < 64; i++ ){
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
  lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
  delay(1000);
  lcd.print("White to move!");
}

void loop() {
  for(int i = 0; i < 64; i++ ){
    float channelValue = readMux(i);
    if (channelValue < 3.0 && BIT_BOARD[i] != EMPTY && PositionDynamics.pickedSquare != i) {
      // enemy piece is picked
      if (!(BIT_BOARD[i] & PositionDynamics.whosMove)) {
        PositionDynamics.moveTypes.capture = true;
        PositionDynamics.attackedPieceSquare = i;
        String squareWithPiece = getPieceLatter(i) + getSquare(i);
        // remove enemy piece from the board
        clearSquare(i);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Capturing ");
        lcd.print(squareWithPiece);
        break;
      }
      // piece is picked
      PositionDynamics.pickedSquare = i;
      String squareWithPiece = getPieceLatter(i) + getSquare(i);
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(squareWithPiece); 
      lcd.print(" is picked!"); 
    }
    if (channelValue > 3.0 && BIT_BOARD[i] == EMPTY) {
      makeMove(PositionDynamics.pickedSquare, i, PositionDynamics.moveTypes);
    }
  }
}
