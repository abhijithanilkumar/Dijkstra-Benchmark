/***************************************************************************************************/
// RANDOM.C
// Contains all functions when user asks for random genration of graph
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#include <stdio.h>
#include <time.h>
#include "header.h"

//Stores the number to vertices 
extern int gNoOfVertex;
//Stores the desity of graph in percentage
extern int gGraphDensity;
//function which allocates memory for adjlist based on number of vertices and density
extern AdjList* initAdjList();

//Output for random mode
int gRandomModeOutput[TOTAL_SCHEMES][MAX_VALID_VERTICES][MAX_VALID_DENSITIES];
int aValidVertices[MAX_VALID_VERTICES] = {100, 200, 300, 500};
int aValidDensities[MAX_VALID_DENSITIES] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

/* Returns random number between start and end inclusive */
int randomWithRange(int start, int end)
{
	return (start + (rand() % (end - start + 1)));
}

int randomProc()
{
	AdjList *pstAdjList = NULL_PTR;
	int i, j, k, timeMsec, totalMsec;
	
	/* Loop against all valid vertices and valid densities */
	for (i = 0; i < MAX_VALID_VERTICES; i++)
	{
		for (j = 0; j < MAX_VALID_DENSITIES; j++)
		{
			/* Assign the current values to global variables. These
			variable will be used by adjlist.c functions */
			gNoOfVertex = aValidVertices[i];
			gGraphDensity = aValidDensities[j];

			/* Prepare Adjacency List */
			pstAdjList = initAdjList();
			if (NULL_PTR == pstAdjList)
			{
				myLog(ERROR, "initAdjList failed!");
				return ERR;
			}

			/* Generate Graph */
			if (OK != buildRandomAdjList(pstAdjList))
			{
				myLog(ERROR, "buildAdjList failed!");
				
				/* deinit the adjacency list */
				destroyAdjList(pstAdjList);
				
				return ERR;
			}

			/* Check if the graph is disconnected. If yes, connect the graph */
			if (OK != connectGraph(pstAdjList))
			{
				myLog(ERROR, "connectGraph failed!");
				
				/* deinit the adjacency list */
				destroyAdjList(pstAdjList);
				
				return ERR;
			}
			
			/************************** Run Simple Scheme *****************************/
			#if 1
			
			/* Run a scheme multiple times and get average */
			myLog(INFO, "Running [%d] times", MAX_SCHEME_RUNS);
			totalMsec = 0;
			for (k = 0; k < MAX_SCHEME_RUNS; k++)
			{
				timeMsec = runSimpleSchemeForRandomMode(pstAdjList);
				if (INVALID_TIME == timeMsec)
				{
					myLog(ERROR, "runSimpleSchemeForRandomMode failed!");
				
					/* deinit the adjacency list */
					destroyAdjList(pstAdjList);
				
					return ERR;
				}
				
				totalMsec += timeMsec;
			}
			
			/* Save the time taken into a matrix */
			gRandomModeOutput[SIMPLE_SCHEME][i][j] = (totalMsec/MAX_SCHEME_RUNS);
			#endif
			
			#if 1
			/************************* Run Binomial Scheme ****************************/
			/* Run a scheme multiple times and get average */
			myLog(INFO, "Running [%d] times", MAX_SCHEME_RUNS);
			totalMsec = 0;
			for (k = 0; k < MAX_SCHEME_RUNS; k++)
			{
				timeMsec = runBinoScheme(pstAdjList);
				if (INVALID_TIME == timeMsec)
				{
					myLog(ERROR, "runBinoScheme failed!");
				
					/* deinit the adjacency list */
					destroyAdjList(pstAdjList);
				
					return ERR;
				}
				
				totalMsec += timeMsec;
			}
			
			/* Save the time taken into a matrix */
			gRandomModeOutput[BINO_SCHEME][i][j] = (totalMsec/MAX_SCHEME_RUNS);
			#endif
			
			/************************** Run Fibonacci Scheme **************************/
			#if 1
			
			/* Run a scheme multiple times and get average */
			myLog(INFO, "Running [%d] times", MAX_SCHEME_RUNS);
			totalMsec = 0;
			for (k = 0; k < MAX_SCHEME_RUNS; k++)
			{
				timeMsec = runFiboScheme(pstAdjList);
				if (INVALID_TIME == timeMsec)
				{
					myLog(ERROR, "runFiboScheme failed!");
				
					/* deinit the adjacency list */
					destroyAdjList(pstAdjList);
				
					return ERR;
				}
				
				totalMsec += timeMsec;
			}
			
			/* Save the time taken into a matrix */
			gRandomModeOutput[FIBO_SCHEME][i][j] = (totalMsec/MAX_SCHEME_RUNS);
			#endif

			/* deinit the adjacency list */
			destroyAdjList(pstAdjList);
		}
	}

	/* Output the calculated matrix */	
	printTimeMatrix(SIMPLE_SCHEME);
	printTimeMatrix(BINO_SCHEME);
	printTimeMatrix(FIBO_SCHEME);

	return OK;
}
