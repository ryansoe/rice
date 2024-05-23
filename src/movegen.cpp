// movegen.cpp

#include "stdlib.h"
#include "defs.h"

#define MOVE(f,t,ca,pro,fl) ( (f) | ((t) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))
#define SQOFFBOARD(sq) (filesBoard[(sq)]==OFFBOARD)

const int loopSlidePiece[8] {
    wB, wR, wQ, 0, bB, bR, bQ, 0
};

const int loopNonSlidePiece[6] {
    wN, wK, 0, bN, bK, 0
};

const int loopSlideIndex[2] = { 0, 4 };
const int loopNonSlideIndex[2] = { 0, 3 };

const int pieceDirection[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};

const int victimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };
static int mvvLvaScore[13][13];

// most valuable victim, least valuable atacker
int initMvvLva() {
    int attacker;
    int victim;
    for(attacker = wP; attacker <= bK; ++attacker) {
        for(victim = wP; victim <= bK; ++victim) {
            mvvLvaScore[victim][attacker] = victimScore[victim] + 6 - ( victimScore[attacker] / 100);
        }
    }
}

const int numDirection[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

int moveExists(S_BOARD *pos, const int move) {
    S_MOVELIST list[1];
    generateAllMoves(pos,list);

    int moveNum = 0;
    for(moveNum = 0; moveNum < list->count; ++moveNum) {
        if( !makeMove(pos,list->moves[moveNum].move)) {
            continue;
        }
        takeMove(pos);
        if(list->moves[moveNum].move == move) {
            return TRUE;
        }
    }
    return false;
}

static void addQuietMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {

    ASSERT(sqOnBoard(FROMSQ(move)));
    ASSERT(sqOnBoard(TOSQ(move)));

    list->moves[list->count].move = move;

    if (pos->searchKillers[0][pos->play] == move) {
        list->moves[list->count].score = 900000;
    } else if (pos->searchKillers[1][pos->play] == move) {
        list->moves[list->count].score = 800000;
    } else {
        list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
    }

    list->count++;
}

static void addCaptureMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {

    ASSERT(sqOnBoard(FROMSQ(move)));
    ASSERT(sqOnBoard(TOSQ(move)));
    ASSERT(pieceValid(CAPTURED(move)));

    list->moves[list->count].move = move;
    list->moves[list->count].score = mvvLvaScore[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1000000;
    list->count++;
}

static void addEnPassantMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {

    ASSERT(sqOnBoard(FROMSQ(move)));
    ASSERT(sqOnBoard(TOSQ(move)));

    list->moves[list->count].move = move;
    list->moves[list->count].score = 105 + 1000000;
    list->count++;
}

static void addWhitePawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
    ASSERT(pieceValidEmpty(cap));
    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));

    if(ranksBoard[from] == RANK_7) {
        addCaptureMove(pos, MOVE(from,to,cap,wQ,0), list);
        addCaptureMove(pos, MOVE(from,to,cap,wR,0), list);
        addCaptureMove(pos, MOVE(from,to,cap,wB,0), list);
        addCaptureMove(pos, MOVE(from,to,cap,wN,0), list);
    } else {
        addCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
    }
}

static void addBlackPawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
    ASSERT(pieceValidEmpty(cap));
    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));

    if(ranksBoard[from] == RANK_2) {
        addCaptureMove(pos, MOVE(from,to,cap,bQ,0), list);
        addCaptureMove(pos, MOVE(from,to,cap,bR,0), list);
        addCaptureMove(pos, MOVE(from,to,cap,bB,0), list);
        addCaptureMove(pos, MOVE(from,to,cap,bN,0), list);
    } else {
        addCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
    }
}


static void addWhitePawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));

    if(ranksBoard[from] == RANK_7) {
        addQuietMove(pos, MOVE(from,to,EMPTY,wQ,0), list);
        addQuietMove(pos, MOVE(from,to,EMPTY,wR,0), list);
        addQuietMove(pos, MOVE(from,to,EMPTY,wB,0), list);
        addQuietMove(pos, MOVE(from,to,EMPTY,wN,0), list);
    } else {
        addQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
    }
}

static void addBlackPawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
    ASSERT(sqOnBoard(from));
    ASSERT(sqOnBoard(to));

    if(ranksBoard[from] == RANK_2) {
        addQuietMove(pos, MOVE(from,to,EMPTY,bQ,0), list);
        addQuietMove(pos, MOVE(from,to,EMPTY,bR,0), list);
        addQuietMove(pos, MOVE(from,to,EMPTY,bB,0), list);
        addQuietMove(pos, MOVE(from,to,EMPTY,bN,0), list);
    } else {
        addQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
    }
}

void generateAllMoves( const S_BOARD *pos, S_MOVELIST *list ) {
    ASSERT(checkBoard(pos));
    
    list->count = 0;

    int piece = EMPTY;
    int side = pos->side;
    int sq = 0; int tempSq = 0;
    int pieceNum = 0;
    int dir = 0;
    int index = 0;
    int pieceIndex = 0;
    
    if(side == WHITE) {
        // add white pawn moves
        for(pieceNum = 0; pieceNum < pos->pieceNum[wP]; ++pieceNum) {
            sq = pos->pieceList[wP][pieceNum];
            ASSERT(sqOnBoard(sq));
            if(pos->pieces[sq + 10] == EMPTY) {
                addWhitePawnMove(pos, sq, sq+10, list);
                if(ranksBoard[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
                    addQuietMove(pos, MOVE(sq,(sq+20),EMPTY,EMPTY,MFLAGPS), list);
                }
            }

            if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
                addWhitePawnCapMove(pos , sq, sq+9, pos->pieces[sq + 9], list);
            }
            if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
                addWhitePawnCapMove(pos , sq, sq+11, pos->pieces[sq + 11], list);
            }

            if(pos->enPas != NO_SQ) {
                if(sq + 9 == pos->enPas) {
                    addEnPassantMove(pos, MOVE(sq,sq + 9, EMPTY,EMPTY,MFLAGEP), list);
                }
                if(sq + 11 == pos->enPas) {
                    addEnPassantMove(pos, MOVE(sq,sq + 11, EMPTY,EMPTY,MFLAGEP), list);
                }
            }
        }

        // castling
        if(pos->castlePerm & WKCA) {
            if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
                if(!sqAttacked(E1,BLACK,pos) && !sqAttacked(F1,BLACK,pos)) {
                    addQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }

        if(pos->castlePerm & WQCA) {
            if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
                if(!sqAttacked(E1,BLACK,pos) && !sqAttacked(D1,BLACK,pos)) {
                    addQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }

    } else { // side == BLACK
		for(pieceNum = 0; pieceNum < pos->pieceNum[bP]; ++pieceNum) {
			sq = pos->pieceList[bP][pieceNum];
			ASSERT(sqOnBoard(sq));

			if(pos->pieces[sq - 10] == EMPTY) {
				addBlackPawnMove(pos, sq, sq-10, list);
				if(ranksBoard[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
					addQuietMove(pos, MOVE(sq,(sq-20),EMPTY,EMPTY,MFLAGPS),list);
				}
			}

			if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
				addBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
			}

			if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
				addBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
			}
			if(pos->enPas != NO_SQ) {
				if(sq - 9 == pos->enPas) {
					addEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEP), list);
				}
				if(sq - 11 == pos->enPas) {
					addEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEP), list);
				}
			}
		}

        // castling
        if(pos->castlePerm & BKCA) {
            if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
                if(!sqAttacked(E8,WHITE,pos) && !sqAttacked(F8,WHITE,pos)) {
                    addQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }

        if(pos->castlePerm & BQCA) {
            if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
                if(!sqAttacked(E8,WHITE,pos) && !sqAttacked(D8,WHITE,pos)) {
                    addQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
    }

    /* Loop for slide pieces */
    pieceIndex = loopSlideIndex[side];
    piece = loopSlidePiece[pieceIndex++];
    while( piece != 0) {
        ASSERT(pieceValid(piece));

            for(pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
            sq = pos->pieceList[piece][pieceNum];
            ASSERT(sqOnBoard(sq));
            //printf("Piece:%c on %s\n", pieceChar[piece], printSq(sq));

            for(index = 0; index < numDirection[piece]; ++index) {
                dir = pieceDirection[piece][index];
                tempSq = sq + dir;

                while(!SQOFFBOARD(tempSq)) {

                    // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
                    if(pos->pieces[tempSq] != EMPTY) {
                        if( PieceCol[pos->pieces[tempSq]] == side ^ 1) {
                            addCaptureMove(pos, MOVE(sq, tempSq, pos->pieces[tempSq], EMPTY, 0), list);
                        }
                        break;
                    }
                    addQuietMove(pos, MOVE(sq, tempSq, EMPTY, EMPTY, 0), list);
                    tempSq += dir;
                }
            }
        }


        piece = loopSlidePiece[pieceIndex++];
    }

    /* Loop for non slide pieces */
    pieceIndex = loopNonSlideIndex[side];
    piece = loopNonSlidePiece[pieceIndex++];
    while( piece != 0) {
        ASSERT(pieceValid(piece));

        for(pieceNum = 0; pieceNum < pos->pieceNum[piece]; ++pieceNum) {
            sq = pos->pieceList[piece][pieceNum];
            ASSERT(sqOnBoard(sq));
            //printf("Piece:%c on %s\n", pieceChar[piece], printSq(sq));

            for(index = 0; index < numDirection[piece]; ++index) {
                dir = pieceDirection[piece][index];
                tempSq = sq + dir;

                if(SQOFFBOARD(tempSq)) {
                    continue;
                }

                // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
                if(pos->pieces[tempSq] != EMPTY) {
                    if( PieceCol[pos->pieces[tempSq]] == side ^ 1) {
                        addCaptureMove(pos, MOVE(sq, tempSq, pos->pieces[tempSq], EMPTY, 0), list);
                    }
                    continue;
                }
                addQuietMove(pos, MOVE(sq, tempSq, EMPTY, EMPTY, 0), list);
            }
        }

        piece = loopNonSlidePiece[pieceIndex++];
    }

}

void generateAllCaps(const S_BOARD *pos, S_MOVELIST *list) {

	ASSERT(checkBoard(pos));

	list->count = 0;

	int pce = EMPTY;
	int side = pos->side;
	int sq = 0; int t_sq = 0;
	int pceNum = 0;
	int dir = 0;
	int index = 0;
	int pceIndex = 0;

	if(side == WHITE) {

		for(pceNum = 0; pceNum < pos->pieceNum[wP]; ++pceNum) {
			sq = pos->pieceList[wP][pceNum];
			ASSERT(sqOnBoard(sq));

			if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
				addWhitePawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], list);
			}
			if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
				addWhitePawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], list);
			}

			if(pos->enPas != NO_SQ) {
				if(sq + 9 == pos->enPas) {
					addEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEP), list);
				}
				if(sq + 11 == pos->enPas) {
					addEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEP), list);
				}
			}
		}

	} else {

		for(pceNum = 0; pceNum < pos->pieceNum[bP]; ++pceNum) {
			sq = pos->pieceList[bP][pceNum];
			ASSERT(sqOnBoard(sq));

			if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
				addBlackPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
			}

			if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
				addBlackPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
			}
			if(pos->enPas != NO_SQ) {
				if(sq - 9 == pos->enPas) {
					addEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEP), list);
				}
				if(sq - 11 == pos->enPas) {
					addEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEP), list);
				}
			}
		}
	}

	/* Loop for slide pieces */
	pceIndex = loopSlideIndex[side];
	pce = loopSlidePiece[pceIndex++];
	while( pce != 0) {
		ASSERT(pieceValid(pce));

		for(pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
			sq = pos->pieceList[pce][pceNum];
			ASSERT(sqOnBoard(sq));

			for(index = 0; index < numDirection[pce]; ++index) {
				dir = pieceDirection[pce][index];
				t_sq = sq + dir;

				while(!SQOFFBOARD(t_sq)) {
					// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
					if(pos->pieces[t_sq] != EMPTY) {
						if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
							addCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
						}
						break;
					}
					t_sq += dir;
				}
			}
		}

		pce = loopSlidePiece[pceIndex++];
	}

	/* Loop for non slide */
	pceIndex = loopNonSlideIndex[side];
	pce = loopNonSlidePiece[pceIndex++];

	while( pce != 0) {
		ASSERT(pieceValid(pce));

		for(pceNum = 0; pceNum < pos->pieceNum[pce]; ++pceNum) {
			sq = pos->pieceList[pce][pceNum];
			ASSERT(sqOnBoard(sq));

			for(index = 0; index < numDirection[pce]; ++index) {
				dir = pieceDirection[pce][index];
				t_sq = sq + dir;

				if(SQOFFBOARD(t_sq)) {
					continue;
				}

				// BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
				if(pos->pieces[t_sq] != EMPTY) {
					if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
						addCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					continue;
				}
			}
		}

		pce = loopNonSlidePiece[pceIndex++];
	}

}