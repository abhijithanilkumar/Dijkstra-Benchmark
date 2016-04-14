/***************************************************************************************************/
// UTILITY.C
// Has all utility functions
/***************************************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "bino.h"
#include "fibo.h"

//The default log level is ERR
extern int gLogLevel;
extern int gNoOfVertex;
extern int dist[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
//Output for random mode
extern int gRandomModeOutput[TOTAL_SCHEMES][MAX_VALID_VERTICES][MAX_VALID_DENSITIES];
extern int aValidVertices[MAX_VALID_VERTICES];
extern int aValidDensities[MAX_VALID_DENSITIES];

extern int gTestTV;
extern int gTestEV;

/* Functions for debugging */
char *schemeString[TOTAL_SCHEMES] = {"SIMPLE SCHEME", "BINOMIAL SCHEME", "FIBONACCI SCHEME"};

/* Functions for debugging */
char *logString[TOTAL_LOG_LEVELS] = {"DEBUG", "INFORMATIONAL", "ERROR"};

/*
inline void loglog(const char *fmt, va_list args)
{
	vfprintf(stdout, fmt, args);
}*/

inline void myLog(int logLevel, char *fileName, int line, char *fmt, ...)
{
	va_list argList;

	if (logLevel < gLogLevel)
	{
		return;
	}
	if (NULL_PTR == fmt)
	{
		return;
	}

	printf("%s: %s(%d): ", logString[logLevel], fileName, line);

	va_start (argList, fmt);	
	//loglog(fmt, argList);
	vfprintf(stdout, fmt, argList);
	va_end (argList);
	printf("\n");
}

/* Initialize distance matrix to INFINITY */
void initDistMatrix()
{
	int i, j;
	
	for (i = 0; i < gNoOfVertex; i++)
	{
		for (j = 0; j < gNoOfVertex; j++)
		{
			if (i == j)
			{
				dist[i][j] = 0;
			}
			else
			{
				dist[i][j] = INFINITY;
			}
		}
	}
}

/* Fills up the dist[] from the adjacency list */
int copyFromAdjListToDist(AdjList *pstAdjList)
{
	int i;
	AdjList *pstTmpList = NULL_PTR, *pstTmpList2 = NULL_PTR;
	
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		return ERR;
	}

	pstTmpList = pstAdjList;

	for (i = 0; i < gNoOfVertex; i++)
	{
		pstTmpList2 = pstTmpList;

		while (pstTmpList2)
		{
			dist[pstTmpList->vertexNum][pstTmpList2->vertexNum] = pstTmpList2->distance;
			pstTmpList2 = pstTmpList2->next;
		}

		pstTmpList++;
	}
	
	return OK;
}

/* Prints the distance matrix */
void printDistMatrix()
{
	int i, j;
	
	myLog(DEBUG, "Printing Distance Matrix: ");
	
	printf("\nNodes");
	for (i = 0; i < gNoOfVertex; i++)
	{
		printf("\t%d", i);
	}
	printf("\n");
	for (i = 0; i < gNoOfVertex; i++)
	{
		printf("\t------", i);
	}
	
	for (i = 0; i < gNoOfVertex; i++)
	{
		printf("\n%d   |\t", i);
		for (j = 0; j < gNoOfVertex; j++)
		{
			if (INFINITY == dist[i][j])
			{
				printf("INF\t");
			}
			else
			{
				printf("%d\t", dist[i][j]);
			}
		}
	}
	
	printf("\n\n");
}

/* Prints the time matrix */
void printTimeMatrix(int scheme)
{
	int i, j;
	
	printf("\n\t\t\t\t--- %s ----\n", schemeString[scheme]);
	
	printf("\t");
	for (j = 0; j < MAX_VALID_DENSITIES; j++)
	{
		printf("%d\t", aValidDensities[j]);
	}

	for (i = 0; i < MAX_VALID_VERTICES; i++)
	{
		printf("\n%d\t", aValidVertices[i]);
		for (j = 0; j < MAX_VALID_DENSITIES; j++)
		{
			printf("[%d]\t", gRandomModeOutput[scheme][i][j]);
		}
	}
	printf("\n");
}

void printCircularList(BinoHeap *pstPtr)
{
	BinoHeap *pstStart;
	int i = 0;
	
	if (LDEBUG != gLogLevel)
	{
		return;
	}
	
	pstStart = pstPtr;
	
	do
	{
		printf("[%d(%d) -> %d(%d)]", pstStart->vertex, pstStart->degree,
			pstStart->next->vertex, pstStart->next->degree);
		
		pstStart = pstStart->next;
		i++;
		
		if (i > gNoOfVertex)
		{
			printf("\n Infinite Loop!");
			break;
		}
	}while(pstStart != pstPtr);
	
	printf("\n");
}

int gI;

void printHeapRec(BinoHeap *pstBinoRoot)
{
	BinoHeap *pstTmp;
	
	if (NULL_PTR == pstBinoRoot)
	{
		return;
	}
	
	printf("{ ");
	
	pstTmp = pstBinoRoot;
	
	while (pstTmp->next != pstBinoRoot)
	{
		gI++;
		
		if (gI > gNoOfVertex)
		{
			return;
		}
		printf("[(%d)%d(%d) %d(%d)] ",
			(pstTmp->parent ? pstTmp->parent->vertex : (501)),
			pstTmp->vertex, pstTmp->degree,
			pstTmp->next->vertex, pstTmp->next->degree);
			
		gTestTV++;
		
		printHeapRec(pstTmp->child);
		
		pstTmp = pstTmp->next;		
	}
	
	printf("[(%d)%d(%d) %d(%d)] ",
			(pstTmp->parent ? pstTmp->parent->vertex : (501)),
			pstTmp->vertex, pstTmp->degree,
			pstTmp->next->vertex, pstTmp->next->degree);
	gTestTV++;
	printHeapRec(pstTmp->child);
		
	printf("}");
}

void printBinoHeap(BinoHeap *pstBinoRoot)
{
	if (LDEBUG != gLogLevel)
	{
		return;
	}
	
	gI = 0;
	gTestTV = 0;
	
	printHeapRec(pstBinoRoot);
	
	if (gTestTV != gTestEV)
	{
		if ((gTestTV - gTestEV) != 1)
		{
			printf("\n\n########## ERROR ##########\n\nActual: [%d] Expected: [%d]",
			gTestTV, gTestEV);
		}
	}
	else
	{
		printf("\nOK\nActual: [%d] Expected: [%d]",
			gTestTV, gTestEV);
	}
	
	printf("\n");
}

void printFiboHeapRec(FiboHeap *pstFiboRoot)
{
	FiboHeap *pstTmp;
	
	if (NULL_PTR == pstFiboRoot)
	{
		return;
	}
	
	printf("{ ");
	
	pstTmp = pstFiboRoot;
	
	while (pstTmp->next != pstFiboRoot)
	{
		gI++;
		
		if (gI > gNoOfVertex)
		{
			return;
		}
		printf("[%d(%d) (%d)%d(%d) %d(%d)] ",
			pstTmp->prev->vertex, pstTmp->prev->degree,
			(pstTmp->parent ? pstTmp->parent->vertex : (501)),
			pstTmp->vertex, pstTmp->degree,
			pstTmp->next->vertex, pstTmp->next->degree);
			
		gTestTV++;
		
		printFiboHeapRec(pstTmp->child);
		
		pstTmp = pstTmp->next;		
	}
	
	printf("[%d(%d) (%d)%d(%d) %d(%d)] ",
			pstTmp->prev->vertex, pstTmp->prev->degree,
			(pstTmp->parent ? pstTmp->parent->vertex : (501)),
			pstTmp->vertex, pstTmp->degree,
			pstTmp->next->vertex, pstTmp->next->degree);
	gTestTV++;
	printFiboHeapRec(pstTmp->child);
		
	printf("}");
}

void printFiboHeap(FiboHeap *pstFiboRoot)
{
	if (LDEBUG != gLogLevel)
	{
		return;
	}
	
	gI = 0;
	gTestTV = 0;
	
	printFiboHeapRec(pstFiboRoot);
	
	if (gTestTV != gTestEV)
	{
		if ((gTestTV - gTestEV) != 1)
		{
			printf("\n\n########## ERROR ##########\n\nActual: [%d] Expected: [%d]",
			gTestTV, gTestEV);
		}
	}
	else
	{
		printf("\nOK\nActual: [%d] Expected: [%d]",
			gTestTV, gTestEV);
	}
	
	printf("\n");
}
