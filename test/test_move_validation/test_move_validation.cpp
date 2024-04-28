#include "unity.h"
#include "../../include/move_validation.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {

}

// Test withinTheBoard function
void square_should_be_withing_the_board() {
    TEST_ASSERT_TRUE(withinTheBoard(0));
    TEST_ASSERT_TRUE(withinTheBoard(63));
    TEST_ASSERT_TRUE(withinTheBoard(33));

    TEST_ASSERT_FALSE(withinTheBoard(-1));
    TEST_ASSERT_FALSE(withinTheBoard(64));
}

// Test isOnTheSameDiagonal function
void should_be_on_the_same_diagonal() {
    // a1 and h8
    TEST_ASSERT_TRUE(isOnTheSameDiagonal(0, 63));
    // c4 and d5
    TEST_ASSERT_TRUE(isOnTheSameDiagonal(26, 35));
    // h1 and a8
    TEST_ASSERT_TRUE(isOnTheSameDiagonal(7, 56));
    // f4 and g3
    TEST_ASSERT_TRUE(isOnTheSameDiagonal(29, 22));
}

// Test isOnTheSameDiagonal function
void should_not_be_on_the_same_diagonal() {
    // a1 and h7
    TEST_ASSERT_FALSE(isOnTheSameDiagonal(0, 62));
    // b4 and d5
    TEST_ASSERT_FALSE(isOnTheSameDiagonal(25, 35));
    // h1 and a7
    TEST_ASSERT_FALSE(isOnTheSameDiagonal(7, 48));
    // c2 and c7
    TEST_ASSERT_FALSE(isOnTheSameDiagonal(10, 50));
}

// Test isOnTheSameFirstDiagonal function
void should_be_on_the_same_first_diagonal(void) {
    // a1 and h8
    TEST_ASSERT_TRUE(isOnTheSameFirstDiagonal(0, 63));
    // a3 and f8
    TEST_ASSERT_TRUE(isOnTheSameFirstDiagonal(16, 61));
    // e3 and g5
    TEST_ASSERT_TRUE(isOnTheSameFirstDiagonal(20, 38));
}

// Test isOnTheSameFirstDiagonal function
void should_not_be_on_the_same_first_diagonal(void) {
    // Check on the squares with the same value by mod 9
    TEST_ASSERT_FALSE(isOnTheSameFirstDiagonal(39, 48));
    TEST_ASSERT_FALSE(isOnTheSameFirstDiagonal(14, 32));
    TEST_ASSERT_FALSE(isOnTheSameFirstDiagonal(14, 41));
}

// Test isOnTheSameSecondDiagonal function
void should_be_on_the_same_second_diagonal(void) {
    // h1 and a8
    TEST_ASSERT_TRUE(isOnTheSameSecondDiagonal(7, 56));
    // f2 and b6
    TEST_ASSERT_TRUE(isOnTheSameSecondDiagonal(13, 41));
    // h5 and f7
    TEST_ASSERT_TRUE(isOnTheSameSecondDiagonal(39, 53));
}

// Test isOnTheSameSecondDiagonal function
void should_not_be_on_the_same_second_diagonal(void) {
    // Check on the squares with the same value by mod 7
    TEST_ASSERT_FALSE(isOnTheSameSecondDiagonal(0, 7));
    TEST_ASSERT_FALSE(isOnTheSameSecondDiagonal(0, 56));
    TEST_ASSERT_FALSE(isOnTheSameSecondDiagonal(16, 23));
}

// Test isOnTheSameRank function
void should_be_on_the_same_rank(void) {
    // a1 and h1
    TEST_ASSERT_TRUE(isOnTheSameRank(0, 7));
    // a8 and h8
    TEST_ASSERT_TRUE(isOnTheSameRank(56, 63));
    // b5 and e5
    TEST_ASSERT_TRUE(isOnTheSameRank(33, 36));
}
// Test isOnTheSameRank function
void should_not_be_on_the_same_rank(void) {
    // a1 and a2
    TEST_ASSERT_FALSE(isOnTheSameRank(0, 8));
    // d6 and c5
    TEST_ASSERT_FALSE(isOnTheSameRank(43, 34));
    // b2 and g7
    TEST_ASSERT_FALSE(isOnTheSameRank(9, 54));
}

// Test isOnTheSameFile function
void should_be_on_the_same_file(void) {
    // a1 and a8
    TEST_ASSERT_TRUE(isOnTheSameFile(0, 56));
    // h1 and h8
    TEST_ASSERT_TRUE(isOnTheSameFile(63, 7));
    // d2 and d7
    TEST_ASSERT_TRUE(isOnTheSameFile(11, 51));
}

// Test isOnTheSameFile function
void should_not_be_on_the_same_file(void) {
    // a1 and b3
    TEST_ASSERT_FALSE(isOnTheSameRank(0, 17));
    // d4 and e6
    TEST_ASSERT_FALSE(isOnTheSameRank(27, 44));
    // f6 and g5
    TEST_ASSERT_FALSE(isOnTheSameRank(45, 38));
}

// Test unexpected input for on the same line functions
void should_handle_unexpected_input(void) {
    // Test outside the board
    TEST_ASSERT_FALSE(isOnTheSameDiagonal(-9, 0));
    TEST_ASSERT_FALSE(isOnTheSameDiagonal(-7, 0));
    TEST_ASSERT_FALSE(isOnTheSameDiagonal(-7, -14));

    TEST_ASSERT_FALSE(isOnTheSameFirstDiagonal(-18, -9));
    TEST_ASSERT_FALSE(isOnTheSameFirstDiagonal(63, 72));

    TEST_ASSERT_FALSE(isOnTheSameSecondDiagonal(56, 64));
    TEST_ASSERT_FALSE(isOnTheSameSecondDiagonal(-7, 0));

    TEST_ASSERT_FALSE(isOnTheSameFile(0, -8));
    TEST_ASSERT_FALSE(isOnTheSameFile(0, 64));

    TEST_ASSERT_FALSE(isOnTheSameRank(-8, -1));
    TEST_ASSERT_FALSE(isOnTheSameRank(64, 66));
}

// Test isPseudoLegalKnightMove function
void should_check_pseudo_legal_knight_move(void) {
    int knightSquare = 35;
    BIT_BOARD[knightSquare] = WHITE_KNIGHT;
    for (int offset: KNIGHT_MOVE_OFFSETS) {
        int targetSquare = knightSquare + offset;
        TEST_ASSERT_TRUE(isPseudoLegalKnightMove(knightSquare, targetSquare));
    }
    // Check in case of capture
    TEST_ASSERT_TRUE(isPseudoLegalKnightMove(knightSquare, 52));
    TEST_ASSERT_TRUE(isPseudoLegalKnightMove(knightSquare, 50));

    // Check in case of moving to the square with own piece
    BIT_BOARD[knightSquare] = BLACK_KNIGHT;
    TEST_ASSERT_FALSE(isPseudoLegalKnightMove(knightSquare, 52));
    TEST_ASSERT_FALSE(isPseudoLegalKnightMove(knightSquare, 50));

    BIT_BOARD[knightSquare] = EMPTY;

    // Check illegal input
    TEST_ASSERT_FALSE(isPseudoLegalKnightMove(1, -9));
    TEST_ASSERT_FALSE(isPseudoLegalKnightMove(6, 16));
}

// Test isPseudoLegalPawnMove function
void should_check_pseudo_legal_pawn_move(void) {
    // Check one square move
    TEST_ASSERT_TRUE(isPseudoLegalPawnMove(11, 19));
    TEST_ASSERT_TRUE(isPseudoLegalPawnMove(51, 43));

    // Check two square first move
    TEST_ASSERT_TRUE(isPseudoLegalPawnMove(11, 27));
    TEST_ASSERT_TRUE(isPseudoLegalPawnMove(51, 35));

    // Check two square not first move
    BIT_BOARD[23] = WHITE_PAWN;
    TEST_ASSERT_FALSE(isPseudoLegalPawnMove(23, 39));
    BIT_BOARD[40] = BLACK_PAWN;
    TEST_ASSERT_FALSE(isPseudoLegalPawnMove(40, 24));
    BIT_BOARD[23] = EMPTY;
    BIT_BOARD[40] = EMPTY;

    // Check two squares if pawn is blocked
    BIT_BOARD[19] = BLACK_PAWN;
    TEST_ASSERT_FALSE(isPseudoLegalPawnMove(11, 27));
    BIT_BOARD[43] = WHITE_PAWN;
    TEST_ASSERT_FALSE(isPseudoLegalPawnMove(51, 35));

    // Check captures
    TEST_ASSERT_TRUE(isPseudoLegalPawnMove(10, 19));
    TEST_ASSERT_TRUE(isPseudoLegalPawnMove(12, 19));

    TEST_ASSERT_TRUE(isPseudoLegalPawnMove(50, 43));
    TEST_ASSERT_TRUE(isPseudoLegalPawnMove(52, 43));

    // Check illegal moves
    BIT_BOARD[19] = EMPTY;
    BIT_BOARD[43] = EMPTY;
    TEST_ASSERT_FALSE(isPseudoLegalPawnMove(10, 19));
    TEST_ASSERT_FALSE(isPseudoLegalPawnMove(12, 19));

    TEST_ASSERT_FALSE(isPseudoLegalPawnMove(50, 43));
    TEST_ASSERT_FALSE(isPseudoLegalPawnMove(52, 43));
}

// Test isPseudoLegalKingMove function
void should_check_pseudo_legal_king_move(void) {
    // Here kings do not have any moves 'cause they're blocked by own pieces
    for (int offset: KING_MOVE_OFFSETS) {
        int whiteTargetSquare = WHITE_KING_START_SQUARE + offset;
        int blackTargetSquare = BLACK_KING_START_SQUARE + offset;
        TEST_ASSERT_FALSE(isPseudoLegalKingMove(WHITE_KING_START_SQUARE, whiteTargetSquare));
        TEST_ASSERT_FALSE(isPseudoLegalKingMove(BLACK_KING_START_SQUARE, blackTargetSquare));
    }

    // Put kings to the center to unsure that they have moves
    BIT_BOARD[WHITE_KING_START_SQUARE] = EMPTY;
    BIT_BOARD[BLACK_KING_START_SQUARE] = EMPTY;
    whiteKingSquare = 25;
    blackKingSquare = 38;
    BIT_BOARD[whiteKingSquare] = WHITE_KING;
    BIT_BOARD[blackKingSquare] = BLACK_KING;

    // Check whether they can move
    for (int offset: KING_MOVE_OFFSETS) {
        int whiteTargetSquare = whiteKingSquare + offset;
        int blackTargetSquare = blackKingSquare + offset;
        TEST_ASSERT_TRUE(isPseudoLegalKingMove(whiteKingSquare, whiteTargetSquare));
        TEST_ASSERT_TRUE(isPseudoLegalKingMove(blackKingSquare, blackTargetSquare));
    }
    whiteKingSquare = WHITE_KING_START_SQUARE;
    blackKingSquare = BLACK_KING_START_SQUARE;
    BIT_BOARD[whiteKingSquare] = WHITE_KING;
    BIT_BOARD[blackKingSquare] = BLACK_KING;
    BIT_BOARD[25] = EMPTY;
    BIT_BOARD[38] = EMPTY;
}

// Test isPseudoLegalBishopMove function
void should_check_pseudo_legal_bishop_move(void) {
    // Bishops can not move from the start squares
    // Check weather bishop can move to the square with the same color piece
    TEST_ASSERT_FALSE(isPseudoLegalBishopMove(2, 9));
    TEST_ASSERT_FALSE(isPseudoLegalBishopMove(2, 11));
    TEST_ASSERT_FALSE(isPseudoLegalBishopMove(61, 47));
    TEST_ASSERT_FALSE(isPseudoLegalBishopMove(5, -6));
    TEST_ASSERT_FALSE(isPseudoLegalBishopMove(58, 65));

    // Put bishop on a1 and clear the a1-h8 diagonal
    BIT_BOARD[0] = WHITE_BISHOP;
    BIT_BOARD[9] = EMPTY;
    BIT_BOARD[54] = EMPTY;

    // Verify taking and movement on the longest first diagonal a1-h8
    TEST_ASSERT_TRUE(isPseudoLegalBishopMove(0, 63));

    // Put bishop on h1 and clear the h1-a8 diagonal
    BIT_BOARD[7] = WHITE_BISHOP;
    BIT_BOARD[14] = EMPTY;
    BIT_BOARD[49] = EMPTY;

    // Verify taking and movement on the longest second diagonal h1-a8
    TEST_ASSERT_TRUE(isPseudoLegalBishopMove(7, 56));

    // Check movement along diagonals in all directions
    BIT_BOARD[36] = BLACK_BISHOP;
    TEST_ASSERT_TRUE(isPseudoLegalBishopMove(36, 54));
    TEST_ASSERT_TRUE(isPseudoLegalBishopMove(36, 0));
    TEST_ASSERT_TRUE(isPseudoLegalBishopMove(36, 22));
    TEST_ASSERT_TRUE(isPseudoLegalBishopMove(36, 43));

    // Bishop can not move to a different color square
    TEST_ASSERT_FALSE(isPseudoLegalBishopMove(0, 17));

    // Restore board state
    BIT_BOARD[0] = WHITE_ROOK;
    BIT_BOARD[7] = WHITE_ROOK;
    BIT_BOARD[9] = WHITE_PAWN;
    BIT_BOARD[14] = WHITE_PAWN;
    BIT_BOARD[36] = EMPTY;
    BIT_BOARD[54] = BLACK_PAWN;
    BIT_BOARD[49] = BLACK_PAWN;
}

// Test isPseudoLegalRookMove function
void should_check_pseudo_legal_rook_move(void) {
    // Rooks aren't able to move 'cause they're blocked by own pieces
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(0, 24));
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(63, 39));

    // Rooks can't move to the same color piece squares
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(0, 8));
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(0, 1));
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(63, 62));
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(63, 55));

    // Rooks can move only withing board
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(63, 71));
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(0, -8));

    // Rooks can't move through enemy pieces
    BIT_BOARD[8] = EMPTY;
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(0, 56));
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(56, 0));

    // Check moves along the rank
    BIT_BOARD[48] = EMPTY;
    TEST_ASSERT_TRUE(isPseudoLegalRookMove(0, 56));
    TEST_ASSERT_TRUE(isPseudoLegalRookMove(0, 8));

    // Check moves along the file
    BIT_BOARD[39] = WHITE_ROOK;
    TEST_ASSERT_TRUE(isPseudoLegalRookMove(39, 32));
    TEST_ASSERT_TRUE(isPseudoLegalRookMove(39, 38));

    // Rook can not move through pieces
    BIT_BOARD[36] = BLACK_ROOK;
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(39, 35));
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(39, 32));
    TEST_ASSERT_TRUE(isPseudoLegalRookMove(39, 36));
    BIT_BOARD[39] = EMPTY;
    BIT_BOARD[32] = WHITE_ROOK;
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(32, 37));
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(32, 39));
    TEST_ASSERT_TRUE(isPseudoLegalRookMove(32, 36));

    // Check moves along the file
    BIT_BOARD[36] = EMPTY;
    TEST_ASSERT_TRUE(isPseudoLegalRookMove(32, 39));
    TEST_ASSERT_TRUE(isPseudoLegalRookMove(32, 33));

    // Restore board state
    BIT_BOARD[32] = EMPTY;
    BIT_BOARD[48] = BLACK_PAWN;
    BIT_BOARD[8] = WHITE_PAWN;
}

// Test isPseudoLegalQueenMove function
void should_check_pseudo_legal_queen_move(void) {
    // Queen can not move outside the board
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(3, -11));
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(59, 67));
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(3, -12));

    // Clear the board
    clearBoard();

    // Place two test queens
    BIT_BOARD[0] = WHITE_QUEEN;
    BIT_BOARD[7] = BLACK_QUEEN;

    // Check rank moves
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(0, 7));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(0, 6));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(0, 1));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(7, 0));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(7, 1));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(7, 6));

    // Can not move through pieces (along the rank)
    BIT_BOARD[4] = WHITE_ROOK; // Place the piece in the middle of the line
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(0, 5));
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(7, 3));

    // Check file moves
    BIT_BOARD[56] = BLACK_QUEEN; // Place test queen
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(0, 56));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(0, 48));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(0, 8));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(56, 48));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(56, 8));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(56, 0));

    // Can not move through pieces (along the file)
    BIT_BOARD[32] = WHITE_ROOK; // Place the piece in the middle of the line
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(0, 40));
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(56, 24));

    // Check first diagonal moves
    BIT_BOARD[63] = BLACK_QUEEN; // Place test queen
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(0, 63));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(0, 54));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(0, 9));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(63, 54));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(63, 9));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(63, 0));

    // Can not move through pieces (along the first diagonal)
    BIT_BOARD[36] = BLACK_PAWN; // Place the piece in the middle of the line
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(0, 45));
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(63, 27));

    // Check second diagonal moves
    BIT_BOARD[7] = WHITE_QUEEN; // Place test queen
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(7, 56));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(7, 49));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(7, 14));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(56, 49));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(56, 14));
    TEST_ASSERT_TRUE(isPseudoLegalQueenMove(56, 7));

    // Can not move through pieces (along the second diagonal)
    BIT_BOARD[35] = BLACK_PAWN; // Place the piece in the middle of the line
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(7, 42));
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(56, 28));

    // Restore the board
    resetBitBoard();
}

// Test isPseudoLegalMove function
void should_check_pseudo_legal_move(void) {
    // test knight
    TEST_ASSERT_TRUE(isPseudoLegalKnightMove(1, 18));
    BIT_BOARD[1] = WHITE_QUEEN;
    TEST_ASSERT_FALSE(isPseudoLegalQueenMove(1, 18));
    BIT_BOARD[1] = WHITE_BISHOP;
    TEST_ASSERT_FALSE(isPseudoLegalBishopMove(1, 18));
    BIT_BOARD[1] = WHITE_ROOK;
    TEST_ASSERT_FALSE(isPseudoLegalRookMove(1, 18));
    BIT_BOARD[1] = WHITE_KING;
    TEST_ASSERT_FALSE(isPseudoLegalKingMove(1, 18));
    BIT_BOARD[1] = WHITE_PAWN;
    TEST_ASSERT_FALSE(isPseudoLegalKingMove(1, 18));

    // Place white knight back
    BIT_BOARD[1] = WHITE_KNIGHT;

    // test king
    BIT_BOARD[12] = EMPTY;
    TEST_ASSERT_FALSE(isPseudoLegalMove(4, 20));
    BIT_BOARD[4] = WHITE_QUEEN;
    TEST_ASSERT_TRUE(isPseudoLegalMove(4, 20));
    BIT_BOARD[4] = WHITE_ROOK;
    TEST_ASSERT_TRUE(isPseudoLegalMove(4, 20));

    BIT_BOARD[13] = EMPTY;
    BIT_BOARD[4] = WHITE_KING;
    TEST_ASSERT_FALSE(isPseudoLegalMove(4, 22));
    BIT_BOARD[4] = WHITE_BISHOP;
    TEST_ASSERT_TRUE(isPseudoLegalMove(4, 22));

    BIT_BOARD[12] = WHITE_KING;
    TEST_ASSERT_TRUE(isPseudoLegalMove(12, 21));

    BIT_BOARD[12] = WHITE_PAWN;
    BIT_BOARD[13] = WHITE_PAWN;
    BIT_BOARD[4] = WHITE_KING;
    TEST_ASSERT_FALSE(isPseudoLegalMove(12, 21));

    // test rook
    BIT_BOARD[19] = BLACK_PAWN;
    TEST_ASSERT_TRUE(isPseudoLegalMove(10, 19));
    BIT_BOARD[10] = WHITE_ROOK;
    TEST_ASSERT_FALSE(isPseudoLegalMove(10, 19));
    BIT_BOARD[10] = WHITE_BISHOP;
    TEST_ASSERT_TRUE(isPseudoLegalMove(10, 19));
    BIT_BOARD[10] = WHITE_QUEEN;
    TEST_ASSERT_TRUE(isPseudoLegalMove(10, 19));
    BIT_BOARD[10] = WHITE_PAWN;
    BIT_BOARD[19] = EMPTY;

    // test bishop
    TEST_ASSERT_TRUE(isPseudoLegalMove(10, 18));
    BIT_BOARD[10] = WHITE_QUEEN;
    TEST_ASSERT_TRUE(isPseudoLegalMove(10, 18));
    BIT_BOARD[10] = WHITE_BISHOP;
    TEST_ASSERT_FALSE(isPseudoLegalMove(10, 18));
    BIT_BOARD[10] = WHITE_PAWN;

    // test queen
    TEST_ASSERT_FALSE(isPseudoLegalMove(10, 42));
    BIT_BOARD[10] = WHITE_QUEEN;
    TEST_ASSERT_TRUE(isPseudoLegalMove(10, 42));
    BIT_BOARD[10] = WHITE_PAWN;

    // There is no need to test pawn since we already tested it with each piece separately
}

// Test canAttackTheKing function with knights
void knight_should_attack_the_king(void) {
    int testWhiteKingSquare = 27;
    // place white king on d4
    BIT_BOARD[whiteKingSquare] = EMPTY;
    BIT_BOARD[testWhiteKingSquare] = WHITE_KING;

    // Check if black knight can attack the white king
    for (int offset: KNIGHT_MOVE_OFFSETS) {
        int testSquare = testWhiteKingSquare + offset;
        // place black knight on the test square
        BIT_BOARD[testSquare] = BLACK_KNIGHT;

        TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testWhiteKingSquare));

        // clear the square
        BIT_BOARD[testSquare] = EMPTY;
    }

    // place white king back
    BIT_BOARD[whiteKingSquare] = WHITE_KING;
    BIT_BOARD[testWhiteKingSquare] = EMPTY;
}

// Test canAttackTheKing function with bishops and queens
void bishop_should_attack_the_king(void) {
    // here we will test the shortest and middle range
    const int testSquares[8] = {9, 13, 54, 41, 34, 36, 18, 20}; // b2, f2, g7, b6, c5, e5, c3, e3
    int testWhiteKingSquare = 27;
    // place white king on d4
    BIT_BOARD[whiteKingSquare] = EMPTY;
    BIT_BOARD[testWhiteKingSquare] = WHITE_KING;

    // Check if black bishop can attack the white king
    for (int testSquare: testSquares) {
        // place black bishop on the test square
        BIT_BOARD[testSquare] = BLACK_BISHOP;

        // test bishop attack
        TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testWhiteKingSquare));

        // test queen attack
        BIT_BOARD[testSquare] = BLACK_QUEEN;
        TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testWhiteKingSquare));

        // clear the square
        BIT_BOARD[testSquare] = EMPTY;
    }

    // place white king back
    BIT_BOARD[whiteKingSquare] = WHITE_KING;
    BIT_BOARD[testWhiteKingSquare] = EMPTY;

    // here we test the longest attack range
    testWhiteKingSquare = 0;
    int testBlackKingSquare = 63;

    // Remove pawns
    BIT_BOARD[9] = EMPTY;
    BIT_BOARD[54] = EMPTY;

    BIT_BOARD[whiteKingSquare] = EMPTY;
    BIT_BOARD[testWhiteKingSquare] = WHITE_KING;

    BIT_BOARD[63] = BLACK_BISHOP;
    TEST_ASSERT_TRUE(canAttackTheKing(63, testWhiteKingSquare));

    BIT_BOARD[63] = BLACK_QUEEN;
    TEST_ASSERT_TRUE(canAttackTheKing(63, testWhiteKingSquare));

    // place black king on the target square
    BIT_BOARD[blackKingSquare] = EMPTY;
    BIT_BOARD[testBlackKingSquare] = BLACK_KING;

    BIT_BOARD[0] = WHITE_BISHOP;
    TEST_ASSERT_TRUE(canAttackTheKing(0, testBlackKingSquare));

    BIT_BOARD[0] = WHITE_QUEEN;
    TEST_ASSERT_TRUE(canAttackTheKing(0, testBlackKingSquare));

    // Reset the board
    resetBitBoard();

    testWhiteKingSquare = 7;
    testBlackKingSquare = 56;

    // Remove pawns
    BIT_BOARD[14] = EMPTY;
    BIT_BOARD[49] = EMPTY;

    BIT_BOARD[whiteKingSquare] = EMPTY;
    BIT_BOARD[testWhiteKingSquare] = WHITE_KING;

    BIT_BOARD[56] = BLACK_BISHOP;
    TEST_ASSERT_TRUE(canAttackTheKing(56, testWhiteKingSquare));

    BIT_BOARD[56] = BLACK_QUEEN;
    TEST_ASSERT_TRUE(canAttackTheKing(56, testWhiteKingSquare));

    // place black king on the target square
    BIT_BOARD[blackKingSquare] = EMPTY;
    BIT_BOARD[testBlackKingSquare] = BLACK_KING;

    BIT_BOARD[7] = WHITE_BISHOP;
    TEST_ASSERT_TRUE(canAttackTheKing(7, testBlackKingSquare));

    BIT_BOARD[7] = WHITE_QUEEN;
    TEST_ASSERT_TRUE(canAttackTheKing(7, testBlackKingSquare));

    resetBitBoard();
}

// Test canAttackTheKing function with rooks and queens
void rook_should_attack_the_king(void) {
    // here we will test the shortest and middle range
    const int testSquares[8] = {19, 26, 35, 28, 11, 24, 31, 51}; // d3, c4, d5, e4, d2, a4, h4, d7
    int testWhiteKingSquare = 27;
    // place white king on d4
    BIT_BOARD[whiteKingSquare] = EMPTY;
    BIT_BOARD[testWhiteKingSquare] = WHITE_KING;

    // Check if black rook can attack the white king
    for (int testSquare: testSquares) {
        // place black rook on the test square
        BIT_BOARD[testSquare] = BLACK_ROOK;

        // test rook attack
        TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testWhiteKingSquare));

        // test queen attack
        BIT_BOARD[testSquare] = BLACK_QUEEN;
        TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testWhiteKingSquare));

        // clear the square
        BIT_BOARD[testSquare] = EMPTY;
    }

    // place white king back
    BIT_BOARD[whiteKingSquare] = WHITE_KING;
    BIT_BOARD[testWhiteKingSquare] = EMPTY;

    // here we test the longest attack range
    clearBoard();
    int testSquare = blackKingSquare;
    int testBlackKingSquare = blackKingSquare + 1;

    BIT_BOARD[whiteKingSquare] = WHITE_KING;
    BIT_BOARD[testBlackKingSquare] = BLACK_KING;

    BIT_BOARD[testSquare] = BLACK_ROOK;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, whiteKingSquare));

    BIT_BOARD[testSquare] = BLACK_QUEEN;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, whiteKingSquare));

    testSquare = whiteKingSquare + 1;

    BIT_BOARD[testSquare] = WHITE_ROOK;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testBlackKingSquare));

    BIT_BOARD[testSquare] = WHITE_QUEEN;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testBlackKingSquare));

    resetBitBoard();

    testWhiteKingSquare = 24;
    testBlackKingSquare = 39;
    testSquare = 31;

    BIT_BOARD[testWhiteKingSquare] = WHITE_KING;
    BIT_BOARD[testBlackKingSquare] = BLACK_KING;

    BIT_BOARD[testSquare] = BLACK_ROOK;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testWhiteKingSquare));

    BIT_BOARD[testSquare] = BLACK_QUEEN;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testWhiteKingSquare));

    testSquare = 32;

    BIT_BOARD[testSquare] = WHITE_ROOK;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testBlackKingSquare));

    BIT_BOARD[testSquare] = WHITE_QUEEN;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, testBlackKingSquare));

    resetBitBoard();
}

// Test canAttackTheKing function with pawn
void pawn_should_attack_the_king(void) {
    int testSquare = 51;
    BIT_BOARD[testSquare] = WHITE_PAWN;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, blackKingSquare));

    testSquare = 53;
    BIT_BOARD[testSquare] = WHITE_PAWN;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, blackKingSquare));

    testSquare = 11;
    BIT_BOARD[testSquare] = BLACK_PAWN;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, whiteKingSquare));

    testSquare = 13;
    BIT_BOARD[testSquare] = BLACK_PAWN;
    TEST_ASSERT_TRUE(canAttackTheKing(testSquare, whiteKingSquare));

    resetBitBoard();
}

// Test canAttackTheKing function with knights
void knight_should_not_attack_the_king(void) {
    int testSquare = 24;
    int testWhiteKingSquare = 39;

    BIT_BOARD[whiteKingSquare] = EMPTY;
    BIT_BOARD[testWhiteKingSquare] = WHITE_KING;
    BIT_BOARD[testSquare] = BLACK_KNIGHT;
    TEST_ASSERT_FALSE(canAttackTheKing(testSquare, testWhiteKingSquare));

    testSquare = 39;
    testWhiteKingSquare = 24;

    BIT_BOARD[testWhiteKingSquare] = WHITE_KING;
    BIT_BOARD[testSquare] = BLACK_KNIGHT;
    TEST_ASSERT_FALSE(canAttackTheKing(testSquare, testWhiteKingSquare));

    resetBitBoard();
}

// Test canAttackTheKing function with queens
void queen_should_not_attack_the_king(void) {
    int testKingSquare = 27; // d4
    const int testSquares[14] = {41, 45, 9, 13, 11, 24, 31, 51, 16, 39, 50, 13, -1, 65};
    // place white king on d4
    BIT_BOARD[whiteKingSquare] = EMPTY;
    BIT_BOARD[testKingSquare] = WHITE_KING;

    BIT_BOARD[19] = WHITE_PAWN;
    BIT_BOARD[26] = WHITE_PAWN;
    BIT_BOARD[35] = WHITE_PAWN;
    BIT_BOARD[28] = WHITE_PAWN;
    BIT_BOARD[34] = WHITE_PAWN;
    BIT_BOARD[36] = WHITE_PAWN;
    BIT_BOARD[18] = WHITE_PAWN;
    BIT_BOARD[20] = WHITE_PAWN;

    // Check if black rook can attack the white king
    for (int testSquare: testSquares) {
        // place black rook on the test square
        BIT_BOARD[testSquare] = BLACK_ROOK;

        // test rook attack
        TEST_ASSERT_FALSE(canAttackTheKing(testSquare, testKingSquare));

        // test queen attack
        BIT_BOARD[testSquare] = BLACK_QUEEN;
        TEST_ASSERT_FALSE(canAttackTheKing(testSquare, testKingSquare));

        // test bishop attack
        BIT_BOARD[testSquare] = BLACK_BISHOP;
        TEST_ASSERT_FALSE(canAttackTheKing(testSquare, testKingSquare));

        // clear the square
        BIT_BOARD[testSquare] = EMPTY;
    }

    resetBitBoard();
}

// Test canAttackTheKing function with pawns
void pawn_should_not_attack_the_king(void) {
    int testKingSquare = 39;
    int testSquare = 48;

    BIT_BOARD[whiteKingSquare] = EMPTY;
    BIT_BOARD[testKingSquare] = WHITE_KING;

    TEST_ASSERT_FALSE(canAttackTheKing(testSquare, testKingSquare));

    resetBitBoard();

    testSquare = 15;
    testKingSquare = 24;

    BIT_BOARD[blackKingSquare] = EMPTY;
    BIT_BOARD[testKingSquare] = BLACK_KING;

    TEST_ASSERT_FALSE(canAttackTheKing(testSquare, testKingSquare));

    resetBitBoard();
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(square_should_be_withing_the_board);

    RUN_TEST(should_be_on_the_same_diagonal);
    RUN_TEST(should_not_be_on_the_same_diagonal);

    RUN_TEST(should_be_on_the_same_first_diagonal);
    RUN_TEST(should_not_be_on_the_same_first_diagonal);

    RUN_TEST(should_be_on_the_same_second_diagonal);
    RUN_TEST(should_not_be_on_the_same_second_diagonal);

    RUN_TEST(should_be_on_the_same_rank);
    RUN_TEST(should_not_be_on_the_same_rank);

    RUN_TEST(should_be_on_the_same_file);
    RUN_TEST(should_not_be_on_the_same_file);

    // test for unexpected input for on the same line functions
    RUN_TEST(should_handle_unexpected_input);

    // tests for isPseudoLegal function
    RUN_TEST(should_check_pseudo_legal_knight_move);
    RUN_TEST(should_check_pseudo_legal_pawn_move);
    RUN_TEST(should_check_pseudo_legal_king_move);
    RUN_TEST(should_check_pseudo_legal_bishop_move);
    RUN_TEST(should_check_pseudo_legal_rook_move);
    RUN_TEST(should_check_pseudo_legal_queen_move);
    RUN_TEST(should_check_pseudo_legal_move);

    // tests for canAttackTheKing function
    RUN_TEST(knight_should_attack_the_king);
    RUN_TEST(bishop_should_attack_the_king);
    RUN_TEST(rook_should_attack_the_king);
    RUN_TEST(pawn_should_attack_the_king);

    RUN_TEST(knight_should_not_attack_the_king);
    RUN_TEST(queen_should_not_attack_the_king);
    RUN_TEST(pawn_should_not_attack_the_king);

    return UNITY_END();
}

void setup() {
    runUnityTests();
}

void loop() {

}