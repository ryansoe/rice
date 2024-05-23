// makemove.cpp

#include "defs.h"

#define HASH_PCE(pce,sq) (pos->posKey ^= (pieceKeys[(pce)][(sq)]))
#define HASH_CA (pos->posKey ^= (castleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= (sideKey))
#define HASH_EP (pos->posKey ^= (pieceKeys[EMPTY][pos->enPas]))

const int castlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

static void clearPiece(const int sq, S_BOARD *pos) {
    ASSERT(sqOnBoard(sq));
    
    int piece = pos->pieces[sq];

    ASSERT(pieceValid(piece));

    int col = PieceCol[piece];
    int index = 0;
    int tempPieceNum = -1;

    HASH_PCE(piece, sq);

    pos->pieces[sq] = EMPTY;
    pos->material[col] -= PieceVal[piece];

    if(PieceBig[piece]) {
        pos->bigPiece[col]--;
        if(PieceMaj[piece]) {
            pos->majorPiece[col]--;
        } else {
            pos->minorPiece[col]--;
        }
    } else {
        CLRBIT(pos->pawns[col],SQ64(sq));
        CLRBIT(pos->pawns[BOTH],SQ64(sq));
    }

    /*
        pos->pieceNum[wP] == 5 (Looping from 0 to 4)
        pos->pieceList[piece][0] = sq0
        pos->pieceList[piece][1] = sq1
        pos->pieceList[piece][2] = sq2
        pos->pieceList[piece][3] = sq3
        pos->pieceList[piece][4] = sq4

        sq==sq3 so tempPieceNum = 3;
    */
    for(index = 0; index < pos->pieceNum[piece]; ++index) {
        if(pos->pieceList[piece][index] == sq) {
            tempPieceNum = index;
            break;
        }
    }

    ASSERT(tempPieceNum != -1);

    pos->pieceNum[piece]--;
    // pos->pieceNum[wP] == 4

    pos->pieceList[piece][tempPieceNum] = pos->pieceList[piece][pos->pieceNum[piece]];
    //pos->pieceList[wP][3]     = pos->pieceList[wP][4] = sq4
    /*
        pos->pieceNum[wP] == 4 (Looping from 0 to 3)
        pos->pieceList[piece][0] = sq0
        pos->pieceList[piece][1] = sq1
        pos->pieceList[piece][2] = sq2
        pos->pieceList[piece][3] = sq4

    */
}


static void addPiece(const int sq, S_BOARD *pos, const int piece) {

    ASSERT(pieceValid(piece));
    ASSERT(sqOnBoard(sq));
	
	int col = PieceCol[piece];
	ASSERT(sideValid(col));

    HASH_PCE(piece,sq);
	
	pos->pieces[sq] = piece;

    if(PieceBig[piece]) {
			pos->bigPiece[col]++;
		if(PieceMaj[piece]) {
			pos->majorPiece[col]++;
		} else {
			pos->minorPiece[col]++;
		}
	} else {
		SETBIT(pos->pawns[col],SQ64(sq));
		SETBIT(pos->pawns[BOTH],SQ64(sq));
	}
	
	pos->material[col] += PieceVal[piece];
	pos->pieceList[piece][pos->pieceNum[piece]++] = sq;
	
}


static void movePiece(const int from, const int to, S_BOARD *pos) {

    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));
	
	int index = 0;
	int pce = pos->pieces[from];	
	int col = PieceCol[pce];
	ASSERT(sideValid(col));
    ASSERT(pieceValid(pce));
	
#ifdef DEBUG
	int t_PieceNum = FALSE;
#endif

	HASH_PCE(pce,from);
	pos->pieces[from] = EMPTY;
	
	HASH_PCE(pce,to);
	pos->pieces[to] = pce;
	
	if(!PieceBig[pce]) {
		CLRBIT(pos->pawns[col],SQ64(from));
		CLRBIT(pos->pawns[BOTH],SQ64(from));
		SETBIT(pos->pawns[col],SQ64(to));
		SETBIT(pos->pawns[BOTH],SQ64(to));		
	}    
	
	for(index = 0; index < pos->pieceNum[pce]; ++index) {
		if(pos->pieceList[pce][index] == from) {
			pos->pieceList[pce][index] = to;
#ifdef DEBUG
			t_PieceNum = TRUE;
#endif
			break;
		}
	}
	ASSERT(t_PieceNum);
}

int makeMove(S_BOARD *pos, int move) {
    
    ASSERT(checkBoard(pos));
    

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;

    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));
    ASSERT(sideValid(side));
    ASSERT(pieceValid(pos->pieces[from]));

    pos->history[pos->hisPlay].posKey = pos->posKey;

    // clear enemy pawn if move is en pas
    if(move & MFLAGEP) {
        if(side == WHITE) {
            clearPiece(to-10, pos);
        } else {
            clearPiece(to+10, pos);
        }
    } else if (move & MFLAGCA) {    //if castling move, move the rook
        switch(to) {
            case C1:
                movePiece(A1, D1, pos);
                break;
            case C8:
                movePiece(A8, D8, pos);
                break;
            case G1:
                movePiece(H1, F1, pos);
                break;
            case G8:
                movePiece(H8, F8, pos);
                break;
            default:    ASSERT(FALSE); break;
        }
    }

    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->history[pos->hisPlay].move = move;
    pos->history[pos->hisPlay].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPlay].enPas = pos->enPas;
    pos->history[pos->hisPlay].castlePerm = pos->castlePerm;

    pos->castlePerm &= castlePerm[from];
    pos->castlePerm &= castlePerm[to];
    pos->enPas = NO_SQ;

    HASH_CA;


    int captured = CAPTURED(move);
    pos->fiftyMove++;

    // set fifty move count to zero if capture is made
    if(captured != EMPTY) {
        ASSERT(pieceValid(captured));
        clearPiece(to, pos);
        pos->fiftyMove = 0;
    }

    pos->hisPlay++;
    pos->play++;

    // check if we need to set a new en pas square
    if(piecePawn[pos->pieces[from]]) {
        pos->fiftyMove = 0;
        if(move & MFLAGPS) {
            if(side==WHITE) {
                pos->enPas=from+10;
                ASSERT(ranksBoard[pos->enPas]==RANK_3);
            } else {
                pos->enPas=from-10;
                ASSERT(ranksBoard[pos->enPas]==RANK_6);
            }
            HASH_EP;
        }
    }

    movePiece(from, to, pos);

    // promoted piece
    int promPiece = PROMOTED(move);
    if (promPiece != EMPTY) {
        ASSERT(pieceValid(promPiece) && !piecePawn[promPiece]);
        clearPiece(to, pos);
        addPiece(to, pos, promPiece);
    }

    if(pieceKing[pos->pieces[to]]) {
        pos->kingSq[pos->side] = to;
    }

    pos->side ^= 1;
    HASH_SIDE;

    
    ASSERT(checkBoard(pos));
    

    // move is invalid if king is in check
    if(sqAttacked(pos->kingSq[side],pos->side,pos)) {
        takeMove(pos);
        return FALSE;
    }

    return TRUE;
}


void takeMove(S_BOARD *pos) {
    ASSERT(checkBoard(pos));

    pos->hisPlay--;
    pos->play--;

    int move = pos->history[pos->hisPlay].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);

    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));

    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->castlePerm = pos->history[pos->hisPlay].castlePerm;
    pos->fiftyMove = pos->history[pos->hisPlay].fiftyMove;
    pos->enPas = pos->history[pos->hisPlay].enPas;

    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->side ^= 1;
    HASH_SIDE;

    if(MFLAGEP & move) {
        if(pos->side == WHITE) {
            addPiece(to-10, pos, bP);
        } else {
            addPiece(to+10, pos, wP);
        }
    } else if(MFLAGCA & move) {
        switch(to) {
            case C1: movePiece(D1, A1, pos);    break;
            case C8: movePiece(D8, A8, pos);    break;
            case G1: movePiece(F1, H1, pos);    break;
            case G8: movePiece(F8, H8, pos);    break;
            default: ASSERT(FALSE); break;
        }
    }

    movePiece(to, from, pos);

    if(pieceKing[pos->pieces[from]]) {
        pos->kingSq[pos->side] = from;
    }

    // add piece back if captured
    int captured = CAPTURED(move);
    if(captured != EMPTY) {
        ASSERT(pieceValid(captured));
        addPiece(to, pos, captured);
    }

    if(PROMOTED(move) != EMPTY) {
        ASSERT(pieceValid(PROMOTED(move)) && !piecePawn[PROMOTED(move)]);
        clearPiece(from, pos);
        addPiece(from, pos, (PieceCol[PROMOTED(move)] == WHITE ? wP : bP));
    }

    ASSERT(checkBoard(pos));
}
