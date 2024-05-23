// pvtable.c

#include "stdio.h"
#include "defs.h"

const int pvSize = 0x100000 * 2;

int getPvLine(const int depth, S_BOARD *pos) {
    ASSERT(depth < MAXDEPTH);

    int move = probePvTable(pos);
    int count = 0;

    while(move != NOMOVE && count < depth) {
        ASSERT(count < MAXDEPTH);

        if( moveExists(pos, move) ) {
            makeMove(pos, move);
            pos->pvArray[count++] = move;
        } else {
            break;
        }
        move = probePvTable(pos);
    }

    while(pos->play > 0) {
        takeMove(pos);
    }

    return count;
}

void clearPvTable(S_PVTABLE *table) {
    S_PVENTRY *pvEntry;

    for (pvEntry = table->pTable; pvEntry < table->pTable + table->numEntries; pvEntry++) {
        pvEntry->posKey = 0ULL;
        pvEntry->move = NOMOVE;
    }
}

void initPvTable(S_PVTABLE *table) {
    table->numEntries = pvSize / sizeof(S_PVENTRY);
    table->numEntries -= 2;
    if(table->pTable!=NULL) {
		free(table->pTable);
	}
    table->pTable = (S_PVENTRY *) malloc(table->numEntries * sizeof(S_PVENTRY));
    clearPvTable(table);
    //printf("pvTable init complete with %d entries\n", table->numEntries);
}

void storePvMove(const S_BOARD *pos, const int move) {
    int index = pos->posKey % pos->pvTable->numEntries;
    ASSERT(index >= 0 && index <= pos->pvTable->numEntries - 1);

    pos->pvTable->pTable[index].move = move;
    pos->pvTable->pTable[index].posKey = pos->posKey;
}

int probePvTable(const S_BOARD *pos) {
    int index = pos->posKey % pos->pvTable->numEntries;
    ASSERT(index >= 0 && index <= pos->pvTable->numEntries - 1);

    if(pos->pvTable->pTable[index].posKey == pos->posKey) {
        return pos->pvTable->pTable[index].move;
    }

    return NOMOVE;
}
