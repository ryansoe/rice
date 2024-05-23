// search.cpp

#include "stdio.h"
#include "defs.h"

static void checkUp(S_SEARCHINFO *info) {
    // check if time up or interrupt from GUI
    if(info->timeset == TRUE && getTimeMs() > info->stoptime) {
        info->stopped = TRUE;
    }


}

static void pickNextMove(int moveNum, S_MOVELIST *list) {
    S_MOVE temp;
    int index = 0;
    int bestScore = 0;
    int bestNum = moveNum;

    for (index = moveNum; index < list->count; ++index) {
        if (list->moves[index].score > bestScore) {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }
    temp = list->moves[moveNum];
    list->moves[moveNum] = list->moves[bestNum];
    list->moves[bestNum] = temp;
}

static int isRepetition(const S_BOARD *pos) {

	int index = 0;

	for(index = pos->hisPlay - pos->fiftyMove; index < pos->hisPlay-1; ++index) {
		ASSERT(index >= 0 && index < MAXGAMEMOVES);
		if(pos->posKey == pos->history[index].posKey) {
			return TRUE;
		}
	}
	return FALSE;
}

static void clearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
    int index = 0;
    int index2 = 0;
    
    for(index = 0; index < 13; ++index) {
        for(index2 = 0; index2 < BRD_SQ_NUM; ++index2) {
            pos->searchHistory[index][index2] = 0;
        }
    }
    
	for(index = 0; index < 2; ++index) {
		for(index2 = 0; index2 < MAXDEPTH; ++index2) {
			pos->searchKillers[index][index2] = 0;
		}
	}
    
    clearPvTable(pos->pvTable);
    pos->play = 0;

    info->stopped = 0;
    info->nodes = 0;
    info->failHigh = 0;
    info->failHighFirst = 0;
}

static int quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
    ASSERT(checkBoard(pos));

    if( (info->nodes & 2047) == 0) {
        checkUp(info);
    }

    info->nodes++;

    if((isRepetition(pos) || pos->fiftyMove >= 100) && pos->play) {
        return 0;
    }

    if(pos->play > MAXDEPTH - 1) {
        return evalPosition(pos);
    }

    int score = evalPosition(pos);

    if(score >= beta) {
        return beta;
    }

    if(score > alpha) {
        alpha = score;
    }

    S_MOVELIST list[1];
    generateAllCaps(pos,list);

    int moveNum = 0;
    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = NOMOVE;
    int pvMove = probePvTable(pos);

    for(moveNum = 0; moveNum < list->count; ++moveNum) { 

        pickNextMove(moveNum, list);

        if ( !makeMove(pos,list->moves[moveNum].move)) {
            continue;
        }
        
        legal++;
        score = -quiescence( -beta, -alpha, pos, info);
        takeMove(pos);

        if(info->stopped == TRUE) {
            return 0;
        }

        if(score > alpha) {
            if(score >= beta) {
                if(legal==1) { 
                    info->failHighFirst++;
                }
                info->failHigh++;
                return beta;
            }
            alpha = score;
            bestMove = list->moves[moveNum].move;
        }
    }


    if(alpha != oldAlpha) {
        storePvMove(pos, bestMove);
    }

    return alpha;
}

static int alphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int doNull) {
    ASSERT(checkBoard(pos));
    
    if(depth == 0) {
        return quiescence(alpha, beta, pos, info);
    }

    if( (info->nodes & 2047) == 0) {
        checkUp(info);
    }
    
    info->nodes++;

    if(isRepetition(pos) || pos->fiftyMove >= 100) {
        return 0;
    }

    if(pos->play > MAXDEPTH - 1) {
        return evalPosition(pos);
    }
    
    int inCheck = sqAttacked(pos->kingSq[pos->side], pos->side^1, pos);

    if(inCheck == TRUE) {
        depth++;
    }

    S_MOVELIST list[1];
    generateAllMoves(pos,list);

    int moveNum = 0;
    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = NOMOVE;
    int score = -INFINITE;
    int pvMove = probePvTable(pos);

    if( pvMove != NOMOVE) {
        for(moveNum = 0; moveNum < list->count; ++moveNum) {
            if( list->moves[moveNum].move == pvMove) {
                list->moves[moveNum].score = 2000000;
                break;
            }
        }
    }
    
    for(moveNum = 0; moveNum < list->count; ++moveNum) { 

        pickNextMove(moveNum, list);

        if ( !makeMove(pos,list->moves[moveNum].move)) {
            continue;
        }
        
        legal++;
        score = -alphaBeta( -beta, -alpha, depth-1, pos, info, TRUE);
        takeMove(pos);

        if(info->stopped == TRUE) {
            return 0;
        }

        if(score > alpha) {
            if(score >= beta) {
                if(legal==1) { 
                    info->failHighFirst++;
                }
                info->failHigh++;

                if(!(list->moves[moveNum].move & MFLAGCAP)) {
                    pos->searchKillers[1][pos->play] = pos->searchKillers[0][pos->play];
                    pos->searchKillers[0][pos->play] = list->moves[moveNum].move;
                }

                return beta;
            }
            alpha = score;
            bestMove = list->moves[moveNum].move;
            if(!(list->moves[moveNum].move & MFLAGCAP)) {
                pos->searchHistory[pos->pieces[FROMSQ(bestMove)]][TOSQ(bestMove)] += depth;
            }
        }
    }

    if(legal == 0) {
        if(inCheck) {
            return -MATE + pos->play;
        } else {
            return 0;
        }
    }

    if(alpha != oldAlpha) {
        storePvMove(pos , bestMove);
    }

    return alpha;
}

void searchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    int currentDepth = 0;
    int pvMoves = 0;
    int pvNum = 0;
    
    clearForSearch(pos,info);
    
    // iterative deepening
    for( currentDepth = 1; currentDepth <= info->depth; ++currentDepth) {
                                //alpha  beta
        bestScore = alphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);

        if(info->stopped == TRUE) {
            break;
        }

        pvMoves = getPvLine(currentDepth, pos);
        bestMove = pos->pvArray[0];

        if(info->GAME_MODE == UCIMODE) {
            printf("info score cp %d depth %d nodes %ld time %d ",
                bestScore,currentDepth,info->nodes,getTimeMs()-info->starttime);
        } else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
            printf("%d %d %d %ld ",
                currentDepth,bestScore,(getTimeMs()-info->starttime)/10,info->nodes);
        } else if(info->POST_THINKING == TRUE) {
            printf("score:%d depth:%d nodes:%ld time:%d(ms) ",
                bestScore,currentDepth,info->nodes,getTimeMs()-info->starttime);
        }

        if(info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {
				pvMoves = getPvLine(currentDepth, pos);
				if(!info->GAME_MODE == XBOARDMODE) {
					printf("pv");
				}
				for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
					printf(" %s",printMove(pos->pvArray[pvNum]));
				}
				printf("\n");
			}

    }

    if(info->GAME_MODE == UCIMODE) {
		printf("bestmove %s\n",printMove(bestMove));
	} else if(info->GAME_MODE == XBOARDMODE) {
		printf("move %s\n",printMove(bestMove));
		makeMove(pos, bestMove);
	} else {
		printf("\n\n***!! Vice makes move %s !!***\n\n",printMove(bestMove));
		makeMove(pos, bestMove);
		printBoard(pos);
	}


}


