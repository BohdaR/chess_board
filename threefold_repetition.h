#ifndef THREEFOLD_REPETITION_H
#define THREEFOLD_REPETITION_H

#include "constants.h"
#include "helpers.h"

void resetPositionTracking() {
    // Reset position tracking by clearing the array and resetting the index
    memset(positionsHistory, 0, sizeof(positionsHistory));
    currentPositionIndex = -1;
}

unsigned long calculatePositionHash() {
    unsigned long hash = 5381;

    for (int i = 0; i < 64; ++i) {
        hash = ((hash << 5) + hash) ^ BIT_BOARD[i];
    }

    hash = ((hash << 5) + hash) ^ PositionDynamics.castlingRights;
    hash = ((hash << 5) + hash) ^ PositionDynamics.enPassantSquare;

    return hash;
}

void savePosition() {
    currentPositionIndex++;
    // Save the current position to the array
    positionsHistory[currentPositionIndex] = calculatePositionHash();
}

bool isTreeFoldRepetition() {
    // This function goes through each of previous positions and check whether current position has occurred before
    int repetitions = 0;

    // Check how many times the current position has occurred
    for (int i = 0; i < MAX_POSITIONS; i++) {
        // If positionsHistory[i] equals current position we increment the repetition counter
        if (positionsHistory[i] == positionsHistory[currentPositionIndex]) {
            repetitions++;
        }
    }

    // Threefold repetition if the current position has occurred two times previously
    return repetitions > 2;
}

#endif
