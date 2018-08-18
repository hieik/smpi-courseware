#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

#define PATH_MAX 60
#define FILE_NAME 40
#define FILE_LOCATION 100
#define FIEL_MAX 1024*10
#define VEX_MAX 1024  //max number of vertex

//Adjacency List
struct node 
{
	int vertex;
	struct node* next;
};

struct node* createNode(int);

struct Graph
{
	int vertex_num;
	struct node** adjLists;
};

struct Graph* createGraph(int vertex_num);

void addEdge(struct Graph* graph, int src, int dest);
void printGraph(struct Graph* graph);

//argument is edges file
int main(int argc, char *argv[])
{
	char cwd[PATH_MAX];
	char edge_file[FILE_NAME];
    char file_location[FILE_LOCATION];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working dir:%s\n", cwd);
	} else {
		perror("getcwd()error");
		return 1;
	}
	strcpy(edge_file, argv[1]);
	printf("file name is %s\n", edge_file);
	strcat(file_location, cwd);
	strcat(file_location, edge_file);
	printf("file location is %s\n", file_location);
	//file read operation
/*	FILE *fp;
	fp = fopen(file_location, "r");
	if (fp == NULL) 
		printf("fail to open the file\n");
	else {
		printf("the file is open\n");
		fclose(fp);
	}*/
	struct Graph* graph = createGraph(6);
	addEdge(graph, 0, 1);
	addEdge(graph, 0, 2);
	addEdge(graph, 1, 2);
	addEdge(graph, 1, 4);
	addEdge(graph, 1, 3);
	addEdge(graph, 2, 4);
	addEdge(graph, 3, 4);
	addEdge(graph, 4, 6);
	addEdge(graph, 5, 1);
	addEdge(graph, 5, 6);
	printf("test\n");
	printGraph(graph);	
	return 0;
}

struct node* createNode(int vertex)
{
	struct node* new_node = malloc(sizeof(struct node));
	new_node -> vertex = vertex;
	new_node -> next = NULL;
	return new_node;
}

struct Graph* createGraph(int vertex_num)
{
	struct Graph* graph = malloc(sizeof(struct Graph));
	graph -> vertex_num;
	graph -> adjLists = malloc(vertex_num * sizeof(struct node*));
	
	int i;
	for (i = 0; i < vertex_num; i++)
	{
		graph -> adjLists[i] = NULL;
	}
	return graph;
}
void addEdge(struct Graph* graph, int src, int dest)
{
	//Add edge from src to dest
	struct node* new_node = createNode(dest);
	new_node -> next = graph->adjLists[src];
	graph -> adjLists[src] = new_node;
	//Add edge from dest to src
	new_node = createNode(src);
	new_node -> next = graph -> adjLists[dest];
	graph -> adjLists[dest] = new_node;
}

void printGraph(struct Graph* graph)
{
	printf("printGraph\n");
	int v;
	for (v = 0; v < graph -> vertex_num; v++)
	{
		struct node* temp = graph -> adjLists[v];
		printf("\n Adjacency list of vertex %d\n", v);
		while (temp) 
		{
			printf("%d -> ", temp -> vertex);
			temp = temp -> next;
		}
		printf("\n");
	}


}
