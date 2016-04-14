/***************************************************************************************************/
// ADJLIST.C
// Contains all functions for Adjacency List
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#include <stdio.h>
#include <malloc.h>
#include "header.h"

//Stores the number to vertices 
extern int gNoOfVertex;
//Stores the desity of graph in percentage
extern int gGraphDensity;
//The output is stored here. For simplicity take static memory and that too for
//the largest possible dist[] matrix
extern int dist[MAX_VERTEX_NUM][MAX_VERTEX_NUM];

/* Returns a pointer to an array which is the starting of adjList */
AdjList* initAdjList()
{
	AdjList *pstAdjList = NULL_PTR;
	int i, size;

	size = gNoOfVertex;

	/*Allocate the size of the array. Each array index will maintain a list
		If 2 is connected to 5 with edge weight 10,
		at piAdjList[1], next will point to a node whose vertextNum
		is 5 and distance is 10. */
	pstAdjList = (AdjList *) malloc(sizeof(AdjList)*size);

	/* If memory was allocted */
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "malloc failed!");
		return NULL_PTR;
	}
	
	/* Initialize */
	for (i = 0; i < size; i++)
	{
		pstAdjList[i].vertexNum = i;
		pstAdjList[i].distance  = 0;
		pstAdjList[i].next = NULL_PTR;
	}

	return pstAdjList;
}

/* Adds nodes to the given adj list. Returns NULL if failure or a valid pointer if success */
AdjList* addToAdjList(AdjList *pstAdjList, int index, int vertex, int distance)
{
	AdjList *pstListNode = NULL_PTR, *pstTraverser = NULL_PTR;

	if ((NULL_PTR == pstAdjList)
		|| (index < 0)
		|| (index >= gNoOfVertex))
	{
		myLog(ERROR, "Invalid Input!");
		return NULL_PTR;
	}

	/* Allocate a new node and add it to list */
	pstListNode = (AdjList *) malloc(sizeof(AdjList));
	if (NULL_PTR == pstListNode)
	{
		myLog(ERROR, "malloc failed!");
		return NULL_PTR;
	}

	/* Fill in the details */
	pstListNode->vertexNum = vertex;
	pstListNode->distance = distance;
	pstListNode->next = NULL_PTR;

	/* Attach to the end of linked list */
	pstTraverser = &(pstAdjList[index]);

	while(pstTraverser->next != NULL_PTR)
	{
		pstTraverser = pstTraverser->next;
	}

	pstTraverser->next = pstListNode;

	return pstAdjList;
}

/* Return TRUE if a pair exists and updates the output variable distance
else return FALSE if not found. ERR on sanity check failure */
int doesPairExistsInAdjList(AdjList *pstAdjList, int vertex1, int vertex2, int *distance)
{
	AdjList *pstLstPtr = NULL_PTR;

	/* If failure, return ERR so that
		the vertex is not added  */
	if ((NULL_PTR == pstAdjList)
		|| (vertex1 < 0)
		|| (vertex1 >= gNoOfVertex)
		|| (vertex2 < 0)
		|| (vertex2 >= gNoOfVertex)
		|| (NULL_PTR == distance))
	{
		return ERR;
	}

	//Find if vertex2 is adjacent to vertex1
	*distance = -1;
	pstLstPtr = &(pstAdjList[vertex1]);

	while(pstLstPtr != NULL_PTR)
	{
		if (pstLstPtr->vertexNum == vertex2)
		{
			*distance = pstLstPtr->distance;
			return TRUE;
		}
		pstLstPtr = pstLstPtr->next;
	}

	return FALSE;
}

/* Use this function for debugging purpose */
void printAdjList(AdjList *pstAdjList)
{
	int i;
	AdjList *pstTmpList = NULL_PTR, *pstTmpList2 = NULL_PTR;

	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		return;
	}
	
	myLog(DEBUG, "Printing Adjacency List:");
	pstTmpList = pstAdjList;

	for (i = 0; i < gNoOfVertex; i++)
	{
		pstTmpList2 = pstTmpList;

		while (pstTmpList2)
		{
			printf("(%d, %d) = %d\n",
				pstTmpList->vertexNum, pstTmpList2->vertexNum, pstTmpList2->distance);
			pstTmpList2 = pstTmpList2->next;
		}

		pstTmpList++;
	}
}

/* Build an adjacency list using random variables.
Returns OK on success and ERR on failure */
int buildRandomAdjList(AdjList *pstAdjList)
{
	int noOfEdges;
	int i, vertex1, vertex2, distance;
	//AdjList *pstTmpList, *pstTmpList2;
	
	#if 0
	FILE *pFile;
	char output[50];
	
	pFile = fopen("random.txt", "w");
	if (NULL_PTR == pFile)
	{
		myLog(ERROR, "Random file creation failed!");
		return ERR;
	}
	#endif

	srand(time(NULL));

	// edges in graph = total edges * density%.
	noOfEdges = ((gNoOfVertex) * (gNoOfVertex - 1));
	noOfEdges = ((noOfEdges * gGraphDensity) / 100);

	myLog(DEBUG, "Vertices: [%d], Edges: "
		"[%d] Density: [%d]", gNoOfVertex, noOfEdges, gGraphDensity);
		
	myLog(INFO, "Building graph randomly with [%d] vertices and [%d] density ([%d] edges).",
		gNoOfVertex, gGraphDensity, noOfEdges);

	/* The following loop builds the graph by adding
	edges in the adjacency list */
	for (i = 0; i < noOfEdges; i++)
	{
		vertex1 = 0;
		vertex2 = 0;

		/* randomly get a end vertex. if the pair exists,
		change both vertex1 and vertex2 and try again. &distance is dummy */
		while ((vertex2 == vertex1)
			|| (FALSE != doesPairExistsInAdjList(pstAdjList, vertex1, vertex2, &distance)))
		{
			//randomly get a start and end vertex
			vertex1 = randomWithRange(0, gNoOfVertex-1);
			vertex2 = randomWithRange(0, gNoOfVertex-1);
		}

		distance = randomWithRange(1, MAX_EDGE_COST);

		/* As we have got all 3 variables, add to AdjList */
		if (NULL_PTR == addToAdjList(pstAdjList, vertex1, vertex2, distance))
		{
			myLog(ERR, "addToAdjList failed!");
			return ERR;
		}

		myLog(DEBUG, "%d: Adding (%d, %d) = %d", i, vertex1, vertex2, distance);
		
		//sprintf(output, "%d %d %d\n", vertex1, vertex2, distance);
		//fputs(output, pFile);
	}

	/********* UNCOMMENT TO CHECK ADJ LIST *************/
	/******* use vertex = 5, density = 30 *************/
	#if 0
	/* Print Adj Matrix and double check */
	printAdjList(pstAdjList);
	#endif
	
	//fclose(pFile);
	
	return OK;
}

/* Checks if the graph is connected. If not, adds edges and connects the graph
or else returns false */
int connectGraph(AdjList *pstAdjList)
{
	int i, j, otherVertex, distance, isConnected = TRUE;
	
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		return;
	}
	/* If any vertex has 0 outward edges, create a random outward edge to connect the graph */
	myLog(INFO, "Checking if graph is connected!");
	
	/* Find the vertex with 0 outward edge. Fixing this increases the probability
		of getting a connected graph */
	for (i = 0; i < gNoOfVertex; i++)
	{
		if (NULL_PTR == pstAdjList[i].next)
		{
			otherVertex = i;
			/* Choose a random vertex to connect with. Also choose an edge with
				random weight */
			while (otherVertex == i)
			{
				otherVertex = randomWithRange(0, gNoOfVertex-1);
			}
			distance = randomWithRange(1, MAX_EDGE_COST);

			/* As we have got all 3 variables, add to AdjList */
			if (NULL_PTR == addToAdjList(pstAdjList, i, otherVertex, distance))
			{
				myLog(ERR, "addToAdjList failed!");
				return ERR;
			}

			myLog(DEBUG, "%d: Adding (%d, %d) = %d", i, i, otherVertex, distance);
		}
	}
	
	myLog(INFO, "Running simple scheme to check if the graph is connected");
	
	while (INVALID_TIME != runSimpleSchemeForRandomMode(pstAdjList))
	{
		isConnected = TRUE;
		
		/* Check the dist[][] for any INFINITIES */
		for (i = 0; i < gNoOfVertex; i++)
		{
			for (j = 0; j < gNoOfVertex; j++)
			{
				if (INFINITY == dist[i][j])
				{
					distance = randomWithRange(1, MAX_EDGE_COST);

					/* As we have got all 3 variables, add to AdjList */
					if (NULL_PTR == addToAdjList(pstAdjList, i, j, distance))
					{
						myLog(ERR, "addToAdjList failed!");
						return ERR;
					}
					
					myLog(DEBUG, "Adding (%d, %d) = %d", i, j, distance);
					isConnected = FALSE;
				}
			}
		}
		
		if (TRUE == isConnected)
		{
			break;
		}
		
		myLog(INFO, "Running simple scheme again to check if the graph is connected");
	}
	
	if (TRUE == isConnected)
	{
		myLog(INFO, "The graph is already connected!");
	}
	else
	{
		myLog(INFO, "The graph is now connected!");
	}
	
	return OK;
}

/* Deallocates memory used by the adj list */
void destroyAdjList(AdjList *pstAdjList)
{
	int i, j;
	AdjList *pstTraverser = NULL_PTR, *pstTmp = NULL_PTR;
	
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		return;
	}
	
	for (i = 0; i < gNoOfVertex; i++)
	{
		pstTraverser = &(pstAdjList[i]);
		
		/* Delete the horizontal lists */
		while (NULL_PTR != pstTraverser->next)
		{
			pstTmp = pstTraverser->next;
			pstTraverser->next = pstTmp->next;
			
			free(pstTmp);
			pstTmp = NULL_PTR;
		}
	}

	/* Delete the vertical list */
	free(pstAdjList);
	pstAdjList = NULL_PTR;
}
