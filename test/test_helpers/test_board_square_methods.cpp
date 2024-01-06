#include "unity.h"
#include "../../include/helpers.h"

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  PositionDynamics.whoseMove = WHITE;
}

// Test getFileLetter function
void should_return_square_file(void) {
  TEST_ASSERT_EQUAL('a', getFileLetter(0));
  TEST_ASSERT_EQUAL('b', getFileLetter(1));
  TEST_ASSERT_EQUAL('c', getFileLetter(2));
  TEST_ASSERT_EQUAL('d', getFileLetter(3));
  TEST_ASSERT_EQUAL('e', getFileLetter(4));
  TEST_ASSERT_EQUAL('f', getFileLetter(5));
  TEST_ASSERT_EQUAL('g', getFileLetter(6));
  TEST_ASSERT_EQUAL('h', getFileLetter(7));
}

// Test getRankNumber function
void should_return_rank_number(void) {
  TEST_ASSERT_EQUAL(1, getRankNumber(0));
  TEST_ASSERT_EQUAL(2, getRankNumber(8));
  TEST_ASSERT_EQUAL(3, getRankNumber(18));
  TEST_ASSERT_EQUAL(4, getRankNumber(29));
  TEST_ASSERT_EQUAL(5, getRankNumber(35));
  TEST_ASSERT_EQUAL(6, getRankNumber(44));
  TEST_ASSERT_EQUAL(7, getRankNumber(50));
  TEST_ASSERT_EQUAL(8, getRankNumber(62));
}

// Test getPieceColor function
void should_return_piece_color(void) {
  TEST_ASSERT_EQUAL(WHITE, getPieceColor(BIT_BOARD[0]));
  TEST_ASSERT_EQUAL(WHITE, getPieceColor(BIT_BOARD[5]));
  TEST_ASSERT_EQUAL(BLACK, getPieceColor(BIT_BOARD[55]));
  TEST_ASSERT_EQUAL(BLACK, getPieceColor(BIT_BOARD[63]));
  // for square without piece should return 0
  TEST_ASSERT_EQUAL(EMPTY, getPieceColor(BIT_BOARD[32]));
}

// Test getOppositeColorKingSquare function
void should_return_opposite_king_square(void) {
  PositionDynamics.whoseMove = WHITE;
  TEST_ASSERT_EQUAL(60, getOppositeColorKingSquare());

  PositionDynamics.whoseMove = BLACK;
  TEST_ASSERT_EQUAL(4, getOppositeColorKingSquare());
}

// Test getOwnKingSquare function
void should_return_own_king_square(void) {
  PositionDynamics.whoseMove = BLACK;
  TEST_ASSERT_EQUAL(60, getOwnKingSquare());
  PositionDynamics.whoseMove = WHITE;
  TEST_ASSERT_EQUAL(4, getOwnKingSquare());
}

// Test toggleWhoseMove function
void should_toggle_whose_move(void) {
  TEST_ASSERT_EQUAL(WHITE, PositionDynamics.whoseMove);
  
  toggleWhoseMove();
  TEST_ASSERT_EQUAL(BLACK, PositionDynamics.whoseMove);

  toggleWhoseMove();
  TEST_ASSERT_EQUAL(WHITE, PositionDynamics.whoseMove);
}

// Test resetPositionDynamics function
void should_reset_position_dynamics() {
  PositionDynamics.pickedSquare = 10;
  PositionDynamics.attackedPieceSquare = 5;
  PositionDynamics.moveTypes.capture = true;
  PositionDynamics.moveTypes.check = true;
  PositionDynamics.moveTypes.promotion = true;

  TEST_ASSERT_EQUAL(10, PositionDynamics.pickedSquare);
  TEST_ASSERT_EQUAL(5, PositionDynamics.attackedPieceSquare);
  TEST_ASSERT_TRUE(PositionDynamics.moveTypes.capture);
  TEST_ASSERT_TRUE(PositionDynamics.moveTypes.check);
  TEST_ASSERT_TRUE(PositionDynamics.moveTypes.promotion);

  resetPositionDynamics();

  TEST_ASSERT_NOT_EQUAL(10, PositionDynamics.pickedSquare);
  TEST_ASSERT_NOT_EQUAL(5, PositionDynamics.attackedPieceSquare);
  TEST_ASSERT_EQUAL(-1, PositionDynamics.pickedSquare);
  TEST_ASSERT_EQUAL(-1, PositionDynamics.attackedPieceSquare);
  TEST_ASSERT_FALSE(PositionDynamics.moveTypes.capture);
  TEST_ASSERT_FALSE(PositionDynamics.moveTypes.check);
  TEST_ASSERT_FALSE(PositionDynamics.moveTypes.promotion);
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(should_return_square_file);
  RUN_TEST(should_return_rank_number);
  RUN_TEST(should_return_piece_color);
  RUN_TEST(should_return_opposite_king_square);
  RUN_TEST(should_toggle_whose_move);
  RUN_TEST(should_return_own_king_square);
  RUN_TEST(should_reset_position_dynamics);
  return UNITY_END();
}


void setup() {
  runUnityTests();
}

void loop() {
  
}
