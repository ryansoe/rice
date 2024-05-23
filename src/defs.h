#ifndef DEFS_H
#define DEFS_H

#include "stdlib.h"
#include "stdio.h"
#include <cstring>

#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)) { \
printf("%s - Failed", #n); \
printf("On %s ",__DATE__); \
printf("At %s ",__TIME__); \
printf("In File %s", __FILE__); \
printf("At Line %d\n", __LINE__); \
exit(1);}
#endif

typedef unsigned long long U64;

#define NAME "Rice 0.1"
#define BRD_SQ_NUM 120

#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INFINITE 30000
#define MATE 29000

enum epiece { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum efile { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum erank { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum ecolor { WHITE, BLACK, BOTH };
enum emode { UCIMODE, XBOARDMODE, CONSOLEMODE };

//  Four bits to represent castling permissions (e.g. 1 0 0 1 means WKCA and BQCA)
enum ecastle { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

enum esquare {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1, 
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

enum ebool { FALSE, TRUE };

typedef struct {
    int move;
    int score;
} S_MOVE;

typedef struct {
    S_MOVE moves[MAXPOSITIONMOVES];
    int count;
} S_MOVELIST;

typedef struct {
    U64 posKey;
    int move;
} S_PVENTRY;

typedef struct {
    S_PVENTRY *pTable = NULL;
    int numEntries;
} S_PVTABLE;
typedef struct {

    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey;

} S_UNDO;

typedef struct {
    int pieces[BRD_SQ_NUM];
    U64 pawns[3];

    int kingSq[2];
    
    int side;
    int enPas;
    int fiftyMove;

    int play;
    int hisPlay;

    int castlePerm;

    U64 posKey;

    int pieceNum[13];
    int bigPiece[2];
    int majorPiece[2];
    int minorPiece[2];
    int material[2];

    S_UNDO history[MAXGAMEMOVES];

    // piece list
    int pieceList[13][10];

    S_PVTABLE pvTable[1];
    int pvArray[MAXDEPTH];

    int searchHistory[13][BRD_SQ_NUM];
    int searchKillers[2][MAXDEPTH];
} S_BOARD;

typedef struct {
    int starttime;
    int stoptime;
    int depth;
    int depthset;
    int timeset;
    int movestogo;
    int infinite;

    long nodes;

    int quit;
    int stopped;

    float failHigh;
    float failHighFirst;

    int GAME_MODE;
	int POST_THINKING;
} S_SEARCHINFO;

/* GAME MOVE */

/*
0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0011 1111 1000 0000 -> To >> 7, 0x7F
0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14, 0xF
0000 0000 0100 0000 0000 0000 0000 -> EnPas 0x40000
0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20, 0xF
0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
*/

#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m)>>7) & 0x7F)
#define CAPTURED(m) (((m)>>14) & 0xF)
#define PROMOTED(m) (((m)>>20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000

#define NOMOVE 0

/* MACROS */

#define FR2SQ(f,r) ( (21 + (f) ) + ( (r) * 10 ) )
#define SQ64(sq120) (Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
#define POP(b) popBit(b)
#define CNT(b) countBits(b)
#define CLRBIT(bb,sq) ((bb) &= clearMask[(sq)])
#define SETBIT(bb,sq) ((bb) |= setMask[(sq)])

#define IsBQ(p) (pieceBishopQueen[(p)])
#define IsRQ(p) (pieceRookQueen[(p)])
#define IsKn(p) (pieceKnight[(p)])
#define IsKi(p) (pieceKing[(p)])

#define MIRROR64(sq) (mirror64[(sq)])

/* GLOBALS */

extern int Sq120ToSq64[BRD_SQ_NUM];
extern int Sq64ToSq120[64];
extern U64 setMask[64];
extern U64 clearMask[64];
extern U64 pieceKeys[13][120];
extern U64 sideKey;
extern U64 castleKeys[16];
extern char pieceChar[];
extern char sideChar[];
extern char rankChar[];
extern char fileChar[];

extern int PieceBig[13];
extern int PieceMaj[13];
extern int PieceMin[13];
extern int PieceVal[13];
extern int PieceCol[13];

extern int filesBoard[BRD_SQ_NUM];
extern int ranksBoard[BRD_SQ_NUM];

extern int pieceKnight[13];
extern int pieceKing[13];
extern int pieceRookQueen[13];
extern int pieceBishopQueen[13];
extern int pieceSlides[13];
extern int piecePawn[13];

extern int mirror64[64];

extern U64 fileBBMask[8];
extern U64 rankBBMask[8];

extern U64 blackPassedMask[64];
extern U64 whitePassedMask[64];
extern U64 isolatedMask[64];

/* FUNCTIONS */

// init.c
extern void allInit();

// bitboards.c
extern void printBitBoard(U64 bb);
extern int popBit(U64 *bb);
extern int countBits(U64 b);

// hashkeys.c
extern U64 generatePosKey(const S_BOARD *pos);

// board.cpp
extern void resetBoard(S_BOARD *pos);
extern int parseFen(char *fen, S_BOARD *pos);
extern void printBoard(const S_BOARD *pos);
extern void updateListsMaterial(S_BOARD *pos);
extern int checkBoard(const S_BOARD *pos);
extern void mirrorBoard(S_BOARD *pos);

// attack.cpp
extern int sqAttacked(const int sq, const int side, const S_BOARD *pos);

// io.cpp
extern char *printSq(const int sq);
extern char *printMove(const int move);
extern void printMoveList(const S_MOVELIST *list);
extern int parseMove(char *ptrChar, S_BOARD *pos);

// validate.cpp
extern int sqOnBoard(const int sq);
extern int sideValid(const int side);
extern int fileRankValid(const int fr);
extern int pieceValidEmpty(const int piece);
extern int pieceValid(const int piece);

// movegen.cpp
extern void generateAllMoves( const S_BOARD *pos, S_MOVELIST *list );
extern void generateAllCaps( const S_BOARD *pos, S_MOVELIST *list );
extern int moveExists(S_BOARD *pos, const int move);
extern int initMvvLva();

// makemove.cpp
extern int makeMove(S_BOARD *pos, int move);
extern void takeMove(S_BOARD *pos);

// perft.cpp
extern void perftTest(int depth, S_BOARD *pos);

// search.cpp
void searchPosition(S_BOARD *pos, S_SEARCHINFO *info);

// misc.cpp
extern int getTimeMs();
extern void readInput(S_SEARCHINFO *info);

// pvtable.cpp
extern void initPvTable(S_PVTABLE *table);
extern void storePvMove(const S_BOARD *pos, const int move);
extern int probePvTable(const S_BOARD *pos);
extern int getPvLine(const int depth, S_BOARD *pos);
extern void clearPvTable(S_PVTABLE *table);

// evaluate.cpp
extern int evalPosition(const S_BOARD *pos);

// uci.cpp
extern void uci_loop(S_BOARD *pos, S_SEARCHINFO *info);

// xboard.cpp
extern void xboard_loop(S_BOARD *pos, S_SEARCHINFO *info);
extern void console_loop(S_BOARD *pos, S_SEARCHINFO *info);

#endif