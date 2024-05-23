// hashkeys.cpp

#include "defs.h"
#include "stdlib.h"
using namespace std;

// creates a unique key for a position by XORing a U64 bit integer
// key holds the positions, side to move, en passant, castling perms
U64 generatePosKey(const S_BOARD *pos) {
    int sq = 0;
    U64 finalKey = 0;
    int piece = EMPTY;

    // pieces
    for(sq = 0; sq < BRD_SQ_NUM; ++sq) {
        piece = pos->pieces[sq];
        if(piece!=NO_SQ && piece!=EMPTY && piece != OFFBOARD) {
            ASSERT(piece>=wP && piece<=bK);
            finalKey ^= pieceKeys[piece][sq];
        }
    }

    if(pos->side == WHITE) {
        finalKey ^= sideKey;
    }

    if(pos->enPas != NO_SQ) {
        ASSERT(pos->enPas>=0 && pos->enPas<BRD_SQ_NUM);
        finalKey ^= pieceKeys[EMPTY][pos->enPas];
    }

    ASSERT(pos->castlePerm>=0 && pos->castlePerm<=15);

    finalKey ^= castleKeys[pos->castlePerm];

    return finalKey;
}