#define N_ROUTER 1000
#define N_CONNECTIONS 1000

typedef struct Connection{
	int ID;
	int weight;
}Connection;
typedef struct Router{
	int ID;
	int n_connections;
	Connection connection[N_CONNECTIONS];
}Router;

void 	iniRouterList(Router *[]);
void 	iniRouter(Router *[]);
Router* createRouter(Router *[]);
int 	deleteRouter(Router *[], int);

int 	createConnection(int, int, Router *[], int);

void	printRouter(Router);
void	printRouterList(Router *[]);
float sendPackage(Router*[], int, int, int);
