// evaluate.cpp

#include "stdio.h"
#include "defs.h"

const int pawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int knightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

const int bishopTable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int rookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

int evalPosition(const S_BOARD *pos) {
    int piece;
    int pieceNum;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];

    piece = wP;
    for(pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
        sq = pos->pieceList[piece][pieceNum];
        ASSERT(sqOnBoard(sq));
        score += pawnTable[SQ64(sq)];
    }

    piece = bP;
    for(pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
        sq = pos->pieceList[piece][pieceNum];
        ASSERT(sqOnBoard(sq));
        score -= pawnTable[MIRROR64(SQ64(sq))];
    }

    piece = wB;
    for(pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
        sq = pos->pieceList[piece][pieceNum];
        ASSERT(sqOnBoard(sq));
        score += bishopTable[SQ64(sq)];
    }

    piece = bB;
    for(pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
        sq = pos->pieceList[piece][pieceNum];
        ASSERT(sqOnBoard(sq));
        score -= bishopTable[MIRROR64(SQ64(sq))];
    }

    piece = wR;
    for(pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
        sq = pos->pieceList[piece][pieceNum];
        ASSERT(sqOnBoard(sq));
        score += rookTable[SQ64(sq)];
    }

    piece = bR;
    for(pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
        sq = pos->pieceList[piece][pieceNum];
        ASSERT(sqOnBoard(sq));
        score -= rookTable[MIRROR64(SQ64(sq))];
    }

    if(pos->side == WHITE) {
        return score;
    } else {
        return -score;
    }

    return 0;
}