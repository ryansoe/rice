// init.cpp

#include "defs.h"
#include "stdlib.h"
using namespace std;

#define RAND_64 (   (U64)rand() | \
                    (U64)rand() << 15 | \
                    (U64)rand() << 30 | \
                    (U64)rand() << 45 | \
                    ((U64)rand() & 0xf) << 60   )

int Sq120ToSq64[BRD_SQ_NUM];
int Sq64ToSq120[64];

U64 setMask[64];
U64 clearMask[64];

U64 pieceKeys[13][120];
U64 sideKey;
U64 castleKeys[16];

int filesBoard[BRD_SQ_NUM];
int ranksBoard[BRD_SQ_NUM];

U64 fileBBMask[8];
U64 rankBBMask[8];

U64 blackPassedMask[64];
U64 whitePassedMask[64];
U64 isolatedMask[64];

void initEvalMasks() {
    int sq, tsq, r, f;

    for(sq = 0; sq < 8; ++sq) {
        fileBBMask[sq] = 0ULL;
        rankBBMask[sq] = 0ULL;
    }

    for(r = RANK_8; r >= RANK_1; r--) {
        for(f = FILE_A; f <= FILE_H; f++) {
            sq = r * 8 + f;
            fileBBMask[f] |= (1ULL << sq);
            rankBBMask[r] |= (1ULL << sq);
        }
    }

    for(sq = 0; sq < 64; sq++) {
        isolatedMask[sq] = 0ULL;
        whitePassedMask[sq] = 0ULL;
        blackPassedMask[sq] = 0ULL;
    }

    for(sq = 0; sq < 64; ++sq) {
        tsq = sq + 8;

        while(tsq < 64) {
            whitePassedMask[sq] |= (1ULL << tsq);
            tsq += 8;
        }

        tsq = sq - 8;
        while(tsq >= 0) {
            blackPassedMask[sq] |= (1ULL << tsq);
            tsq -= 8;
        }

        if(filesBoard[SQ120(sq)] > FILE_A) {
            isolatedMask[sq] |= fileBBMask[filesBoard[SQ120(sq)] - 1];

            tsq = sq + 7;
            while(tsq < 64) {
                whitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 9;
            while(tsq >= 0) {
                blackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }

        if(filesBoard[SQ120(sq)] < FILE_H) {
            isolatedMask[sq] |= fileBBMask[filesBoard[SQ120(sq)] + 1];

            tsq = sq + 9;
            while(tsq < 64) {
                whitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 7;
            while(tsq >= 0) {
                blackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }
    }

}

void initFilesRanksBoard() {
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;

    for(index = 0; index < BRD_SQ_NUM; ++index) {
        filesBoard[index] = OFFBOARD;
        ranksBoard[index] = OFFBOARD;
    }

    for(rank = RANK_1; rank <= RANK_8; ++rank) {
        for(file = FILE_A; file <= FILE_H; ++file) {
            sq = FR2SQ(file,rank);
            filesBoard[sq] = file;
            ranksBoard[sq] = rank;
        }
    }
}

void initHashKeys() {

	int index = 0;
	int index2 = 0;
	for(index = 0; index < 13; ++index) {
		for(index2 = 0; index2 < 120; ++index2) {
			pieceKeys[index][index2] = RAND_64;
		}
	}
	sideKey = RAND_64;
	for(index = 0; index < 16; ++index) {
		castleKeys[index] = RAND_64;
	}

}

void initBitMask () {
    int index = 0;

    for(index = 0; index < 64; index++) {
        setMask[index] = 0ULL;
        clearMask[index] = 0ULL;
    }

    for(index = 0; index < 64; index++) {
        setMask[index] |= (1ULL << index);
        clearMask[index] = ~setMask[index];
    }
}

void initSq120To64() {

    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;
    for(index = 0; index < BRD_SQ_NUM; ++index) {
        Sq120ToSq64[index] = 65;
    }

    for(index = 0; index < 64; ++index) {
        Sq64ToSq120[index] = 120;
    }

    for(rank = RANK_1; rank <= RANK_8; ++rank) {
        for(file = FILE_A; file <= FILE_H; ++file) {
            sq = FR2SQ(file, rank);
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++;
        }
    }
}

void allInit() {
    initSq120To64();
    initBitMask();
    initHashKeys();
    initFilesRanksBoard();
    initEvalMasks();
    initMvvLva();
};