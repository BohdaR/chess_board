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

byte const FULL_CASTLING_RIGHTS = 7;

struct {
  byte whosMove            = WHITE; // 1 for white, 2 for black
  byte pickedSquare        = -1;
  byte enpassantSquare     = -1;
  byte attackedPieceSquare = -1;
  byte castlingRights      = FULL_CASTLING_RIGHTS; // binary representation of castling rights
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

char getFileLetter(byte squareIndex) {
  char fileLetter = 'a' + squareIndex % 8;
  return fileLetter;
}

int getRankNumber(byte squareIndex) {
  return squareIndex / 8 + 1;
}

int getPieceColor(byte squareIndex) {
  return BIT_BOARD[squareIndex] & 3;
}

String getPieceLatter(byte squareIndex) {
  byte piece = BIT_BOARD[squareIndex] & 252;
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

void updatePosition(byte fromSquare, byte toSquare) {
  byte movedPiece = BIT_BOARD[fromSquare];
  BIT_BOARD[fromSquare] = EMPTY;
  BIT_BOARD[toSquare] = movedPiece;
}

void capturePiece(byte attakingPiece, byte capturedPiece) {
  updatePosition(attakingPiece, capturedPiece);

}

String getSquare(byte squareIndex) {
  return String(getFileLetter(squareIndex)) + String(getRankNumber(squareIndex));
}

String getMoveNotation(byte fromSquare, byte toSquare, bool capture=false, bool check=false, bool checkmate=false, bool promotion=false) {
  byte piece = BIT_BOARD[fromSquare];
  String square = getPieceLatter(fromSquare) + getSquare(toSquare);

  if (capture) {
    if (piece & PAWN) {
        char previousFileLetter = getFileLetter(fromSquare);
        square = String(previousFileLetter) + "x" + square;
      }
      else {
        square = String(square[0]) + "x" + square.substring(1, -1);
      }
  }

  if (checkmate) {
    square += "#";
    return square;
  }
  if (promotion) {
    square += "=Q";
  }
  if (check) {
    square += "+";
  }

  return square;
}

void setup() {
  Serial.begin(9600);
  Serial.println(getMoveNotation(1, 17, true, true, true));
}

void loop() {

}
