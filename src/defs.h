#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>

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

#define BRD_SQ_NUM 120

#define MAXGAMEMOVES 2048

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

enum epiece { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum efile { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum erank { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum ecolor { WHITE, BLACK, BOTH };

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
} S_BOARD;

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

// attack.cpp
extern int sqAttacked(const int sq, const int side, const S_BOARD *pos);

/* FUNCTIONS */

// // init.c
// void allInit();

// // bitboards.c
// void printBitBoard(U64 bb);
// int popBit(U64 *bb);
// int countBits(U64 b);

// // hashkeys.c
// U64 generatePosKey(const S_BOARD *pos);

// // board.cpp
// void resetBoard(S_BOARD *pos);
// int parseFen(char *fen, S_BOARD *pos);
// void printBoard(const S_BOARD *pos);
// void updateListsMaterial(S_BOARD *pos);
// int checkBoard(const S_BOARD *pos);

// // attack.cpp
// int sqAttacked(const int sq, const int side, const S_BOARD *pos);

#endif