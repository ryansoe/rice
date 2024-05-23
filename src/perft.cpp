// perft.cpp

#include "defs.h"
#include "stdio.h"

long leafNodes;

void perft(int depth, S_BOARD *pos) {

    ASSERT(checkBoard(pos));  

	if(depth == 0) {
        leafNodes++;
        return;
    }	

    S_MOVELIST list[1];
    generateAllMoves(pos,list);
      
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
       
        if ( !makeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }

        perft(depth - 1, pos);
        takeMove(pos);
    }

    return;
}

void perftTest(int depth, S_BOARD *pos) {

    ASSERT(checkBoard(pos));

	printBoard(pos);
	printf("\nStarting Test To Depth:%d\n",depth);	
	leafNodes = 0;
	int start = getTimeMs();
    S_MOVELIST list[1];
    generateAllMoves(pos,list);	
    
    int move;	    
    int MoveNum = 0;
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        move = list->moves[MoveNum].move;
        if ( !makeMove(pos,move))  {
            continue;
        }
        long cumnodes = leafNodes;
        perft(depth - 1, pos);
        takeMove(pos);        
        long oldnodes = leafNodes - cumnodes;
        printf("move %d : %s : %ld\n",MoveNum+1,printMove(move),oldnodes);
        
    }
	
	printf("\nTest Complete : %ld nodes visited in %dms\n",leafNodes,getTimeMs() - start);

    return;
}
