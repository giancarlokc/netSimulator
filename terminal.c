#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simulator.h"
#include "graph.h"
#include "terminal.h"

/* Verify if the string s can be translated as an integer */
int validNumEntry(char *s){
	int i = 0;

	while(s[i] != '\n'){
		if(s[i] < '0' || s[i] > '9'){
			printf("Invalid entry\n");
			return 0;
		}
		i++;
	}
	return 1;
}

/* -------------------------------------------------------------------------------- Command Functions */
/* Shows how to use the program */
void help(){
	printf("netSimulator\n\n");
	printf("Commands:\n");
	printf("   mkr         - Creates a new router with the next valid ID\n");
	printf("   rmr         - Remove a specific router\n");
	printf("   conr        - Connect a router to another\n");
	printf("   descr       - Inverse of 'conr'\n");
	printf("   automap off - Turn off the viewing of the router map\n");
	printf("   automap on  - Turn on the viewing of the router map\n");
	printf("   save        - Save the current configuration\n");
	printf("   load        - Load the current configuration\n");
	printf("   help        - Show the help page\n");
	printf("   dmap        - Manually displays the router map\n");
	printf("   exit        - Exit the program\n");
}

/* Creates a new router with the next valid ID */
void mkr(Router *listRouter[]){
	createRouter(listRouter);
	printf("Router created\n");
}

/* Remove a specific router */
void rmr(Router *listRouter[]){
	int ID;
	char s_ID[SIZE_ENTRY];

	do{
		printf("Enter the router ID: ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	ID = atoi(s_ID);
	if(!deleteRouter(listRouter,ID)){
		printf("Router not found\n");
	}
}

/* Set automap on */
void autoMapOn(int *autoMap){
	*autoMap = 1;
}

/* Set automap off */
void autoMapOff(int *autoMap){
	*autoMap = 0;
}

/* Display the map of routers */
void dmap(Router *listRouter[]){
	printRouterList(listRouter);
}

void sendto(Router *listRouter[], char *sys_mesg){
	int from, to, pSize;
	char s_ID[SIZE_ENTRY];
	float result;
	
	printf("Send package\n");
	do{
		printf("From (ID): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	from = atoi(s_ID);
	do{
		printf("To (ID): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	to = atoi(s_ID);
	do{
		printf("Package size (Kb): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	pSize = atoi(s_ID);
	
	if((result = sendPackage(listRouter, from, to, pSize)) >= 0 ){
		printf("Package sent in %f seconds (%f hours).\n", result, result/3600);
		getchar();
		sprintf(s_ID, "Package sent.");
		strcpy(sys_mesg,s_ID);
	} else {
		sprintf(s_ID, "Router %d is not connected to router %d.", from, to);
		strcpy(sys_mesg,s_ID);
	}
}

void sendp(Router *listRouter[], char *sys_mesg){
	int from, to, pSize, i, visited[N_ROUTER], path[N_ROUTER], final_path[N_ROUTER], path_size;
	char s_ID[SIZE_ENTRY];
	float result, minTime = 10000000.0;
	
	for(i=0;i<N_ROUTER;i++)
		visited[i] = 0;
	
	printf("Send package\n");
	do{
		printf("From (ID): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	from = atoi(s_ID);
	do{
		printf("To (ID): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	to = atoi(s_ID);
	do{
		printf("Package size (Kb): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	pSize = atoi(s_ID);
	visited[from] = 1;
	send(0, listRouter, from, to, pSize, 0.0, &minTime, visited, path, final_path, &path_size);
	if(minTime == 10000000.0){
		printf("The router %d is unreachable from %d.\n", to, from);
		getchar();
	}
	printf("\n## Statistics ##\n");
	printf("   Time from %d to %d: %f seconds (%f hours).\n", from, to, minTime, minTime/3600);
	printf("   Distance: %d hop(s).\n", path_size);
	printf("   Path: %d", from);
	for(i=0;i<path_size;i++)
		printf("->%d", final_path[i]);
	getchar();
}

/* Connect two routers */
void conr(Router *listRouter[]){
	int from, to, weight;
	char s_ID[SIZE_ENTRY];

	do{
		printf("Connecting from (ID): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	from = atoi(s_ID);
	do{
		printf("To (ID): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	to = atoi(s_ID);
	do{
		printf("Weight: ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	weight = atoi(s_ID);

	if(!createConnection(from, to, listRouter, weight))
		printf("Router not found\n");
	else
		printf("Connection created between '%d' and '%d'\n", from, to);
}

/* Remove Connections between two routers */
void rcon(Router *listRouter[]){
	int from, to, weight;
	char s_ID[SIZE_ENTRY];

	do{
		printf("Removing connection from (ID): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	from = atoi(s_ID);
	do{
		printf("To (ID): ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	to = atoi(s_ID);
	do{
		printf("Weight: ");
		fgets(s_ID, SIZE_ENTRY, stdin);
	}while(!validNumEntry(s_ID));
	weight = atoi(s_ID);

	if(!removeConnection(from, to, listRouter, weight))
		printf("Connection not found with %d weight\n",weight);
	else
		printf("Connection removed between '%d' and '%d'\n", from, to);
}

/* Creates the  adjacent matrix of the graph */
void mkgraph(Router *listRouter[], int *n_nodes, int adMatrix[][N_ROUTER]){
	int i = 0, n_router = 0, j;

	while(i < N_ROUTER){
		if(listRouter[i] != NULL)
			n_router++;
		i++;
	}
	printf("Number of nodes: %d\n",n_router);

	*n_nodes = n_router;
	for(i=0;i<n_router;i++)
		for(j=0;j<n_router;j++)
			adMatrix[i][j] = 0;

	for(i=0;i<n_router;i++){
		for(j=0;j<listRouter[i]->n_connections;j++){
			adMatrix[i][listRouter[i]->connection[j].ID] = 1;
		}
	}

	/* Print matrix */
	printf("\nAdjacent Matrix\n");
	for(i=0;i<n_router;i++){
		for(j=0;j<n_router;j++){
			printf("%d ",adMatrix[i][j]);
		}
		printf("\n");
	}
}

/* Display the adjacent matrix */
void dgraph(int adMatrix[][N_ROUTER], int n_nodes){
	int i, j;

	printf("\nAdjacent Matrix\n");
	for(i=0;i<n_nodes;i++){
		for(j=0;j<n_nodes;j++){
			printf("%d ",adMatrix[i][j]);
		}
		printf("\n");
	}
}

/* Save the current configuration of the simulation */
void save(Router *listRouter[], char *sys_mesg){
	char s[SIZE_ENTRY];

	printf("Save network configuration as: ");
	fgets(s, SIZE_ENTRY, stdin);
	s[strlen(s) - 1] = '\0';
	saveConf(listRouter, s);
	strcpy(sys_mesg,"Configuration saved as '");
	strcat(sys_mesg, s);
	strcat(sys_mesg,"'.");
}

/* Load a configuration */
void load(Router *listRouter[], char *sys_mesg){
	char s[SIZE_ENTRY];

	printf("Load network configuration file: ");
	fgets(s, SIZE_ENTRY, stdin);
	s[strlen(s) - 1] = '\0';
	loadConf(listRouter, s);
	strcpy(sys_mesg,"Configuration loaded.");
}

int terminal(Router *listRouter[], int adMatrix[][N_ROUTER], int *n_nodes){
	char entry[SIZE_ENTRY];
	char sys_mesg[SYS_MESG];
	int autoMap = 1, showSysMesg = 0;

	do{
		if(autoMap){
			system("clear");
			if(showSysMesg){
				printf("[Simulator]: %s\n", sys_mesg);
				showSysMesg = 0;	
			}
			printRouterList(listRouter);
		}
		printf(">");
		fgets(entry, SIZE_ENTRY, stdin);

		// menu of options
		if(!strcmp(entry,"mkr\n")){
			mkr(listRouter);
		} else if(!strcmp(entry,"rmr\n")){
			rmr(listRouter);
		} else if(!strcmp(entry,"help\n")){
			help();
			if(autoMap)
				getchar();
		} else if(!strcmp(entry,"automap on\n")){
			autoMap = 1;
		} else if(!strcmp(entry,"automap off\n")){
			autoMap = 0;
		} else if(!strcmp(entry,"dmap\n")){
			dmap(listRouter);
		} else if(!strcmp(entry,"conr\n")){
			conr(listRouter);
		} else if(!strcmp(entry,"save\n")){
			save(listRouter, sys_mesg);
			showSysMesg = 1;
		} else if(!strcmp(entry,"load\n")){
			load(listRouter, sys_mesg);
			showSysMesg = 1;
		} else if(!strcmp(entry,"rcon\n")){
			rcon(listRouter);
			if(autoMap)
				getchar();
		} else if(!strcmp(entry,"mkgraph\n")){
			mkgraph(listRouter, n_nodes, adMatrix);
			if(autoMap)
				getchar();
		} else if(!strcmp(entry,"dgraph\n")){
			dgraph(adMatrix, *n_nodes);
			if(autoMap)
				getchar();
		} else if(!strcmp(entry,"sendto\n")){
			sendto(listRouter, sys_mesg);
			showSysMesg = 1;
		} else if(!strcmp(entry,"sendp\n")){
			sendp(listRouter, sys_mesg);
			showSysMesg = 1;
		} else if(strcmp(entry,"exit\n")){
			printf("Unknow command. Use 'help'\n");
			if(autoMap)
				getchar();
		}
	}while(strcmp(entry,"exit\n"));
}
