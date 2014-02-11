#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simulator.h"
#include "graph.h"
#include "terminal.h"

/* -------------------------------------------------------------------------------- Funcionalities */
/* Send package from router A to adjacent router B */
float sendPackage(Router *listRouter[], int A_ID, int B_ID, int pSize){
	int currentRouter = A_ID, i = 0, j = 0;
	
	while(i < N_ROUTER){
		if(listRouter[i] != NULL && listRouter[i]->ID == currentRouter){
			for(j=0;j<listRouter[i]->n_connections;j++){
				if(listRouter[i]->connection[j].ID == B_ID){
					float size = pSize, weight = listRouter[i]->connection[j].weight;
					return size/weight;
				}
			}
			if(j == listRouter[i]->n_connections)
				return -1;
		}
		i++;
	}
	if(i == N_ROUTER)
		return -1;
}

/* Send package from router A to any router B */
int send(int level, Router *listRouter[], int A_ID, int B_ID, int pSize, float time, float *minTime, int visited[], int path[], int final_path[], int *path_size){
	int i = 0, j;

/*	printf("LEVEL %d\n", level);
	printf("CURRENT_TIME: %f\n", time);
	printf("MIN_TIME: %f\n", *minTime);
	printf("FROM: %d\n", A_ID);
	printf("TO: %d\n", B_ID);*/
	if(A_ID == B_ID){
//		getchar();
		if(time < *minTime){
//			getchar();
//			printf("FOUND MIN_TIME: %f\n",time);
			*minTime = time;
			int k;
			for(k=0;k<N_ROUTER;k++)
				final_path[k] = path[k];
			*path_size = level;
		}
		return 1;
	}
	while(i < N_ROUTER){
		if(listRouter[i] != NULL && listRouter[i]->ID == A_ID){
//			printf("N of connections: %d\n",listRouter[i]->n_connections);
			for(j=0;j<listRouter[i]->n_connections;j++){
				if(visited[listRouter[i]->connection[j].ID] == 0){
//					printf("TRYING ROUTER %d\n", listRouter[i]->connection[j].ID);
					visited[listRouter[i]->connection[j].ID] = 1;
					float size = pSize, weight = listRouter[i]->connection[j].weight;
					path[level] = listRouter[i]->connection[j].ID;
					if(send(level+1, listRouter, listRouter[i]->connection[j].ID, B_ID, pSize, time + size/weight, minTime, visited, path, final_path, path_size) < 0)
						return -1;
					path[level] = -1;
					visited[listRouter[i]->connection[j].ID] = 0;
				}
			}
			break;
		}
		i++;
	}
	if(i == N_ROUTER)
		return -1;
	return 1;
}

/* -------------------------------------------------------------------------------- Router Functions */
/* Inicializates the list of Rousters */
void iniRouterList(Router *listRouter[]){
	int i;

	for(i=0;i<N_ROUTER;i++){
		listRouter[i] = NULL;
	}
}

/* It calls the functons so that the routers can work properly */
void iniRouter(Router *listRouter[]){
	iniRouterList(listRouter);
}

/* Create a router with the next valid ID in the listRouter */
Router* createRouter(Router *listRouter[]){
	int i;

	Router *r = malloc(sizeof(Router));
	for(i=0;i<N_ROUTER;i++){
		if(listRouter[i] == NULL){
			listRouter[i] = r;
			r->ID = i;
			r->n_connections = 0;
			return r;
		}
	}
	return NULL;
}

/* Delete a router */
int deleteRouter(Router *listRouter[], int ID){
	int i = 0, j;

	while(i < N_ROUTER){
		if(listRouter[i] != NULL && listRouter[i]->ID == ID){
			for(j=0;j<listRouter[i]->n_connections;j++){
				removeConnection(listRouter[i]->ID, listRouter[i]->connection[j].ID, listRouter, listRouter[i]->connection[j].weight);
			}
			free(listRouter[i]);
			listRouter[i] = NULL;
			return 1;
		}
		i++;
	}
	if(i == N_ROUTER)
		return 0;
}

/* -------------------------------------------------------------------------------- Connections Functions */
/* Create a connections between routers */
int createConnection(int from, int to, Router *listRouter[], int weight){
	int i = 0;

	while(i < N_ROUTER){
		if(listRouter[i] != NULL && listRouter[i]->ID == from){
			from = i;
			break;
		}
		i++;
	}
	if(i == N_ROUTER){
		return 0;
	}
	i = 0;
	while(i < N_ROUTER){
		if(listRouter[i] != NULL && listRouter[i]->ID == to){
			to = i;
			break;
		}
		i++;
	}
	if(i == N_ROUTER){
		return 0;
	}
	printf("Index: from:%d to:%d\n",from,to);
	listRouter[from]->connection[listRouter[from]->n_connections].ID = to;
	listRouter[from]->connection[listRouter[from]->n_connections].weight = weight;
	listRouter[from]->n_connections = listRouter[from]->n_connections + 1;
	listRouter[to]->connection[listRouter[to]->n_connections].ID = from;
	listRouter[to]->connection[listRouter[to]->n_connections].weight = weight;
	listRouter[to]->n_connections = listRouter[to]->n_connections + 1;
	return 1;
}

int removeConnection(int from, int to, Router *listRouter[], int weight){
	int i = 0, j, from_conn, to_conn;

	while(i < N_ROUTER){
		if(listRouter[i] != NULL && listRouter[i]->ID == from){
			for(j=0;j<N_CONNECTIONS;j++){
				if(listRouter[i]->connection[j].ID == to && listRouter[i]->connection[j].weight == weight){
					from_conn = j;
					from = i;
					break;
				}
			}
			if(j < N_CONNECTIONS)
				break;
		}
		i++;
	}
	if(i == N_ROUTER){
		return 0;
	}
	i = 0;
	while(i < N_ROUTER){
		if(listRouter[i] != NULL && listRouter[i]->ID == to){
			for(j=0;j<N_CONNECTIONS;j++){
				if(listRouter[i]->connection[j].ID == from && listRouter[i]->connection[j].weight == weight){
					to_conn = j;
					to = i;
					break;
				}
			}
			if(j < N_CONNECTIONS)
				break;
		}
		i++;
	}
	if(i == N_ROUTER){
		return 0;
	}

	for(i = from_conn;i<N_CONNECTIONS;i++){
		listRouter[from]->connection[i] = listRouter[from]->connection[i+1];
	}
	listRouter[from]->n_connections = listRouter[from]->n_connections - 1;

	for(i = to_conn;i<N_CONNECTIONS;i++){
		listRouter[to]->connection[i] = listRouter[to]->connection[i+1];
	}
	listRouter[to]->n_connections = listRouter[to]->n_connections - 1;
	return 1;	
}

/* -------------------------------------------------------------------------------- Print Functions */
/* Print a specific router */
void printRouter(Router r){
	int i = 0;

	printf("   ID: %d\n",r.ID);
	printf("   Number of connections: %d\n",r.n_connections);
	printf("   Connections:");
	while(i < r.n_connections){
		printf(" [");
		printf("ID:%d",r.connection[i].ID);
		printf(" Weight:%d Kb/s",r.connection[i].weight);
		printf("]");
		i++;
	}
	printf("\n");
}

/* Print a list of routers */
void printRouterList(Router *listRouter[]){
	int i = 0;

	printf("#---------------------------------------------------#\n");
	printf("Map of routers:\n\n");
	while(i < N_ROUTER){
		if(listRouter[i] != NULL){
//			printf("Router %d:\n",i);
			printRouter(*listRouter[i]);
		}
		i++;
	}
	printf("#---------------------------------------------------#\n");
}

/* Convert int to string */
char* intToString(int i, char *out){
	int j = 0,  k = 0;
	char s[1000], n[1000];

	while(i / 10 > 0){
		s[j++] = (char) (i % 10) + '0';
		i = i/10;
	}
	s[j]  = (char) i + '0';
	s[j+1] = '\0';
	while(j >= 0){
		out[k] = s[j];
		j--;
		k++;
	}
	out[k] = '\0';
	return out;
}

/* -------------------------------------------------------------------------------- File Functions */
/* Save the configurations of the network */
void saveConf(Router *listRouter[], char *name){
	int i = 0, cont = 0;
	char s[1000];
	FILE *fp;

	fp = fopen(name, "w");
	while(listRouter[i] != NULL){
		cont++;
		i++;
	}
	fputs(intToString(cont,s), fp);
	fputs("\n", fp);
	i = 0;
	while(listRouter[i] != NULL){
		fputs(intToString(listRouter[i]->ID,s), fp);
		fputs(" ", fp);
		fputs(intToString(listRouter[i]->n_connections,s), fp);
		int j = 0;
		while(j < listRouter[i]->n_connections){
			fputs(" ", fp);
			fputs(intToString(listRouter[i]->connection[j].ID,s), fp);
			fputs(" ", fp);
			fputs(intToString(listRouter[i]->connection[j].weight,s), fp);
			j++;
		}
		i++;
		fputs("\n", fp);
	}
	fclose(fp);
}

/* Load the configuration of the network */
void loadConf(Router *listRouter[], char *name){
	FILE *fp;

	fp = fopen(name,"r+");
	if(fp){
		int i = 0, ID, cont = 0;
		fscanf(fp, "%d",&cont);
		printf("cont = %d\n",cont);
		iniRouterList(listRouter);
		while(i < cont){
			createRouter(listRouter);
			fscanf(fp, "%d",&listRouter[i]->ID);
			fscanf(fp, "%d",&(listRouter[i]->n_connections));
			int j = 0;
			while( j < listRouter[i]->n_connections){
				fscanf(fp, "%d",&(listRouter[i]->connection[j].ID));
				fscanf(fp, "%d",&(listRouter[i]->connection[j].weight));
				j++;
			}
			i++;
		}
	}
}
