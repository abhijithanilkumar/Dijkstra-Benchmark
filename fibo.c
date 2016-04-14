/***************************************************************************************************/
// FIBO.C
// Contains all functions for using fibonacci scheme
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#include "fibo.h"
#include <string.h>
#include <time.h>

extern int gNoOfVertex;
extern int dist[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
extern AdjList* initAdjList();

int gTestTV;
int gTestEV;

extern int gFCombine;

/* Pointer Flags. Assuming maximum 500 nodes, the maxdegree of the fibonacci tree can
go only to maximum of log_2 (500) <= 9 */
int *fiboPtrFlags[MAX_FIBO_DEGREE] = {NULL_PTR};

/* Entry function for user input */
int fiboProc()
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
	
	/* Run fibonacci scheme */
	if (INVALID_TIME == runFiboScheme(pstAdjList))
	{
		myLog(ERROR, "runFiboScheme failed!");
		return ERR;
	}
	
	/* Print the distance matrix */
	printDistMatrix();

	return OK;
}

/* Entry function for user file input */
int fiboFileProc(char *fileName)
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
	
	/* Run fibonacci scheme */
	if (INVALID_TIME == runFiboScheme(pstAdjList))
	{
		myLog(ERROR, "runFiboScheme failed!");
		return ERR;
	}
	
	/* Print the distance matrix */
	printDistMatrix();
	
	return OK;
}

/* Functions for Fibonacci Heap */

/* Creates an empty fibo heap and initializes distance matrix */
FiboHeap* initFibonacciHeap(AdjList *pstAdjList)
{
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		/* return a non-null pointer */
		return (FiboHeap *)(0x1);
	}
	/* Iniitalize every value to infinity, distance to itself is 0 */
	initDistMatrix();
	
	/* Copy from adj list to dist[] and then apply Dijkstra's algo on dist[] */
	if (OK != copyFromAdjListToDist(pstAdjList))
	{
		myLog(ERROR, "copyFromAdjListToDist failed!");
		/* return a non-null pointer */
		return (FiboHeap *)(0x1);
	}

	return NULL_PTR;
}

/* Returns a single init'd FiboHeap Node */
FiboHeap* getBlankFiboNode()
{
	FiboHeap *pstFiboNode = NULL_PTR;
	
	pstFiboNode = (FiboHeap *)malloc (sizeof(FiboHeap));
	if (NULL_PTR == pstFiboNode)
	{
		myLog(ERROR, "Failed to allocate %d bytes!", sizeof(FiboHeap));
		return NULL_PTR;
	}
	
	pstFiboNode->key = INFINITY;
	pstFiboNode->vertex = MAX_VERTEX_NUM;
	pstFiboNode->degree = 0;
	pstFiboNode->childCut = FALSE;
	pstFiboNode->child = NULL_PTR;
	pstFiboNode->parent = NULL_PTR;
	pstFiboNode->prev = pstFiboNode;
	pstFiboNode->next = pstFiboNode;
	
	return pstFiboNode;
}

/* Insert a tree into the fibonacci heap return the pointer to the new node */
FiboHeap* fiboInsert(FiboHeap **pstFiboHeapRoot, int vertex, int distance)
{
	FiboHeap *pstFiboNode = NULL_PTR;
	
	/* Do not check for *pstFiboHeapRoot as it can be NULL_PTR */
	if (NULL_PTR == pstFiboHeapRoot)
	{
		myLog(ERROR, "Invalid Input!");
		return NULL_PTR;
	}

	pstFiboNode = getBlankFiboNode();
	if (NULL_PTR == pstFiboNode)
	{
		myLog(ERROR, "getBlankFiboNode failed!");
		return NULL_PTR;
	}
	
	pstFiboNode->key = distance;
	pstFiboNode->vertex = vertex;

	/* Heap is empty. Create a node and assign it to the root */	
	if (NULL_PTR == *pstFiboHeapRoot)
	{
		myLog(DEBUG, "Empty heap!, Vertex: [%d] with distance [%d] "
			"becomes the new root!", vertex, distance);
		*pstFiboHeapRoot = pstFiboNode;
		
		return pstFiboNode;
	}
	
	/* If the heap is not empty! */
	pstFiboNode->next = (*pstFiboHeapRoot)->next;
	(*pstFiboHeapRoot)->next->prev = pstFiboNode;
	(*pstFiboHeapRoot)->next = pstFiboNode;
	pstFiboNode->prev = (*pstFiboHeapRoot);
	
	/* If the key of newly inserted node is < than the old root, update root pointer */
	if (pstFiboNode->key < (*pstFiboHeapRoot)->key)
	{
		myLog(DEBUG, "Root updated from [%d] to [%d]",
			(*pstFiboHeapRoot)->vertex, pstFiboNode->vertex);
		*pstFiboHeapRoot = pstFiboNode;
	}
	
	return pstFiboNode;
}

/* Merges two fibonacci trees */
FiboHeap* fiboMeld(FiboHeap *pstFiboTree1, FiboHeap *pstFiboTree2)
{
	FiboHeap *pstTree1Next = NULL_PTR;
	/* If both are empty, return empty tree */
	if ((NULL_PTR == pstFiboTree1) && (NULL_PTR == pstFiboTree2))
	{
		return NULL_PTR;
	}
	
	/* If one of them is empty, return the other */
	if ((NULL_PTR == pstFiboTree1) && (NULL_PTR != pstFiboTree2))
	{
		return pstFiboTree1;
	}
	
	if ((NULL_PTR != pstFiboTree1) && (NULL_PTR == pstFiboTree2))
	{
		return pstFiboTree2;
	}
	
	myLog(DEBUG, "Melding trees with vertices: [%d] and [%d]",
		pstFiboTree1->vertex, pstFiboTree2->vertex);
	
	/* Merge the top level circular linked list and meld is done! */
	pstTree1Next = pstFiboTree1->next;
	
	pstFiboTree1->next = pstFiboTree2->next;
	pstFiboTree2->next->prev = pstFiboTree1;
	
	pstFiboTree2->next = pstTree1Next;
	pstTree1Next->prev = pstFiboTree2;
	
	//printFiboHeap(pstFiboTree1->key < pstFiboTree2->key ? pstFiboTree1 : pstFiboTree2);
	
	/* Return the lowest key, the new root */
	return (pstFiboTree1->key < pstFiboTree2->key ? pstFiboTree1 : pstFiboTree2);
}

/* Useful in pairwise combine */
FiboHeap* fiboCombine(FiboHeap *pstFiboTree1, FiboHeap *pstFiboTree2)
{
	FiboHeap **pstNewRoot = NULL_PTR;
	FiboHeap **pstNewChild = NULL_PTR;
	FiboHeap *pstChildPrev;
	
	/* If any of the trees are empty, return empty tree, failure condition */
	if ((NULL_PTR == pstFiboTree1) || (NULL_PTR == pstFiboTree2))
	{
		myLog(ERROR, "Invalid Input!");
		return NULL_PTR;
	}
	
	myLog(DEBUG, "Combining trees with vertices: [%d] and [%d]",
		pstFiboTree1->vertex, pstFiboTree2->vertex);
	
	/* The keys of both the trees should be same */
	if (pstFiboTree1->degree != pstFiboTree2->degree)
	{
		myLog(ERROR, "FiboTree degrees don't match. [%d] and [%d]!",
			pstFiboTree1->degree, pstFiboTree2->degree);
		return NULL_PTR;
	}
	
	/* Determine who would be the new root */
	if (pstFiboTree1->key < pstFiboTree2->key)
	{
		pstNewRoot = &pstFiboTree1;
		pstNewChild = &pstFiboTree2;
	}
	else
	{
		pstNewRoot = &pstFiboTree2;
		pstNewChild = &pstFiboTree1;
	}
	
	/* Print Heap */
	//printFiboHeap((*pstNewRoot));
	
	/* As they were siblings before, break their sibling ties */	
	pstChildPrev = (*pstNewChild)->prev;
		
	pstChildPrev->next = (*pstNewChild)->next;
	(*pstNewChild)->next->prev = pstChildPrev;
	
	/* The child should act solo */
	(*pstNewChild)->next = (*pstNewChild);
	(*pstNewChild)->prev = (*pstNewChild);
	
	/* After determining, make one tree the child of other.
		Increase the degree of the new root and childcut of the child
		becomes FALSE now */
	(*pstNewRoot)->degree = (*pstNewRoot)->degree + 1;
	(*pstNewChild)->childCut = FALSE;
	
	/* If this was the first child that was inserted, then child pointer would be NULL */
	if (1 == (*pstNewRoot)->degree)
	{
		(*pstNewRoot)->child = (*pstNewChild);
		(*pstNewChild)->parent = (*pstNewRoot);
	}
	else
	{
		/* Add it to the circular linked-list of root's prev children */
		(*pstNewChild)->next = (*pstNewRoot)->child->next;
		(*pstNewRoot)->child->next->prev = (*pstNewChild);
		
		(*pstNewRoot)->child->next = (*pstNewChild);
		(*pstNewChild)->prev = (*pstNewRoot)->child;
		
		(*pstNewChild)->parent = NULL_PTR;
	}
	
	myLog(DEBUG, "After combining, the new root is with vertex: [%d] "
		"and degree: [%d]", (*pstNewRoot)->vertex, (*pstNewRoot)->degree);
		
	/* Print Heap */
	//printFiboHeap((*pstNewRoot));
	
	return (*pstNewRoot);
}

/* Returns the root data and do pairwise combine of the rest trees */
int fiboRemoveMin(FiboHeap **pstFiboHeap, int *vertex, int *distance)
{
	FiboHeap *pstRmvdTreeChild = NULL_PTR, *pstTraverser = NULL_PTR;
	FiboHeap *pstNewRoot = NULL_PTR, *pstStartNode = NULL_PTR;

	if ((NULL_PTR == pstFiboHeap) || (NULL_PTR == (*pstFiboHeap)))
	{
		myLog(ERROR, "Invalid Input! [%p] and [%p]",
			pstFiboHeap, (*pstFiboHeap));
		return ERR;
	}
	
	/* Give the output to the user */
	*distance = (*pstFiboHeap)->key;
	*vertex = (*pstFiboHeap)->vertex;
	
	//printCircularList((*pstFiboHeap));
	
	myLog(DEBUG, "Removing min. vertex: [%d] and distance: [%d]", *vertex, *distance);
	
	/* If root is the only node present */
	if ((*pstFiboHeap)->next == (*pstFiboHeap) && (0 == (*pstFiboHeap)->degree))
	{
		myLog(DEBUG, "Root is the only node present, heap becomes empty now!");
		free((*pstFiboHeap));
		(*pstFiboHeap) = NULL_PTR;
		
		return OK;
	}
	
	/* If there is ONLY ONE ROOT, but WITH childern,
		then the new root is either of the child
		The && condition is implicit */
	if ((*pstFiboHeap)->next == (*pstFiboHeap)) //&& 0 != (*pstFiboHeap)->degree
	{
		myLog(DEBUG, "A single root with vertex: [%d] and degree: [%d] is present "
			"in the heap", (*pstFiboHeap)->vertex, (*pstFiboHeap)->degree);
		
		/* Points to the first child of the tree whose root is removed */
		pstRmvdTreeChild = (*pstFiboHeap)->child;
		/* Detach child from its parent. The parent still points to the child */
		pstRmvdTreeChild->parent = NULL_PTR;
		
		/* Search the new root */
		pstNewRoot = pstRmvdTreeChild;

		do
		{
			if (pstRmvdTreeChild->key < pstNewRoot->key)
			{
				pstNewRoot = pstRmvdTreeChild;
			}
			
			pstRmvdTreeChild = pstRmvdTreeChild->next;
		}
		while(pstRmvdTreeChild != (*pstFiboHeap)->child);
		
		/* Free the node and update the new root */
		free((*pstFiboHeap));
		(*pstFiboHeap) = pstNewRoot;
		
		return OK;
	}
	
	myLog(DEBUG, "More than 1 node is present at the top level. Root "
			"with vertex: [%d] has [%d] children",
			(*pstFiboHeap)->vertex, (*pstFiboHeap)->degree);
	
	/* Just like deleting a node from linked list */
	/* Search the new root and also find the node prev to current root*/
	pstStartNode = (*pstFiboHeap)->prev;
	pstNewRoot = pstStartNode;

	/* Start node points to the prev node of root */
	pstStartNode->next = (*pstFiboHeap)->next;
	(*pstFiboHeap)->next->prev = pstStartNode;
	
	//printCircularList(pstStartNode);
	//printFiboHeap(pstStartNode);
	
	/* The top level has more than 1 tree but the current root has children */
	if (0 != (*pstFiboHeap)->degree)
	{
		/* Points to the first child of the tree whose root is removed */
		pstRmvdTreeChild = (*pstFiboHeap)->child;
		/* Detach child from its parent. The parent still points to the child */
		pstRmvdTreeChild->parent = NULL_PTR;
		(*pstFiboHeap)->child = NULL_PTR;
		
		(void)fiboMeld((*pstFiboHeap)->next, pstRmvdTreeChild);
	}
	
	/* Pairwise combine all trees whose degree is same */
	pstTraverser = (*pstFiboHeap)->next;
	pstStartNode = (*pstFiboHeap)->next;
	
	(void)memset(fiboPtrFlags, NULL_PTR, sizeof(fiboPtrFlags));
	
	do
	{
		if (pstTraverser == pstTraverser->next)
		{
			break;
		}
		
		myLog(DEBUG, "Scanning vertex: [%d], distance: [%d] and degree: [%d]"
			" Root vertex: [%d], distance: [%d]",
			pstTraverser->vertex, pstTraverser->key, pstTraverser->degree,
			pstNewRoot->vertex, pstNewRoot->key);

		while (NULL_PTR != fiboPtrFlags[pstTraverser->degree])
		{
			/* No need to combine if the vertices are same */
			if (pstTraverser->vertex ==
				((FiboHeap *)(fiboPtrFlags[pstTraverser->degree]))->vertex)
			{
				break;
			}
			
			/*
			myLog(DEBUG, "Combining vertex: [%d] and vertex: [%d] with key "
				"[%d] and [%d] respectively",
				pstTraverser->vertex,
				((FiboHeap *)(fiboPtrFlags[pstTraverser->degree]))->vertex,
				pstTraverser->key,
				((FiboHeap *)(fiboPtrFlags[pstTraverser->degree]))->key);*/
				
			//printCircularList(pstTraverser);
				
			/*Combine the two trees. The traverser degree increases by 1 */
			pstTraverser = fiboCombine(pstTraverser,
				(FiboHeap *)(fiboPtrFlags[pstTraverser->degree]));
			if (NULL_PTR == pstTraverser)
			{
				myLog(ERROR, "fiboCombine failed!");
				return ERR;
			}
			
			pstNewRoot = pstTraverser;
				
			myLog(DEBUG, "Combined root vertex: [%d]", pstTraverser->vertex);
			
			/* Update the start node. it should be the previous of traverser */
			pstStartNode = pstTraverser;
				
			//printCircularList(pstTraverser);
			
			//myLog(DEBUG, "Degree: [%d]", pstTraverser->degree);
			
			/* Delete the old entry from the table */
			fiboPtrFlags[(pstTraverser->degree - 1)] = NULL_PTR;
		}
		
		fiboPtrFlags[pstTraverser->degree] = (int *)pstTraverser;
		
		pstTraverser = pstTraverser->next;
	}
	while (pstTraverser != pstStartNode);
	
	/* Update the root */
	pstStartNode = pstNewRoot;
	pstTraverser = pstStartNode->next;
	
	while (pstTraverser != pstStartNode)
	{
		if (pstTraverser->key < pstNewRoot->key)
		{
			myLog(DEBUG, "Old Root Vertex: [%d] and distance: [%d]. "
				"Updated Root Vertex: [%d] and distance: [%d]",
				pstNewRoot->vertex, pstNewRoot->key,
				pstTraverser->vertex, pstTraverser->key);
			pstNewRoot = pstTraverser;
		}
		
		pstTraverser = pstTraverser->next;
	}
	
	/*
	myLog(DEBUG, "Freeing old root address: [%x], vertex: [%d], distance: [%d] and degree: [%d]",
		(*pstFiboHeap), (*pstFiboHeap)->vertex, (*pstFiboHeap)->key, (*pstFiboHeap)->degree);*/
	
	/* Free the node and update the new root */
	free((*pstFiboHeap));
	(*pstFiboHeap) = pstNewRoot;
	
	myLog(DEBUG, "Final Root Vertex: [%d]", (*pstFiboHeap)->vertex);
	
	/*
	myLog(DEBUG, "New root address: [%x] with vertex: [%d], distance: [%d] and degree: [%d]",
		(*pstFiboHeap), (*pstFiboHeap)->vertex, (*pstFiboHeap)->key, (*pstFiboHeap)->degree);*/

	return OK;
}

/* Returs the parent of an arbitary node */
FiboHeap *fiboFindParent(FiboHeap *pstNode)
{
	FiboHeap *pstParent = NULL_PTR, *pstTraverser;
	
	if (NULL_PTR == pstNode)
	{
		myLog(ERROR, "Invalid Input!");
		return NULL_PTR;
	}
	
	//myLog(DEBUG, "Finding parent for vertex: [%d]", pstNode->vertex);
	
	if (NULL_PTR == pstNode->parent)
	{
		pstTraverser = pstNode->next;
		while (pstTraverser != pstNode)
		{
			if (NULL_PTR != pstTraverser->parent)
			{
				//myLog(DEBUG, "Parent found!: [%d]", pstTraverser->parent->vertex);
				return (pstTraverser->parent);
			}
			
			pstTraverser = pstTraverser->next;
		}
	}
	else
	{
		//myLog(DEBUG, "Parent found!: [%d]", pstNode->parent->vertex);
		return (pstNode->parent);
	}
	
	/* Parent was not found */
	//myLog(DEBUG, "Parent not found for vertex: [%d]!", pstNode->vertex);
	return NULL_PTR;
}

/* Removes the subtree of a randomly pointed vertex in fibonacci tree and melds it to the
	upper doubly circular linked list */
int fiboDecreaseKey(FiboHeap **pstFiboHeap, FiboHeap *pstNode, int newKey)
{
	FiboHeap *pstParent, *pstTraverser;
	FiboHeap *pstNewRoot = NULL_PTR, *pstTmp;
	
	if ((NULL_PTR == pstNode) || (NULL_PTR == (*pstFiboHeap)))
	{
		myLog(ERROR, "Invalid Input! Node: [%p] and Root: [%p]",
			pstNode, (*pstFiboHeap));
		return ERR;
	}
	
	/* Decrease the key, and swap with the parent if needed */
	pstNode->key = newKey;
	
	while (TRUE)
	{	
		/* Find parent */
		pstParent = fiboFindParent(pstNode);
		if (NULL_PTR == pstParent)
		{
			/* The node might be at top level. So parent is not present (probably a B0 tree) */
			myLog(DEBUG, "Parent not found! Do nothing.");
			return OK;
		}
	
		/* Compare key, if  childKey >= parentKey; everything is fine */
		if (pstNode->key >= pstParent->key)
		{
			/* No need to swap if decrease key did not cause any harm */
			myLog(DEBUG, "Key greater than its parent! No need to remove the tree");
			return OK;
		}
		
		myLog(DEBUG, "Parent: [%d] degree: [%d]", pstParent->vertex, pstParent->degree);
		
		/* Perform cascading cut */
		pstNode->parent = NULL_PTR;
		pstParent->degree = (pstParent->degree - 1);
		
		if (0 == pstParent->degree)
		{
			pstParent->child = NULL_PTR;
		}
		else if (pstParent->degree > 0)
		{
			/* If parent lost the child to which it was pointing,
				update to a new child */
			if (pstNode == pstParent->child)
			{
				pstParent->child = pstNode->prev;
				pstParent->child->parent = pstParent;
			}
		}
		else
		{
			myLog(ERROR, "Degree: [%d] cannot be less than 0", pstParent->degree);
			return ERR;
		}
		
		/* Break ties of the current node */
		pstTmp = pstNode->prev;
		pstNode->prev->next = pstNode->next;
		pstNode->next->prev = pstTmp;
		
		pstNode->next = pstNode;
		pstNode->prev = pstNode;
		
		//printFiboHeap((*pstFiboHeap));
		//printFiboHeap(pstNode);
		
		/* Meld it with the upper layer */
		(void)fiboMeld((*pstFiboHeap), pstNode);
		
		if (FALSE == pstParent->childCut)
		{
			pstParent->childCut = TRUE;
			break;
		}
		
		pstNode = pstParent;
	}
	
	/* Update the root if required */
	pstTraverser = (*pstFiboHeap)->next;
	pstNewRoot = (*pstFiboHeap);
	
	while (pstTraverser != (*pstFiboHeap))
	{
		if (pstTraverser->key < pstNewRoot->key)
		{
			pstNewRoot = pstTraverser;
		}
		pstTraverser = pstTraverser->next;
	}
	
	(*pstFiboHeap) = pstNewRoot;
	
	return OK;
}

/* Checks if the fibonacci heap is empty or not */
int isFiboHeapEmpty(int **nodeInFiboHeap)
{
	int i, isPresent = FALSE;
	
	for (i = 0; i < gNoOfVertex; i++)
	{
		//myLog(DEBUG, "[%d %d] ", i, (!!(nodeInFiboHeap[i])));
		/* If any of the nodes are present, isPresent becomes NON-ZERO */
		isPresent  = (isPresent | (!!(nodeInFiboHeap[i])));
	}
	
	return (!isPresent);
}

/* Deallocates the memory given to the fibonacci heap */
void destroyFiboHeap(FiboHeap *pstFiboHeapRoot, int **nodeInFiboHeap)
{
	int vertex, distance;
	
	while(FALSE == isFiboHeapEmpty(nodeInFiboHeap))
	{
		if (OK != fiboRemoveMin(&pstFiboHeapRoot, &vertex, &distance))
		{
			myLog(ERROR, "fiboRemoveMin failed! Memory might have leaked!");
			break;
		}
		nodeInFiboHeap[vertex] = NULL_PTR;
	}
}

/* Input is a graph with certain no of edges and vertices, output should be the time taken in msec */
int runFiboScheme(AdjList *pstAdjList)
{
	FiboHeap *pstFiboHeap = NULL_PTR, *pstFiboNode = NULL_PTR;
	AdjList *pstListTraverser = NULL_PTR;
	char nodeVisited[MAX_VERTEX_NUM] = {FALSE};
	int *nodeInFiboHeap[MAX_VERTEX_NUM] = {NULL_PTR};
	int sourceVertex, closestVertex, distance, adjVertex, adjDistance;
	int newDistance, hasDistanceChanged = FALSE, isHeapEmpty;
	clock_t startTime, time;
	
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		return INVALID_TIME;
	}

	/* Initialize everthing (data structures) */
	pstFiboHeap = initFibonacciHeap(pstAdjList);
	if (NULL_PTR != pstFiboHeap)
	{
		myLog(ERROR, "initFibonacciHeap failed!");
		return INVALID_TIME;
	}
	
	startTime = clock();
	
	/* We have an empty fibonacci heap now */
	for (sourceVertex = 0; sourceVertex < gNoOfVertex; sourceVertex++)
	{
		/* Init visited flags */
		(void)memset(nodeVisited, FALSE, gNoOfVertex);
		/* Say all node are outside the heap */
		(void)memset(nodeInFiboHeap, NULL_PTR, gNoOfVertex);
		
		gTestEV = 0;
		
		myLog(DEBUG, "******* Root: [%x] *******", pstFiboNode);
		myLog(DEBUG, "Inserting source vertex: [%d] with distance 0", sourceVertex);
		
		/* Insert source vertex into the fibonacci heap */
		pstFiboNode = fiboInsert(&pstFiboHeap, sourceVertex, 0);
		if (NULL_PTR == pstFiboNode)
		{
			myLog(ERROR, "fiboInsert failed!");
			return INVALID_TIME;
		}
		
		gTestEV++;
		
		myLog(DEBUG, "Inserted source vertex: [%d] at [%x]",
			sourceVertex, pstFiboNode);
			
		myLog(DEBUG, "Root: [%x], root vertex: [%d], root distance: [%d]",
			pstFiboNode, pstFiboNode->vertex, pstFiboNode->key);
		
		/* Store the address into a lookup table */
		nodeInFiboHeap[sourceVertex] = (int *)pstFiboNode;
		
		/* Solve and update the dist[] using Dijkstra's algorithm */
		while(FALSE == (isHeapEmpty = isFiboHeapEmpty(nodeInFiboHeap)))
		{
			myLog(DEBUG, "FiboHeap is not empty. Extracting min now");
			
			gTestEV--;
			
			if (OK != fiboRemoveMin(&pstFiboHeap, &closestVertex, &distance))
			{
				myLog(ERROR, "fiboRemoveMin failed!");
				destroyFiboHeap(pstFiboHeap, nodeInFiboHeap);
				return INVALID_TIME;
			}
			
			myLog(DEBUG, "Extracted min. Vertex: [%d] Distance: [%d]",
				closestVertex, distance);
			
			/* As the node was removed from Fibonacci Heap, delete lookup entry */
			nodeInFiboHeap[closestVertex] = (int *)NULL_PTR;
			/* Mark the node as visited */
			nodeVisited[closestVertex] = TRUE;
			
			myLog(DEBUG, "Scanning the adjacent nodes of vertex: [%d]", closestVertex);
			
			/* Get the adjacent nodes of the removed vertex and relax them
			If not present in FiboHeap, insert() into FiboHeap or
			else perform decreaseKey() (if needed) */
			pstListTraverser = pstAdjList[closestVertex].next;
			while (NULL_PTR != pstListTraverser)
			{
				myLog(DEBUG, "[%d] is adjacent to [%d] at distance: [%d]",
					pstListTraverser->vertexNum, closestVertex,
					pstListTraverser->distance);
				
				/* Relax edges */
				if (TRUE == nodeVisited[pstListTraverser->vertexNum])
				{
					pstListTraverser = pstListTraverser->next;
					continue;
				}
				
				hasDistanceChanged = FALSE;
				newDistance = dist[closestVertex][pstListTraverser->vertexNum]
						+ dist[sourceVertex][closestVertex];
				
				myLog(DEBUG, "New distance between: [%d] and [%d] via [%d] "
					"is [%d]. Old distance: [%d]",
					sourceVertex, pstListTraverser->vertexNum,
					closestVertex, newDistance,
					dist[sourceVertex][pstListTraverser->vertexNum]);
				
				if (dist[sourceVertex][pstListTraverser->vertexNum] > newDistance)
				{
					dist[sourceVertex][pstListTraverser->vertexNum] = newDistance;
					hasDistanceChanged = TRUE;
				}
				else
				{
					newDistance = dist[sourceVertex][pstListTraverser->vertexNum];
				}
				
				myLog(DEBUG, "Checking if vertex [%d] is present in heap",
					pstListTraverser->vertexNum);
				
				/* Check if the node is already present in the fibonacci heap */
				pstFiboNode = (FiboHeap *)nodeInFiboHeap[pstListTraverser->vertexNum];
				if (NULL_PTR == pstFiboNode)
				{
					myLog(DEBUG, "Vertex [%d] is not present in heap. Insert "
						"into heap with distance: [%d]",
						pstListTraverser->vertexNum, newDistance);

					pstFiboNode = fiboInsert(&pstFiboHeap,
							pstListTraverser->vertexNum, newDistance);
					if (NULL_PTR == pstFiboNode)
					{
						destroyFiboHeap(pstFiboHeap, nodeInFiboHeap);
						myLog(ERROR, "fiboInsert failed!");
						return INVALID_TIME;
					}
					
					gTestEV++;
					
					myLog(DEBUG, "Inserted vertex: [%d] at [%x]",
						pstListTraverser->vertexNum, pstFiboNode);
						
					myLog(DEBUG, "Root: [%x], root vertex: [%d], "
						"root distance: [%d]",
						pstFiboHeap, pstFiboHeap->vertex, pstFiboHeap->key);
										
					nodeInFiboHeap[pstListTraverser->vertexNum]
									= (int *)pstFiboNode;
				}
				else
				{
					myLog(DEBUG, "Vertex [%d] is present in heap at "
						"address: [%x] with distance: [%d]. "
						"Distance changed?: [%d]",
						pstFiboNode->vertex, pstFiboNode,
						pstFiboNode->key, hasDistanceChanged);
					/* If the distance has not changed, continue */
					if (FALSE == hasDistanceChanged)
					{
						pstListTraverser = pstListTraverser->next;
						continue;
					}
					
					myLog(DEBUG, "Performing decrease key on vertex: [%d] "
						"distance: [%d] and newDistance: [%d]",
						pstFiboNode->vertex, pstFiboNode->key, newDistance);
					
					if (OK != fiboDecreaseKey(&pstFiboHeap,
						pstFiboNode, newDistance))
					{
						myLog(ERROR, "fiboDecreaseKey failed!");
						destroyFiboHeap(pstFiboHeap, nodeInFiboHeap);
						return INVALID_TIME;
					}
				}
				
				pstListTraverser = pstListTraverser->next;
			}
		}
		
		if (TRUE == isHeapEmpty)
		{
			myLog(DEBUG, "Heap becomes empty! No further calculations needed "
				"for source vertex: [%d]", sourceVertex);
			pstFiboNode = NULL_PTR;
		}
	}
	
	time = ((clock() - startTime) / CLOCKS_PER_MSEC);
	
	myLog(INFO, "Time taken by FiboDijkstra: [%d] msec.", time);
	
	return time;
}
