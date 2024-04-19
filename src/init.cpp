#include<iostream>
#include "defs.h"
#include <stdio.h>
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

void initFilesRanksBoard() {
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;

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

};