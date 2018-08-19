#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

#define PATH_MAX 60
#define FILE_NAME 40
#define FILE_LOCATION 100
#define FIEL_MAX 1024*10
#define VEX_MAX 1024  //max number of vertex
#define QUEUE_SIZE 1024
struct queue 
{
	int items[QUEUE_SIZE];
	int front;
	int rear;
};

struct queue* createQueue();
void enqueue(struct queue* q, int);
int dequeue(struct queue* q);
void display(struct queue* q);
int isEmpty(struct queue* q);
void printQueue(struct queue* q);

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
	int* visited;
};

struct Graph* createGraph(int vertex_num);
struct Graph* bfsTree(struct Graph* graph, int start_vertex);
void addEdge(struct Graph* graph, int src, int dest);
void printGraph(struct Graph* graph);
void bfs(struct Graph* graph, int strat_vertex);
void dfs(struct Graph* graph, int strat_vertex);
void initGraph(struct Graph* graph);
//argument is edges file
int main(int argc, char *argv[])
{
	char cwd[PATH_MAX];
	char edge_file[FILE_NAME];
	char file_location[FILE_LOCATION] = "platform/random_d6_64-bTrue.edges";
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working dir:%s\n", cwd);
	} else {
		perror("getcwd()error");
		return 1;
	}
	if (argv[1] == NULL)
	{
		printf("please input file name.\n");
		return 1;
	}
	strcpy(edge_file, argv[1]);
	printf("file name is %s\n", edge_file);
//	strcat(file_location, cwd);
//	strcat(file_location, "/");
//	strcat(file_location, edge_file);
	printf("file location is %s\n", file_location);
	//file read operation
	FILE *fp;
	fp = fopen(file_location, "r");
	if (fp == NULL) 
		printf("fail to open the file\n");
	else {
		printf("the file is open\n");
		fclose(fp);
	}
	struct Graph* graph = createGraph(6);
	addEdge(graph, 0, 1);
	addEdge(graph, 0, 2);
	addEdge(graph, 1, 2);
	addEdge(graph, 1, 4);
	addEdge(graph, 1, 3);
	addEdge(graph, 2, 4);
	addEdge(graph, 3, 4);
	addEdge(graph, 5, 1);
	printGraph(graph);	
//	bfs(graph, 0);
	initGraph(graph);
//	dfs(graph, 0);
	struct Graph* spanning_tree = bfsTree(graph, 0);
	printGraph(spanning_tree);
	dfs(spanning_tree, 0);
	return 0;
}


//operation of Queue
struct queue* createQueue()
{
	struct queue* q = malloc(sizeof(struct queue));
	q -> front = -1;
	q -> rear = -1;
	return q;
}

int isEmpty(struct queue* q)
{
	if (q->rear == -1)
 		return 1;
	else 
		return 0;
}

void enqueue(struct queue* q, int value)
{
	if (q->rear == QUEUE_SIZE-1)
		printf("\nQueue is Full.\n");
	else {
		if (q->front == -1)
			q->front = 0;
		q->rear++;
		q->items[q->rear] = value;
	}
}

int dequeue(struct queue* q)
{
	int item;
	if (isEmpty(q))
	{
		printf("Queue is empty\n");
		item = -1;
	} else {
		item = q->items[q->front];
		q->front++;
		if (q->front > q->rear)
		{
			printf("Resetting queue\n");
			q->front = q->rear = -1;
		}
	}
	return item;
} 

void printQueue(struct queue* q)
{
	int i = q->front;
	
	if (isEmpty(q)) 
	{
		printf("Queue is empty\n");
	} else {
		printf("\nQueue contains \n");
		for (i = q->front; i < q->rear+1; i++)
		{
			printf("%d ", q->items[i]);
		}
		printf("\n");
	}
}



//operations of Graph
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
	graph -> vertex_num = vertex_num;
	graph -> adjLists = malloc(vertex_num * sizeof(struct node*));
	graph -> visited = malloc(vertex_num * sizeof(int));
	int i;
	for (i = 0; i < vertex_num; i++)
	{
		graph -> adjLists[i] = NULL;
		graph -> visited[i] = 0;
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
	printf("\n%d vertex\n", graph -> vertex_num);
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
//breadth frist search
void bfs(struct Graph* graph, int start_vertex)
{
	struct queue* q = createQueue();
	graph->visited[start_vertex] = 1;
	enqueue(q, start_vertex);
	while (!isEmpty(q)) {
		printQueue(q);
		int current_vertex = dequeue(q);
		printf("Visited %d\n", current_vertex);
		struct node* temp = graph->adjLists[current_vertex];
		while(temp) {
			int adj_vertex = temp->vertex;
			if (graph->visited[adj_vertex] == 0) 
			{
				graph->visited[adj_vertex] = 1;
				enqueue(q, adj_vertex);
			}
			temp = temp->next;
		}		
	}
}
//breadth first spanning tree
struct Graph* bfsTree(struct Graph* graph, int start_vertex)
{
	int vertex_num = graph->vertex_num;
	int src = -1, dest = -1;
	int step = 0;
	struct Graph* spanning_tree = createGraph(vertex_num);
	struct queue* q = createQueue();
	graph->visited[start_vertex] = 1;
	enqueue(q, start_vertex);
	while (!isEmpty(q)) {
		printQueue(q);
		int current_vertex = dequeue(q);
		printf("Visited %d\n", current_vertex);
		src = current_vertex;
		struct node* temp = graph->adjLists[current_vertex];
		while(temp) {
			int adj_vertex = temp->vertex;
			if (graph->visited[adj_vertex] == 0)	
			{
				graph->visited[adj_vertex] = 1;
				enqueue(q, adj_vertex);
				dest = adj_vertex;
				printf("src=%d, dest=%d\n", src, dest);
				addEdge(spanning_tree, src, dest);
			}
			temp = temp->next;
		}
	}
	printf("step=%d\n", step);
	initGraph(graph);
	return spanning_tree;	
}
//deep first search
void dfs(struct Graph* graph, int start_vertex)
{
	struct node* adjList = graph->adjLists[start_vertex];
	struct node* temp = adjList;
	graph->visited[start_vertex] = 1;
	printf("Visited %d \n", start_vertex);
	
	while (temp != NULL) {
		int connected_vertex = temp->vertex;
		if (graph->visited[connected_vertex] == 0) {
			dfs(graph, connected_vertex);
		}
		temp = temp ->next;
	}
}
//set visited nodes unvisited
void initGraph(struct Graph* graph)
{
	for (int i = 0; i < graph->vertex_num; i++)
	{
		graph -> visited[i] = 0;
	}
}








