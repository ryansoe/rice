// attack.cpp

#include "defs.h"
#include "stdlib.h"

const int knightDirection[8] = { -8, -19, -21, -12, 8, 19, 21, 12};
const int rookDirection[4] = { -1, -10, 1, 10 };
const int bishopDirection[4] = { -9, -11, 11, 9 };
const int kingDirection[8] = { -1, -10, 1, 10, -9, -11, 11, 9};

int sqAttacked(const int sq, const int side, const S_BOARD *pos) {
    // note: "side" is the attacking side
    int piece,index,tempSq,dir;

    ASSERT(sqOnBoard(sq));
    ASSERT(sideValid(side));
    ASSERT(checkBoard(pos));

    // pawns
    if(side == WHITE) {
        if(pos->pieces[sq-11] == wP || pos->pieces[sq-9] == wP) {
            return TRUE;
        }
    } else {
        if(pos->pieces[sq+11] == bP || pos->pieces[sq+9] == bP) {
            return TRUE;
        }
    }

    // knights
    for(index = 0; index < 8; ++index) {
        piece = pos->pieces[sq + knightDirection[index]];
        if(piece != OFFBOARD && IsKn(piece) && PieceCol[piece]==side) {
            return TRUE;
        }
    }

    // rooks, queens
    for(index = 0; index < 4; ++index) {
        dir = rookDirection[index];
        tempSq = sq + dir;
        piece = pos->pieces[tempSq];
        while (piece != OFFBOARD) {
            if(piece != EMPTY) {
                if(IsRQ(piece) && PieceCol[piece] == side) {
                    return TRUE;
                }
                break;
            }
            tempSq += dir;
            piece = pos->pieces[tempSq];
        }
    }

    // bishop, queens
    for(index = 0; index < 4; ++index) {
        dir = bishopDirection[index];
        tempSq = sq + dir;
        piece = pos->pieces[tempSq];
        while (piece != OFFBOARD) {
            if(piece != EMPTY) {
                if(IsBQ(piece) && PieceCol[piece] == side) {
                    return TRUE;
                }
                break;
            }
            tempSq += dir;
            piece = pos->pieces[tempSq];
        }
    }

    // kings
    for(index = 0; index < 8; ++index) {
        piece = pos->pieces[sq + kingDirection[index]];
        if(piece != OFFBOARD && IsKi(piece) && PieceCol[piece]==side) {
            //printf("king in check\n");
            return TRUE;
        }
    }
    //printf("king not attacked\n");
    return FALSE;
}
