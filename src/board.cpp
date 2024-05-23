// board.cpp

#include "defs.h"
#include "stdlib.h"


int checkBoard(const S_BOARD *pos) {
    int tempPieceNum[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int tempBigPiece[2] = { 0, 0 };
    int tempMajPiece[2] = { 0, 0 };
    int tempMinPiece[2] = { 0, 0 };
    int tempMaterial[2] = { 0, 0 };

    int sq64, tempPiece, tempPieceNumber, sq120, color, pawnCount;

    U64 tempPawns[3] = {0ULL, 0ULL, 0ULL};

    tempPawns[WHITE] = pos->pawns[WHITE];
    tempPawns[BLACK] = pos->pawns[BLACK];
    tempPawns[BOTH] = pos->pawns[BOTH];

    for(tempPiece = wP; tempPiece <= bK; ++tempPiece) {
        for(tempPieceNumber = 0; tempPieceNumber < pos->pieceNum[tempPiece]; ++tempPieceNumber) {
            sq120 = pos->pieceList[tempPiece][tempPieceNumber];
            if (pos->pieces[sq120]!=tempPiece) {
                printBoard(pos);
                
            }
            ASSERT(pos->pieces[sq120]==tempPiece)
        }
    }

    for(sq64 = 0; sq64 < 64; ++sq64) {
        sq120 = SQ120(sq64);
        tempPiece = pos->pieces[sq120];
        tempPieceNum[tempPiece]++;
        color = PieceCol[tempPiece];
        if( PieceBig[tempPiece] == TRUE) tempBigPiece[color]++;
        if( PieceMin[tempPiece] == TRUE) tempMinPiece[color]++;
        if( PieceMaj[tempPiece] == TRUE) tempMajPiece[color]++;

        tempMaterial[color] += PieceVal[tempPiece];
    }

    for(tempPiece = wP; tempPiece <= bK; ++tempPiece) {
        ASSERT(tempPieceNum[tempPiece]==pos->pieceNum[tempPiece]);
    }

    // check bitboards count
    pawnCount = CNT(tempPawns[WHITE]);
    ASSERT(pawnCount == pos->pieceNum[wP]);
    pawnCount = CNT(tempPawns[BLACK]);
    ASSERT(pawnCount == pos->pieceNum[bP]);
    pawnCount = CNT(tempPawns[BOTH]);
    ASSERT(pawnCount == pos->pieceNum[wP] + pos->pieceNum[bP]);

    // check bitboards squares
    while(tempPawns[WHITE]) {
        sq64 = POP(&tempPawns[WHITE]);
        ASSERT(pos->pieces[SQ120(sq64)] == wP);
    }

    while(tempPawns[BLACK]) {
        sq64 = POP(&tempPawns[BLACK]);
        ASSERT(pos->pieces[SQ120(sq64)] == bP);
    }

    while(tempPawns[BOTH]) {
        sq64 = POP(&tempPawns[BOTH]);
        ASSERT( (pos->pieces[SQ120(sq64)] == bP) || (pos->pieces[SQ120(sq64)] == wP));
    }
    
    ASSERT(tempMaterial[WHITE]==pos->material[WHITE] && tempMaterial[BLACK]==pos->material[BLACK]);
    ASSERT(tempMinPiece[WHITE]==pos->minorPiece[WHITE] && tempMinPiece[BLACK]==pos->minorPiece[BLACK]);
    ASSERT(tempMajPiece[WHITE]==pos->majorPiece[WHITE] && tempMajPiece[BLACK]==pos->majorPiece[BLACK]);
    ASSERT(tempBigPiece[WHITE]==pos->bigPiece[WHITE] && tempBigPiece[BLACK]==pos->bigPiece[BLACK]);

    ASSERT(pos->side==WHITE || pos->side==BLACK);
    ASSERT(generatePosKey(pos)==pos->posKey);

    ASSERT(pos->enPas==NO_SQ || ( ranksBoard[pos->enPas]==RANK_6 && pos->side == WHITE ) || ( ranksBoard[pos->enPas]==RANK_3 && pos->side == BLACK ));

    ASSERT(pos->pieces[pos->kingSq[WHITE]] == wK);
    ASSERT(pos->pieces[pos->kingSq[BLACK]] == bK);

    return TRUE;
}

void updateListsMaterial(S_BOARD *pos) {
    int piece,sq,index, color;

    for(index = 0; index < BRD_SQ_NUM; ++index) {
        sq = index;
        piece = pos->pieces[index];
        if(piece!=OFFBOARD && piece!=EMPTY) {
            color = PieceCol[piece];

            if( PieceBig[piece] == TRUE) pos->bigPiece[color]++;
            if( PieceMin[piece] == TRUE) pos->minorPiece[color]++;
            if( PieceMaj[piece] == TRUE) pos->majorPiece[color]++;

            pos->material[color] += PieceVal[piece];

            // piece list
            pos->pieceList[piece][pos->pieceNum[piece]] = sq;
            pos->pieceNum[piece]++;

            if(piece==wK) pos->kingSq[WHITE] = sq;
            if(piece==bK) pos->kingSq[BLACK] = sq;

			if(piece==wP) {
				SETBIT(pos->pawns[WHITE],SQ64(sq));
				SETBIT(pos->pawns[BOTH],SQ64(sq));
			} else if(piece==bP) {
				SETBIT(pos->pawns[BLACK],SQ64(sq));
				SETBIT(pos->pawns[BOTH],SQ64(sq));
            }
        }
    }
}

int parseFen(char *fen, S_BOARD *pos) {
    ASSERT(fen!=NULL);
    ASSERT(pos!=NULL);

    int rank = RANK_8;
    int file = FILE_A;
    int piece = 0;
    int count = 0;
    int i = 0;
    int sq64 = 0;
    int sq120 = 0;

    resetBoard(pos);

    while ((rank >= RANK_1) && *fen) {
        count = 1;

		switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;

            default:
                printf("FEN error \n");
                return -1;
        }

        for (i = 0; i < count; i++) {
            sq64 = rank * 8 + file;
            sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
            file++;
        }
        fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b');

    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2;

    for (i = 0; i < 4; i++) {
        if (*fen == ' ') {
            break;
        }
        switch(*fen) {
			case 'K': pos->castlePerm |= WKCA; break;
			case 'Q': pos->castlePerm |= WQCA; break;
			case 'k': pos->castlePerm |= BKCA; break;
			case 'q': pos->castlePerm |= BQCA; break;
			default:	     break;
        }
        fen++;
    }
    fen++;

    ASSERT(pos->castlePerm>=0 && pos->castlePerm <= 15);

    if (*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';

        ASSERT(file>=FILE_A && file <= FILE_H);
        ASSERT(file>=RANK_1 && file <= RANK_8);

        pos->enPas = FR2SQ(file,rank);
    }

    pos->posKey = generatePosKey(pos);

    updateListsMaterial(pos);

    return 0;
}

void resetBoard(S_BOARD *pos) {
    int index = 0;

    for(index = 0; index < BRD_SQ_NUM; ++index) {
        pos->pieces[index] = OFFBOARD;
    }

    for(index = 0; index < 64; ++index) {
        pos->pieces[SQ120(index)] = EMPTY;
    }

    for(index = 0; index < 2; ++index) {
        pos->bigPiece[index] = 0;
        pos->majorPiece[index] = 0;
        pos->minorPiece[index] = 0;
        pos->material[index] = 0;
    }

    for(index = 0; index < 3; ++index) {
        pos->pawns[index] = 0ULL;
    }

    for(index = 0; index < 13; ++index) {
        pos->pieceNum[index] = 0;
    }

    pos->kingSq[WHITE] = pos->kingSq[BLACK] = NO_SQ;

    pos->side = BOTH;
    pos->enPas = NO_SQ;
    pos->fiftyMove = 0;

    pos->play = 0;
    pos->hisPlay = 0;

    pos->castlePerm = 0;

    pos->posKey = 0ULL;
}

void printBoard(const S_BOARD *pos) {
    int sq,file,rank,piece;

    printf("\nGame Board:\n\n");

    for(rank = RANK_8; rank >= RANK_1; rank--) {
        printf("%d ", rank+1);
        for(file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file,rank);
            piece = pos->pieces[sq];
            printf("%3c", pieceChar[piece]);
        }
        printf("\n");
    }

    printf("\n  ");
    for(file = FILE_A; file <= FILE_H; file++) {
        printf("%3c", 'a'+file);
    }
    printf("\n");
    printf("side:%c\n", sideChar[pos->side]);
    printf("enPas:%d\n",pos->enPas);
    	printf("castle:%c%c%c%c\n",
			pos->castlePerm & WKCA ? 'K' : '-',
			pos->castlePerm & WQCA ? 'Q' : '-',
			pos->castlePerm & BKCA ? 'k' : '-',
			pos->castlePerm & BQCA ? 'q' : '-'
			);
	printf("PosKey:%llX\n",pos->posKey);
}

void mirrorBoard(S_BOARD *pos) {
    int tempPiecesArray[64];
    int tempSide = pos->side^1;
    int swapPiece[13] = { EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK };
    int tempCastlePerm = 0;
    int tempEnPas = NO_SQ;

    int sq;
    int tp;
    
    if (pos->castlePerm & WKCA) tempCastlePerm |= BKCA;
    if (pos->castlePerm & WQCA) tempCastlePerm |= BQCA;

    if (pos->castlePerm & BKCA) tempCastlePerm |= WKCA;
    if (pos->castlePerm & BQCA) tempCastlePerm |= WQCA;

    if (pos->enPas != NO_SQ) {
        tempEnPas = SQ120(mirror64[SQ64(pos->enPas)]);
    }

    for (sq = 0; sq < 64; sq++) {
        tempPiecesArray[sq] = pos->pieces[SQ120(mirror64[sq])];
    }

    resetBoard(pos);

    for (sq = 0; sq < 64; sq++) {
        tp = swapPiece[tempPiecesArray[sq]];
        pos->pieces[SQ120(sq)] = tp;
    }

    pos->side = tempSide;
    pos->castlePerm = tempCastlePerm;
    pos->enPas = tempEnPas;

    pos->posKey = generatePosKey(pos);

    updateListsMaterial(pos);

    ASSERT(checkBoard(pos));
}