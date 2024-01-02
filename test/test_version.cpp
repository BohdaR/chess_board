#include "iostream"
//#include "../disambiguation.h"

char* disambiguateMove() {
    // test move
    return "Bab2";
}

int main() {
//    BIT_BOARD[56] = WHITE_QUEEN;
    const char* result = disambiguateMove();
    std::cout << result << "\n";
}
