#include<iostream>
#include "defs.h"
#include "stdio.h"
#include "stdlib.h"

// #include "bitboards.cpp"
// #include "init.cpp"
// #include "board.cpp"
// #include "data.cpp"
// #include "hashkeys.cpp"
// #include "attack.cpp"

#define PERFT "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN1 "8/8/3q4/2PPP3/5P2/4Q3/8/8 w - - 0 2"

int main() {
    allInit();
    
    S_BOARD board[1];

    parseFen(PERFT, board);
    printBoard(board);

    return 0;
}

