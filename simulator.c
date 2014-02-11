#include <stdio.h>

#include "simulator.h"
#include "graph.h"
#include "terminal.h"

Router* listRouter[N_ROUTER];
int adMatrix[N_ROUTER][N_ROUTER];
int n_nodes = 0;

int main(int argc, char **argv){

	iniRouter(listRouter);

	terminal(listRouter, adMatrix, &n_nodes);

	return;
}
