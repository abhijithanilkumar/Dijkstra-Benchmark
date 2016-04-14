/***************************************************************************************************/
// SIMPLE.C
// Contains all functions for using simple scheme
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "header.h"

extern int gNoOfVertex;
extern int dist[MAX_VERTEX_NUM][MAX_VERTEX_NUM];

// Structure to represent a min heap node
struct MinHeapNode
{
    int  v;
    int dist;
};

// Structure to represent a min heap
struct MinHeap
{
    int size;      // Number of heap nodes present currently
    int capacity;  // Capacity of min heap
    int *pos;     // This is needed for decreaseKey()
    struct MinHeapNode **array;
};

// A utility function to create a new Min Heap Node
struct MinHeapNode* newMinHeapNode(int v, int dist)
{
    struct MinHeapNode* minHeapNode =
           (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// A utility function to create a Min Heap
struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap =
         (struct MinHeap*) malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array =
         (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// A utility function to swap two nodes of min heap. Needed for min heapify
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// A standard function to heapify at given idx
// This function also updates position of nodes when they are swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size &&
        minHeap->array[left]->dist < minHeap->array[smallest]->dist )
      smallest = left;

    if (right < minHeap->size &&
        minHeap->array[right]->dist < minHeap->array[smallest]->dist )
      smallest = right;

    if (smallest != idx)
    {
        // The nodes to be swapped in min heap
        struct MinHeapNode* smallestNode = minHeap->array[smallest];
        struct MinHeapNode* idxNode = minHeap->array[idx];

        // Swap positions
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        // Swap nodes
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

// A utility function to check if the given minHeap is ampty or not
int isEmpty(struct MinHeap* minHeap)
{
    return minHeap->size == 0;
}

// Standard function to extract minimum node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;

    // Store the root node
    struct MinHeapNode* root = minHeap->array[0];

    // Replace root node with last node
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // Update position of last node
    minHeap->pos[root->v] = minHeap->size-1;
    minHeap->pos[lastNode->v] = 0;

    // Reduce heap size and heapify root
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// Function to decreasy dist value of a given vertex v. This function
// uses pos[] of min heap to get the current index of node in min heap
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
    // Get the index of v in  heap array
    int i = minHeap->pos[v];

    // Get the node and update its dist value
    minHeap->array[i]->dist = dist;

    // Travel up while the complete tree is not hepified.
    // This is a O(Logn) loop
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        // Swap this node with its parent
        minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
        swapMinHeapNode(&minHeap->array[i],  &minHeap->array[(i - 1) / 2]);

        // move to parent index
        i = (i - 1) / 2;
    }
}

// A utility function to check if a given vertex
// 'v' is in min heap or not
int isInMinHeap(struct MinHeap *minHeap, int v)
{
   if (minHeap->pos[v] < minHeap->size)
     return 1;
   return 0;
}

int binaryProc()
{
	int v1, v2, distance;
	char input[50] = {0};
	char *num;
	AdjList *pstAdjList = NULL_PTR;

	gNoOfVertex = MAX_VERTEX_NUM;

	/* Allocate sufficient memory for adjacency list */
	pstAdjList = initAdjList();
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "initAdjList failed!");
		return ERR;
	}

	gNoOfVertex = 0;

	/* Get user input and keep it in dist matrix */
	printf("Enter graph elements in the following fashion <V1> <V2> <Cost>:\n"
		"When you are done, give '*' as input\n");

	while (TRUE)
	{
		fgets(input, 50, stdin);
		if ('*' == input[0])
		{
			break;
		}

		sscanf(input, "%d %d %d", &v1, &v2, &distance);

		if ((v1 >= 0) && (v1 < MAX_VERTEX_NUM)
			&& (v2 >= 0) && (v2 < MAX_VERTEX_NUM)
			&& (distance >= 1) && (distance < MAX_EDGE_COST))
		{
			/* Get the upperbound on the no of vertex in graph */
			gNoOfVertex = MAX(gNoOfVertex, v1+1);
			gNoOfVertex = MAX(gNoOfVertex, v2+1);

			if (NULL_PTR == addToAdjList(pstAdjList, v1, v2, distance))
			{
				myLog(ERROR, "addToAdjList failed!");
				continue;
			}
		}
	}

	/* Run binomial scheme */
	if (INVALID_TIME == runBinaryScheme(pstAdjList))
	{
		myLog(ERROR, "runBinaryScheme failed!");
		return ERR;
	}

	/* Print the distance matrix */
	printDistMatrix();

	return OK;
}

/* Entry function for user file input */
int binaryFileProc(char *fileName)
{
	FILE *pFile = NULL_PTR;
	char fileLine[50];
	int v1, v2, distance;
	AdjList *pstAdjList;

	if (NULL_PTR == fileName)
	{
		myLog(ERROR, "Invalid Input!");
		return ERR;
	}

	gNoOfVertex = MAX_VERTEX_NUM;

	/* Allocate sufficient memory for adjacency list */
	pstAdjList = initAdjList();
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "initAdjList failed!");
		return ERR;
	}

	gNoOfVertex = 0;

	/* Check if the file is present */
	pFile = fopen(fileName, "r");
	if (NULL_PTR == pFile)
	{
		myLog(ERROR, "File [%s] does not exist!", fileName);
		return OK;
	}

	/* Read file and put it in char array */
	while (NULL != fgets(fileLine, 50, pFile))
	{
		if ('*' == fileLine[0])
		{
			break;
		}

		sscanf(fileLine, "%d %d %d", &v1, &v2, &distance);

		/* Take into account only valid vertices and edges */
		if ((v1 >= 0) && (v1 < MAX_VERTEX_NUM)
			&& (v2 >= 0) && (v2 < MAX_VERTEX_NUM)
			&& (distance >= 1) && (distance < MAX_EDGE_COST))
		{
			/* Get the upperbound on the no of vertex in graph */
			gNoOfVertex = MAX(gNoOfVertex, v1+1);
			gNoOfVertex = MAX(gNoOfVertex, v2+1);

			if (NULL_PTR == addToAdjList(pstAdjList, v1, v2, distance))
			{
				myLog(ERROR, "addToAdjList failed!");
				continue;
			}
		}
	}

	fclose(pFile);

	/* Run binomial scheme */
	if (INVALID_TIME == runBinaryScheme(pstAdjList))
	{
		myLog(ERROR, "runBinaryScheme failed!");
		return ERR;
	}

	/* Print the distance matrix */
	printDistMatrix();

	return OK;
}

int runBinaryScheme(AdjList *pstAdjList)
{
	struct MinHeap* minHeap = createMinHeap(gNoOfVertex);
	AdjList *pCrawl = NULL_PTR;
	clock_t startTime, time;
	int src;

	/*if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		return INVALID_TIME;
	}*/

	startTime = clock();

	/* We have an empty binomial heap now */
  for (src = 0; src < gNoOfVertex; ++src)
  {
      // Initialize min heap with all vertices. dist value of all vertices
      int v;
      for (v = 0; v < gNoOfVertex; ++v)
      {
          dist[src][v] = INFINITY;
          minHeap->array[v] = newMinHeapNode(v, dist[src][v]);
          minHeap->pos[v] = v;
      }

      // Make dist value of src vertex as 0 so that it is extracted first
      minHeap->array[src] = newMinHeapNode(src, dist[src][src]);
      minHeap->pos[src]   = src;
      dist[src][src] = 0;
      decreaseKey(minHeap, src, dist[src][src]);

      // Initially size of min heap is equal to V
      minHeap->size = gNoOfVertex;

      // In the followin loop, min heap contains all nodes
      // whose shortest distance is not yet finalized.
      while (!isEmpty(minHeap))
      {
          // Extract the vertex with minimum distance value
          struct MinHeapNode* minHeapNode = extractMin(minHeap);
          int u = minHeapNode->v; // Store the extracted vertex number

          // Traverse through all adjacent vertices of u (the extracted
          // vertex) and update their distance values
          //struct AdjListNode* pCrawl = graph->array[u].head;
          pCrawl = pstAdjList[u].next;
          while (pCrawl != NULL)
          {
              int v = pCrawl->vertexNum;

              // If shortest distance to v is not finalized yet, and distance to v
              // through u is less than its previously calculated distance
              if (isInMinHeap(minHeap, v) && dist[src][u] != INFINITY &&
                                            pCrawl->distance + dist[src][u] < dist[src][v])
              {
                  dist[src][v] = dist[src][u] + pCrawl->distance;

                  // update distance value in min heap also
                  decreaseKey(minHeap, v, dist[src][v]);
              }
              pCrawl = pCrawl->next;
          }
      }
  }

	time = ((clock() - startTime) / CLOCKS_PER_MSEC);

	myLog(INFO, "Time taken by BinaryDijkstra: [%d] msec.", time);

	return time;
}

/* Returns the closest vertex to srcVertex which is not yet visited */
int simpleSchemeGetClosestVertex(int srcVertex, char *visited)
{
	int closestVertex = MAX_VERTEX_NUM;
	int closestDist = INFINITY;
	int i;

	if ((NULL_PTR == visited)
		|| (srcVertex >= gNoOfVertex))
	{
		myLog(ERROR, "Invalid Input! "
			"srcVertex: [%d], Vertices: [%d]",
			srcVertex, gNoOfVertex);
		return ERR;
	}

	myLog(DEBUG, "Finding closest vertex to [%d]", srcVertex);

	for (i = 0; i < gNoOfVertex; i++)
	{
		if ((!visited[i]) && (dist[srcVertex][i] < closestDist))
		{
			closestVertex = i;
			closestDist = dist[srcVertex][i];

			myLog(DEBUG, "Updating distance between [%d] and [%d] from [%d] to [%d]",
				srcVertex, i, dist[srcVertex][i], closestDist);
		}
	}

	myLog(DEBUG, "Closest vertex to [%d] is [%d] at distance: [%d]",
		srcVertex, closestVertex, closestDist);

	return closestVertex;
}

/* Relaxes the distance of all the vertices adjacent to the given vertex */
int simpleSchemeRelax(int sourceVertex, int closestVertex, char *visited)
{
	int i = 0, distance;

	myLog(DEBUG, "Relaxing with sourceVertex: [%d] "
		"and closestVertex: [%d].\nVisited Nodes:", sourceVertex, closestVertex);

	for (i = 0; i < gNoOfVertex; i++)
	{
		myLog(DEBUG, "%d: %d ", i, visited[i]);
	}

	/* For every neighbour of vertex */
	for (i = 0; i < gNoOfVertex; i++)
	{
		if ((!visited[i])
			&& (i != closestVertex)
			&& (dist[closestVertex][i] != INFINITY))
		{
			/* i is the neighbour vertex, relax (sourceVertex, vertex, i) */
			distance = dist[sourceVertex][closestVertex] + dist[closestVertex][i];
			myLog(DEBUG, "[%d] is a neighbour to [%d] at distance [%d]."
				"Other distance: [%d]", i, closestVertex, dist[closestVertex][i],
				distance);
			if (distance < dist[sourceVertex][i])
			{
				dist[sourceVertex][i] = distance;
			}
		}
	}

	return OK;
}


int calculateDistanceMatrixSimpleScheme()
{
	int sourceVertex, closestVertex, distance;
	char nodeVisited[MAX_VERTEX_NUM] = {FALSE};

	for (sourceVertex = 0; sourceVertex < gNoOfVertex; sourceVertex++)
	{
		/* Init visited flags */
		(void)memset(nodeVisited, FALSE, gNoOfVertex);

		/* Mark the source node as visited */
		nodeVisited[sourceVertex] = TRUE;

		while (TRUE)
		{
			/* Get the vertex closest to sourceVertex */
			closestVertex = simpleSchemeGetClosestVertex(sourceVertex, nodeVisited);
			if (MAX_VERTEX_NUM == closestVertex)
			{
				/* We could not find any more vertices closest to source */
				break;
			}
			else if (ERR == closestVertex)
			{
				myLog(ERROR, "simpleSchemeGetClosestVertex failed!");
				return ERR;
			}

			/* Relax all vertices adjacent to closestVertex */
			if (ERR == simpleSchemeRelax(sourceVertex, closestVertex, nodeVisited))
			{
				myLog(ERROR, "simpleSchemeRelax failed!");
				return ERR;
			}

			/* Mark the closestVertex as visited */
			nodeVisited[closestVertex] = TRUE;
		}
	}

	return OK;
}

/* When Simple scheme is to be run in Random Mode */
int runSimpleSchemeForRandomMode(AdjList *pstAdjList)
{
	clock_t startTime, time;

	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		return INVALID_TIME;
	}

	/* Iniitalize every value to infinity */
	initDistMatrix();

	/* Copy from adj list to dist[] and then apply Dijkstra's algo on dist[] */
	if (OK != copyFromAdjListToDist(pstAdjList))
	{
		myLog(ERROR, "copyFromAdjListToDist failed!");
		return ERR;
	}

	startTime = clock();

	/* Apply Dijkstra's algorithm for all vertices as source */
	if (OK != calculateDistanceMatrixSimpleScheme())
	{
		myLog(ERROR, "calculateDistanceMatrixSimpleScheme failed!");
		return INVALID_TIME;
	}

	time = ((clock() - startTime) / CLOCKS_PER_MSEC);

	myLog(INFO, "Time taken by SimpleDijkstra: [%d] msec.", time);

	/* Print the matrix and see if we really achived anything! */
	//printDistMatrix();

	return time;
}
