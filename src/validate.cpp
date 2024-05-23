// validate.cpp

#include "defs.h"

int sqOnBoard(const int sq) {
    return filesBoard[sq]==OFFBOARD ? 0 : 1;
}

int sideValid(const int side) {
    return (side==WHITE || side == BLACK) ? 1 : 0;
}

int fileRankValid(const int fr) {
    return (fr >= 0 && fr <= 7) ? 1 : 0;
}

int pieceValidEmpty(const int piece) {
    return (piece >= EMPTY && piece <= bK) ? 1 : 0;
}

int pieceValid(const int piece) {
    return (piece >= wP && piece <= bK) ? 1 : 0;
}
