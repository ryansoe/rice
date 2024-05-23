#include "defs.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define PERFT "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"

int main() {
    
    allInit();

    S_BOARD pos[1];
    S_SEARCHINFO info[1];
    initPvTable(pos->pvTable);
    
    printf("Welcome to Rice! Type 'rice' for console mode...\n");

    char line[256];
    while (TRUE) {
		memset(&line[0], 0, sizeof(line));

		fflush(stdout);
		if (!fgets(line, 256, stdin))
			continue;
		if (line[0] == '\n')
			continue;
		if (!strncmp(line, "uci",3)) {
			uci_loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "xboard",6))	{
			xboard_loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "rice",4))	{
			console_loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if(!strncmp(line, "quit",4))	{
			break;
		}
	}


    free(pos->pvTable->pTable);

    return 0;
}

