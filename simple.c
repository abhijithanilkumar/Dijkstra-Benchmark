/***************************************************************************************************/
// SIMPLE.C
// Contains all functions for using simple scheme
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "header.h"

extern int gNoOfVertex;
extern int dist[MAX_VERTEX_NUM][MAX_VERTEX_NUM];

int calculateDistanceMatrixSimpleScheme();

/* Entry function when using simple scheme user input mode */
int simpleProc()
{
	int v1, v2, distance;
	char input[50] = {0};
	char *num;
	
	gNoOfVertex = MAX_VERTEX_NUM;
	/* Iniitalize every value to infinity */
	initDistMatrix();
	
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
			dist[v1][v2] = distance;
			
			/* Get the upperbound on the no of vertex in graph */
			gNoOfVertex = MAX(gNoOfVertex, v1+1);
			gNoOfVertex = MAX(gNoOfVertex, v2+1);
		}
	}
	
	/* Run simple scheme on dist[] */
	if (OK != calculateDistanceMatrixSimpleScheme())
	{
		myLog(ERROR, "calculateDistanceMatrixSimpleScheme failed");
		return ERR;
	}
	
	/* Print Dist Matrix */
	printDistMatrix();
	return OK;
}

/* Entry function when using simple scheme file input mode */
int simpleFileProc(char *fileName)
{
	FILE *pFile = NULL_PTR;
	char fileLine[50];
	int v1, v2, distance;
	
	if (NULL_PTR == fileName)
	{
		myLog(ERROR, "Invalid Input!");
		return ERR;
	}
	
	/* Iniitalize every value to infinity */
	gNoOfVertex = MAX_VERTEX_NUM;
	initDistMatrix();
	
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
			dist[v1][v2] = distance;
			
			/* Get the upperbound on the no of vertex in graph */
			gNoOfVertex = MAX(gNoOfVertex, v1+1);
			gNoOfVertex = MAX(gNoOfVertex, v2+1);
		}
	}
	
	fclose(pFile);
	
	/* Run simple scheme on dist[] */
	if (OK != calculateDistanceMatrixSimpleScheme())
	{
		myLog(ERROR, "calculateDistanceMatrixSimpleScheme failed");
		return ERR;
	}
	
	/* Print Dist Matrix */
	printDistMatrix();

	return OK;
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
