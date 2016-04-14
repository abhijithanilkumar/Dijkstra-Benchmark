/***************************************************************************************************/
// BINO.C
// Contains all functions for using binomial scheme
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#include "bino.h"
#include <string.h>
#include <time.h>

extern int gNoOfVertex;
extern int dist[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
extern AdjList* initAdjList();

int gTestTV;
int gTestEV;

extern int gBCombine;

/* Pointer Flags. Assuming maximum 500 nodes, the maxdegree of the binomial tree can
go only to maximum of log_2 (500) <= 9 */
int *binoPtrFlags[MAX_BINO_DEGREE] = {NULL_PTR};

/* Entry function for user input */
int binoProc()
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
	if (INVALID_TIME == runBinoScheme(pstAdjList))
	{
		myLog(ERROR, "runBinoScheme failed!");
		return ERR;
	}
	
	/* Print the distance matrix */
	printDistMatrix();

	return OK;
}

/* Entry function for user file input */
int binoFileProc(char *fileName)
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
	if (INVALID_TIME == runBinoScheme(pstAdjList))
	{
		myLog(ERROR, "runBinoScheme failed!");
		return ERR;
	}
	
	/* Print the distance matrix */
	printDistMatrix();
	
	return OK;
}

/* Functions for Binomial Heap */

/* Creates an empty bino heap and initializes distance matrix */
BinoHeap* initBinomialHeap(AdjList *pstAdjList)
{
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		/* return a non-null pointer */
		return (BinoHeap *)(0x1);
	}
	/* Iniitalize every value to infinity, distance to itself is 0 */
	initDistMatrix();
	
	/* Copy from adj list to dist[] and then apply Dijkstra's algo on dist[] */
	if (OK != copyFromAdjListToDist(pstAdjList))
	{
		myLog(ERROR, "copyFromAdjListToDist failed!");
		/* return a non-null pointer */
		return (BinoHeap *)(0x1);
	}

	return NULL_PTR;
}

/* Returns a single init'd BinoHeap Node */
BinoHeap* getBlankBinoNode()
{
	BinoHeap *pstBinoNode = NULL_PTR;
	
	pstBinoNode = (BinoHeap *)malloc (sizeof(BinoHeap));
	if (NULL_PTR == pstBinoNode)
	{
		myLog(ERROR, "Failed to allocate %d bytes!", sizeof(BinoHeap));
		return NULL_PTR;
	}
	
	pstBinoNode->key = INFINITY;
	pstBinoNode->vertex = MAX_VERTEX_NUM;
	pstBinoNode->degree = 0;
	pstBinoNode->child = NULL_PTR;
	pstBinoNode->parent = NULL_PTR;
	pstBinoNode->next = pstBinoNode;
	
	return pstBinoNode;
}

/* Insert a tree into the binomial heap return the pointer to the new node */
BinoHeap* binoInsert(BinoHeap **pstBinoHeapRoot, int vertex, int distance)
{
	BinoHeap *pstBinoNode = NULL_PTR;
	
	/* Do not check for *pstBinoHeapRoot as it can be NULL_PTR */
	if (NULL_PTR == pstBinoHeapRoot)
	{
		myLog(ERROR, "Invalid Input!");
		return NULL_PTR;
	}

	pstBinoNode = getBlankBinoNode();
	if (NULL_PTR == pstBinoNode)
	{
		myLog(ERROR, "getBlankBinoNode failed!");
		return NULL_PTR;
	}
	
	pstBinoNode->key = distance;
	pstBinoNode->vertex = vertex;

	/* Heap is empty. Create a node and assign it to the root */	
	if (NULL_PTR == *pstBinoHeapRoot)
	{
		myLog(DEBUG, "Empty heap!, Vertex: [%d] with distance [%d] "
			"becomes the new root!", vertex, distance);
		*pstBinoHeapRoot = pstBinoNode;
		
		//printCircularList(pstBinoNode);
		return pstBinoNode;
	}
	
	/* If the heap is not empty! */
	pstBinoNode->next = (*pstBinoHeapRoot)->next;
	(*pstBinoHeapRoot)->next = pstBinoNode;
	
	/* If the key of newly inserted node is < than the old root, update root pointer */
	if (pstBinoNode->key < (*pstBinoHeapRoot)->key)
	{
		myLog(DEBUG, "Root updated from [%d] to [%d]",
			(*pstBinoHeapRoot)->vertex, pstBinoNode->vertex);
		*pstBinoHeapRoot = pstBinoNode;
	}
	
	//printCircularList((*pstBinoHeapRoot));
	
	return pstBinoNode;
}

/* Merges two binomial trees */
BinoHeap* binoMeld(BinoHeap *pstBinoTree1, BinoHeap *pstBinoTree2)
{
	BinoHeap *pstTree1Next = NULL_PTR;
	/* If both are empty, return empty tree */
	if ((NULL_PTR == pstBinoTree1) && (NULL_PTR == pstBinoTree2))
	{
		return NULL_PTR;
	}
	
	/* If one of them is empty, return the other */
	if ((NULL_PTR == pstBinoTree1) && (NULL_PTR != pstBinoTree2))
	{
		return pstBinoTree1;
	}
	
	if ((NULL_PTR != pstBinoTree1) && (NULL_PTR == pstBinoTree2))
	{
		return pstBinoTree2;
	}
	
	myLog(DEBUG, "Melding trees with vertices: [%d] and [%d]",
		pstBinoTree1->vertex, pstBinoTree2->vertex);
	
	/* Merge the top level circular linked list and merge is done! */
	pstTree1Next = pstBinoTree1->next;
	pstBinoTree1->next = pstBinoTree2->next;
	pstBinoTree2->next = pstTree1Next;
	
	/* Return the lowest key, the new root */
	return (pstBinoTree1->key < pstBinoTree2->key ? pstBinoTree1 : pstBinoTree2);
}

/* Useful in pairwise combine */
BinoHeap* binoCombine(BinoHeap *pstBinoTree1, BinoHeap *pstBinoTree2)
{
	BinoHeap **pstNewRoot = NULL_PTR;
	BinoHeap **pstNewChild = NULL_PTR;
	BinoHeap *pstChildPrev;
	
	/* If any of the trees are empty, return empty tree, failure condition */
	if ((NULL_PTR == pstBinoTree1) || (NULL_PTR == pstBinoTree2))
	{
		myLog(ERROR, "Invalid Input!");
		return NULL_PTR;
	}
	
	myLog(DEBUG, "Combining trees with vertices: [%d] and [%d]",
		pstBinoTree1->vertex, pstBinoTree2->vertex);
	
	/* The keys of both the trees should be same */
	if (pstBinoTree1->degree != pstBinoTree2->degree)
	{
		myLog(ERROR, "BinoTree degrees don't match. [%d] and [%d]!",
			pstBinoTree1->degree, pstBinoTree2->degree);
		return NULL_PTR;
	}
	
	/* Determine who would be the new root */
	if (pstBinoTree1->key < pstBinoTree2->key)
	{
		pstNewRoot = &pstBinoTree1;
		pstNewChild = &pstBinoTree2;
	}
	else
	{
		pstNewRoot = &pstBinoTree2;
		pstNewChild = &pstBinoTree1;
	}
	
	/* Print Heap */
	printBinoHeap((*pstNewRoot));
	
	/* As they were siblings before, break their sibling ties */	
	pstChildPrev = (*pstNewChild);
	while (pstChildPrev->next != (*pstNewChild))
	{
		pstChildPrev = pstChildPrev->next;
	}
	
	pstChildPrev->next = (*pstNewChild)->next;
	(*pstNewChild)->next = (*pstNewChild);
	
	/* After determining, make one tree the child of other.
		Increase the degree of the new root */
	(*pstNewRoot)->degree = (*pstNewRoot)->degree + 1;
	
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
		(*pstNewRoot)->child->next = (*pstNewChild);
		(*pstNewChild)->parent = NULL_PTR;
	}
	
	myLog(DEBUG, "After combining, the new root is with vertex: [%d] "
		"and degree: [%d]", (*pstNewRoot)->vertex, (*pstNewRoot)->degree);
		
	/* Print Heap */
	printBinoHeap((*pstNewRoot));
	
	return (*pstNewRoot);
}

/* Prints the addresses of the nodes which are waiting to be combined */
void printAddrList()
{
	int i;
	
	for (i = 0; i < MAX_BINO_DEGREE; i++)
	{
		printf("[%d: %p] ", i, binoPtrFlags[i]);
	}
	printf("\n");
}

/* Returns the previous pointer to the current node */
BinoHeap* findPrevNode(BinoHeap *pstNode)
{
	BinoHeap *pstPrevNode = NULL_PTR;
	
	pstPrevNode = pstNode;
	
	while (pstPrevNode->next != pstNode)
	{
		pstPrevNode = pstPrevNode->next;
	}
	
	return pstPrevNode;
}

/* Returns the root data and do pairwise combine of the rest trees */
int binoRemoveMin(BinoHeap **pstBinoHeap, int *vertex, int *distance)
{
	BinoHeap *pstRmvdTreeChild = NULL_PTR, *pstTraverser = NULL_PTR;
	BinoHeap *pstNewRoot = NULL_PTR, *pstStartNode = NULL_PTR;

	if ((NULL_PTR == pstBinoHeap) || (NULL_PTR == (*pstBinoHeap)))
	{
		myLog(ERROR, "Invalid Input! [%p] and [%p]",
			pstBinoHeap, (*pstBinoHeap));
		return ERR;
	}
	
	/* Give the output to the user */
	*distance = (*pstBinoHeap)->key;
	*vertex = (*pstBinoHeap)->vertex;
	
	//printCircularList((*pstBinoHeap));
	
	myLog(DEBUG, "Removing min. vertex: [%d] and distance: [%d]", *vertex, *distance);
	
	/* If root is the only node present */
	if ((*pstBinoHeap)->next == (*pstBinoHeap) && (0 == (*pstBinoHeap)->degree))
	{
		myLog(DEBUG, "Root is the only node present, heap becomes empty now!");
		free((*pstBinoHeap));
		(*pstBinoHeap) = NULL_PTR;
		
		return OK;
	}
	
	/* If there is ONLY ONE ROOT, but WITH childern,
		then the new root is either of the child
		The && condition is implicit */
	if ((*pstBinoHeap)->next == (*pstBinoHeap)) //&& 0 != (*pstBinoHeap)->degree
	{
		myLog(DEBUG, "A single root with vertex: [%d] and degree: [%d] is present "
			"in the heap", (*pstBinoHeap)->vertex, (*pstBinoHeap)->degree);
		
		/* Points to the first child of the tree whose root is removed */
		pstRmvdTreeChild = (*pstBinoHeap)->child;
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
		while(pstRmvdTreeChild != (*pstBinoHeap)->child);
		
		/* Free the node and update the new root */
		free((*pstBinoHeap));
		(*pstBinoHeap) = pstNewRoot;
		
		//printCircularList((*pstBinoHeap));
		
		return OK;
	}
	
	myLog(DEBUG, "More than 1 node is present at the top level. Root "
			"with vertex: [%d] has [%d] children",
			(*pstBinoHeap)->vertex, (*pstBinoHeap)->degree);
	
	/* Just like deleting a node from linked list */
	/* Search the new root and also find the node prev to current root*/
	pstStartNode = (*pstBinoHeap);
	pstNewRoot = (*pstBinoHeap)->next;
	
	//printCircularList(pstStartNode);
	//printBinoHeap((*pstBinoHeap));
	
	while (pstStartNode->next != (*pstBinoHeap))
	{
		pstStartNode = pstStartNode->next;
		
		/* Update the root if required */
		if (pstStartNode->key < pstNewRoot->key)
		{
			pstNewRoot = pstStartNode;
		}
	}
	
	//printCircularList(pstStartNode);
	
	/* Start node points to the prev node of root */
	pstStartNode->next = (*pstBinoHeap)->next;
	
	//printCircularList(pstStartNode);
	//printBinoHeap((*pstBinoHeap));
	
	/* The top level has more than 1 tree but the current root has children */
	if (0 != (*pstBinoHeap)->degree)
	{
		/* Points to the first child of the tree whose root is removed */
		pstRmvdTreeChild = (*pstBinoHeap)->child;
		/* Detach child from its parent. The parent still points to the child */
		pstRmvdTreeChild->parent = NULL_PTR;
		(*pstBinoHeap)->child = NULL_PTR;
		
		(void)binoMeld((*pstBinoHeap)->next, pstRmvdTreeChild);
	}
	
	/* Pairwise combine all trees whose degree is same */
	pstTraverser = (*pstBinoHeap)->next;
	pstStartNode = (*pstBinoHeap)->next;
	
	(void)memset(binoPtrFlags, NULL_PTR, sizeof(binoPtrFlags));
	
	do
	{
		if (pstTraverser == pstTraverser->next)
		{
			break;
		}

		while (NULL_PTR != binoPtrFlags[pstTraverser->degree])
		{
			/* No need to combine if the vertices are same */
			if (pstTraverser->vertex ==
				((BinoHeap *)(binoPtrFlags[pstTraverser->degree]))->vertex)
			{
				break;
			}
			
			/*
			myLog(DEBUG, "Combining vertex: [%d] and vertex: [%d] with key "
				"[%d] and [%d] respectively",
				pstTraverser->vertex,
				((BinoHeap *)(binoPtrFlags[pstTraverser->degree]))->vertex,
				pstTraverser->key,
				((BinoHeap *)(binoPtrFlags[pstTraverser->degree]))->key);*/
				
			//printCircularList(pstTraverser);
			//printAddrList();
				
			/*Combine the two trees. The traverser degree increases by 1 */
			pstTraverser = binoCombine(pstTraverser,
				(BinoHeap *)(binoPtrFlags[pstTraverser->degree]));
			if (NULL_PTR == pstTraverser)
			{
				myLog(ERROR, "binoCombine failed!");
				return ERR;
			}
				
			myLog(DEBUG, "Combined root vertex: [%d]", pstTraverser->vertex);
			
			/* Update the start node. it should be the previous of traverser */
			pstStartNode = pstTraverser;
				
			//printCircularList(pstTraverser);
			//printAddrList();
			
			/* Update the root if required */
			if (pstTraverser->key <= pstNewRoot->key)
			{
				pstNewRoot = pstTraverser;
				myLog(DEBUG, "Updated Root Vertex: [%d]", pstNewRoot->vertex);
			}
			
			//myLog(DEBUG, "Degree: [%d]", pstTraverser->degree);
			//printAddrList();
			
			/* Delete the old entry from the table */
			binoPtrFlags[(pstTraverser->degree - 1)] = NULL_PTR;
		}
		
		binoPtrFlags[pstTraverser->degree] = (int *)pstTraverser;
		
		/* Update the root if required */
		if (pstTraverser->key <= pstNewRoot->key)
		{
			pstNewRoot = pstTraverser;
			myLog(DEBUG, "Updated Root Vertex: [%d]", pstNewRoot->vertex);
		}
		
		pstTraverser = pstTraverser->next;
	}
	while (pstTraverser != pstStartNode);
	
	/*
	myLog(DEBUG, "Freeing old root address: [%x], vertex: [%d], distance: [%d] and degree: [%d]",
		(*pstBinoHeap), (*pstBinoHeap)->vertex, (*pstBinoHeap)->key, (*pstBinoHeap)->degree);*/
	
	/* Free the node and update the new root */
	free((*pstBinoHeap));
	(*pstBinoHeap) = pstNewRoot;
	
	/*
	myLog(DEBUG, "New root address: [%x] with vertex: [%d], distance: [%d] and degree: [%d]",
		(*pstBinoHeap), (*pstBinoHeap)->vertex, (*pstBinoHeap)->key, (*pstBinoHeap)->degree);*/

	return OK;
}

/* Returs the parent of an arbitary node */
BinoHeap *binoFindParent(BinoHeap *pstNode)
{
	BinoHeap *pstParent = NULL_PTR, *pstTraverser;
	
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

/* Decreases the key of a randomly pointed vertex in binomial tree */
int binoDecreaseKey(BinoHeap **pstBinoHeap, BinoHeap *pstNode, int newDistance)
{
	BinoHeap *pstParent = NULL_PTR, *pstGrandParent = NULL_PTR, *pstParentPrev = NULL_PTR;
	BinoHeap *pstTraverser = NULL_PTR, *pstChildPrev = NULL_PTR;
	BinoHeap *pstTmp = NULL_PTR;
	BinoHeap stParentBkup = {0};
	int tmp;
	
	if (NULL_PTR == pstNode)
	{
		myLog(ERROR, "Invalid Input!");
		return ERR;
	}
	
	/* Decrease the key, and swap with the parent if needed */
	pstNode->key = newDistance;
	
	while (TRUE)
	{	
		/* Find parent */
		pstParent = binoFindParent(pstNode);
		if (NULL_PTR == pstParent)
		{
			/* The node might be at top level. So parent is not present (probably a B0 tree) */
			myLog(DEBUG, "Parent not found! No need to swap with parent.");
			return OK;
		}
	
		/* Compare key, if  childKey >= parentKey; everything is fine */
		if (pstNode->key >= pstParent->key)
		{
			/* No need to swap if decrease key did not cause any harm */
			myLog(DEBUG, "Key greater than its parent! No need to swap with parent.");
			return OK;
		}
		
		/* Update the root if needed */
		if ((*pstBinoHeap) == pstParent)
		{
			/*myLog(DEBUG, "Root updated from [%d] to [%d]",
				pstParent->vertex, pstNode->vertex);*/
			(*pstBinoHeap) = pstNode;
		}
		
		/* Swap the degrees of both parent and child! */
		tmp = pstParent->degree;
		pstParent->degree = pstNode->degree;
		pstNode->degree = tmp;
	
		/* childKey < parentKey; swap nodes */
		/* Point the child pointer of parent to the child which is to be swapped */	
		/* Take backup of the parent node */
		(void)memcpy(&stParentBkup, pstParent, sizeof(stParentBkup));
		
		/*
		myLog(DEBUG, "Swapping child and parent. Parent vertex: [%d] "
			"and child vertex: [%d]", pstParent->vertex, pstNode->vertex);*/
	
		/* If a B1 tree exists */
		if ((pstParent->next == pstParent) && (pstNode->next == pstNode))
		{
			myLog(DEBUG, "Parent solo, child solo");
			
			/* The parent can never have a parent */
			pstParent->parent = pstNode;
			pstNode->child = pstParent;
			pstParent->child = NULL_PTR;
			pstNode->parent = NULL_PTR;
		}		
		/* If parent acts solo and child has siblings */
		else if ((pstParent->next == pstParent) && (pstNode->next != pstNode))
		{
			myLog(DEBUG, "Parent solo, child with siblings");
			
			/* Search the previous node of the child */
			pstChildPrev = pstNode;
			while (pstChildPrev->next != pstNode)
			{
				pstChildPrev = pstChildPrev->next;
			}
		
			pstParent->child->parent = NULL_PTR;
		
			/* If child also has a child,
				the parent of the grandchild is the current parent */
			if (NULL_PTR != pstNode->child)
			{
				pstNode->child->parent = pstParent;
			}
		
			/* Swap the parent and the child */
			pstChildPrev->next = pstParent;
			pstParent->next = pstNode->next;
			pstParent->child = pstNode->child;
			pstParent->parent = NULL_PTR;
		
			pstNode->next = pstNode;
			pstNode->parent = NULL_PTR;
			pstNode->child = pstChildPrev;
			pstChildPrev->parent = pstNode;
		}	
		/* If the parent has siblings and the child acts solo */
		else if ((pstParent->next != pstParent) && (pstNode->next == pstNode))
		{
			myLog(DEBUG, "Parent with siblings, child solo");
			/* Search the previous node of parent */
			pstParentPrev = pstParent;
			while (pstParentPrev->next != pstParent)
			{
				pstParentPrev = pstParentPrev->next;
			}
		
			pstParentPrev->next = pstNode;
			
			pstParent->next = pstParent;
			pstParent->parent = pstNode;
			pstParent->child = NULL_PTR;
			
			pstNode->child = pstParent;
			pstNode->parent = stParentBkup.parent;
			if (NULL_PTR != stParentBkup.parent)
			{
				stParentBkup.parent->child = pstNode;
			}
			pstNode->next = stParentBkup.next;
		}	
		/* If the parent has siblings and the child also has siblings */
		else if ((pstParent->next != pstParent) && (pstNode->next != pstNode))
		{
			myLog(DEBUG, "Both Parent and child have siblings");
			
			/* First remove the details of parent's child */
			if (NULL_PTR != pstParent->child)
			{
				pstParent->child->parent = NULL_PTR;
				pstParent->child = NULL_PTR;				
			}
			
			/* Search the previous node of parent */
			pstParentPrev = pstParent;
			while (pstParentPrev->next != pstParent)
			{
				pstParentPrev = pstParentPrev->next;
			}
		
			/* Search the previous node of the child */
			pstChildPrev = pstNode;
			while (pstChildPrev->next != pstNode)
			{
				pstChildPrev = pstChildPrev->next;
			}
				
			/* Update the child */
			pstChildPrev->next = pstParent;
			pstParent->next = pstNode->next;
			pstParent->parent = pstNode;
			pstParent->child = pstNode->child;
			if (NULL_PTR != pstNode->child)
			{
				pstNode->child->parent = pstParent;
			}
	
			/* Update the parent */
			pstParentPrev->next = pstNode;
			pstNode->next = stParentBkup.next;
			pstNode->parent = stParentBkup.parent;
			if (NULL_PTR != stParentBkup.parent)
			{
				stParentBkup.parent->child = pstNode;
			}
			pstNode->child = pstParent;
		}
		else
		{
			myLog(ERROR, "Invalid condition occurs while decreasing key!");
			return ERR;
		}
		
		/*
		myLog(DEBUG, "Parent: [%d] next: [%d] child: [%d] parent: [%d]",
				pstNode->vertex,
				pstNode->next ? pstNode->next->vertex : (501),
				pstNode->child ? pstNode->child->vertex : (501),
				pstNode->parent ? pstNode->parent->vertex : (501));
		myLog(DEBUG, "Child: [%d] next: [%d] child: [%d] parent: [%d]",
				pstParent->vertex,
				pstParent->next ? pstParent->next->vertex : (501),
				pstParent->child ? pstParent->child->vertex : (501),
				pstParent->parent ? pstParent->parent->vertex : (501));
				*/
	}
}

/* Checks if the binomial heap is empty or not */
int isBinoHeapEmpty(int **nodeInBinoHeap)
{
	int i, isPresent = FALSE;
	
	for (i = 0; i < gNoOfVertex; i++)
	{
		//myLog(DEBUG, "[%d %d] ", i, (!!(nodeInBinoHeap[i])));
		/* If any of the nodes are present, isPresent becomes NON-ZERO */
		isPresent  = (isPresent | (!!(nodeInBinoHeap[i])));
	}
	
	return (!isPresent);
}

/* Deallocates the memory given to the binomial heap */
void destroyBinoHeap(BinoHeap *pstBinoHeapRoot, int **nodeInBinoHeap)
{
	int vertex, distance;
	
	while(FALSE == isBinoHeapEmpty(nodeInBinoHeap))
	{
		if (OK != binoRemoveMin(&pstBinoHeapRoot, &vertex, &distance))
		{
			myLog(ERROR, "binoRemoveMin failed! Memory might have leaked!");
			break;
		}
		nodeInBinoHeap[vertex] = NULL_PTR;
	}
}

/* Input is a graph with certain no of edges and vertices, output should be the time taken in msec */
int runBinoScheme(AdjList *pstAdjList)
{
	BinoHeap *pstBinoHeap = NULL_PTR, *pstBinoNode = NULL_PTR;
	AdjList *pstListTraverser = NULL_PTR;
	char nodeVisited[MAX_VERTEX_NUM] = {FALSE};
	int *nodeInBinoHeap[MAX_VERTEX_NUM] = {NULL_PTR};
	int sourceVertex, closestVertex, distance, adjVertex, adjDistance;
	int newDistance, hasDistanceChanged = FALSE, isHeapEmpty;
	clock_t startTime, time;
	
	if (NULL_PTR == pstAdjList)
	{
		myLog(ERROR, "Invalid Input!");
		return INVALID_TIME;
	}

	/* Initialize everthing (data structures) */
	pstBinoHeap = initBinomialHeap(pstAdjList);
	if (NULL_PTR != pstBinoHeap)
	{
		myLog(ERROR, "initBinomialHeap failed!");
		return INVALID_TIME;
	}
	
	startTime = clock();
	
	/* We have an empty binomial heap now */
	for (sourceVertex = 0; sourceVertex < gNoOfVertex; sourceVertex++)
	{
		/* Init visited flags */
		(void)memset(nodeVisited, FALSE, gNoOfVertex);
		/* Say all node are outside the heap */
		(void)memset(nodeInBinoHeap, NULL_PTR, gNoOfVertex);
		
		gTestEV = 0;
		
		//myLog(DEBUG, "******* Root: [%x] *******", pstBinoNode);
		//myLog(DEBUG, "Inserting source vertex: [%d] with distance 0", sourceVertex);
		
		/* Insert source vertex into the binomial heap */
		pstBinoNode = binoInsert(&pstBinoHeap, sourceVertex, 0);
		if (NULL_PTR == pstBinoNode)
		{
			myLog(ERROR, "binoInsert failed!");
			return INVALID_TIME;
		}
		
		gTestEV++;
		
		//myLog(DEBUG, "Inserted source vertex: [%d] at [%x]",
		//	sourceVertex, pstBinoNode);
			
		//myLog(DEBUG, "Root: [%x], root vertex: [%d], root distance: [%d]",
		//	pstBinoNode, pstBinoNode->vertex, pstBinoNode->key);
		
		/* Store the address into a lookup table */
		nodeInBinoHeap[sourceVertex] = (int *)pstBinoNode;
		
		/* Solve and update the dist[] using Dijkstra's algorithm */
		while(FALSE == (isHeapEmpty = isBinoHeapEmpty(nodeInBinoHeap)))
		{
			//myLog(DEBUG, "BinoHeap is not empty. Extracting min now");
			
			gTestEV--;
			
			if (OK != binoRemoveMin(&pstBinoHeap, &closestVertex, &distance))
			{
				myLog(ERROR, "binoRemoveMin failed!");
				destroyBinoHeap(pstBinoHeap, nodeInBinoHeap);
				return INVALID_TIME;
			}
			
			//myLog(DEBUG, "Extracted min. Vertex: [%d] Distance: [%d]",
			//	closestVertex, distance);
			
			/* As the node was removed from Binomial Heap, delete lookup entry */
			nodeInBinoHeap[closestVertex] = (int *)NULL_PTR;
			/* Mark the node as visited */
			nodeVisited[closestVertex] = TRUE;
			
			//myLog(DEBUG, "Scanning the adjacent nodes of vertex: [%d]", closestVertex);
			
			/* Get the adjacent nodes of the removed vertex and relax them
			If not present in BinoHeap, insert() into BinoHeap or
			else perform decreaseKey() (if needed) */
			pstListTraverser = pstAdjList[closestVertex].next;
			while (NULL_PTR != pstListTraverser)
			{
				//myLog(DEBUG, "[%d] is adjacent to [%d] at distance: [%d]",
				//	pstListTraverser->vertexNum, closestVertex,
				//	pstListTraverser->distance);
				
				/* Relax edges */
				if (TRUE == nodeVisited[pstListTraverser->vertexNum])
				{
					pstListTraverser = pstListTraverser->next;
					continue;
				}
				
				hasDistanceChanged = FALSE;
				newDistance = dist[closestVertex][pstListTraverser->vertexNum]
						+ dist[sourceVertex][closestVertex];
				
				//myLog(DEBUG, "New distance between: [%d] and [%d] via [%d] "
				//	"is [%d]. Old distance: [%d]",
				//	sourceVertex, pstListTraverser->vertexNum,
				//	closestVertex, newDistance,
				//	dist[sourceVertex][pstListTraverser->vertexNum]);
				
				if (dist[sourceVertex][pstListTraverser->vertexNum] > newDistance)
				{
					dist[sourceVertex][pstListTraverser->vertexNum] = newDistance;
					hasDistanceChanged = TRUE;
				}
				else
				{
					newDistance = dist[sourceVertex][pstListTraverser->vertexNum];
				}
				
				//myLog(DEBUG, "Checking if vertex [%d] is present in heap",
				//	pstListTraverser->vertexNum);
				
				/* Check if the node is already present in the binomial heap */
				pstBinoNode = (BinoHeap *)nodeInBinoHeap[pstListTraverser->vertexNum];
				if (NULL_PTR == pstBinoNode)
				{
					//myLog(DEBUG, "Vertex [%d] is not present in heap. Insert "
					//	"into heap with distance: [%d]",
					//	pstListTraverser->vertexNum, newDistance);

					pstBinoNode = binoInsert(&pstBinoHeap,
							pstListTraverser->vertexNum, newDistance);
					if (NULL_PTR == pstBinoNode)
					{
						destroyBinoHeap(pstBinoHeap, nodeInBinoHeap);
						myLog(ERROR, "binoInsert failed!");
						return INVALID_TIME;
					}
					
					gTestEV++;
					
					//myLog(DEBUG, "Inserted vertex: [%d] at [%x]",
					//	pstListTraverser->vertexNum, pstBinoNode);
						
					//myLog(DEBUG, "Root: [%x], root vertex: [%d], "
					//	"root distance: [%d]",
					//	pstBinoHeap, pstBinoHeap->vertex, pstBinoHeap->key);
										
					nodeInBinoHeap[pstListTraverser->vertexNum]
									= (int *)pstBinoNode;
				}
				else
				{
					//myLog(DEBUG, "Vertex [%d] is present in heap at "
					//	"address: [%x] with distance: [%d]. "
					//	"Distance changed?: [%d]",
					//	pstBinoNode->vertex, pstBinoNode,
					//	pstBinoNode->key, hasDistanceChanged);
					/* If the distance has not changed, continue */
					if (FALSE == hasDistanceChanged)
					{
						pstListTraverser = pstListTraverser->next;
						continue;
					}
					
					//myLog(DEBUG, "Performing decrease key on vertex: [%d] "
					//	"distance: [%d] and newDistance: [%d]",
					//	pstBinoNode->vertex, pstBinoNode->key, newDistance);
					
					if (OK != binoDecreaseKey(&pstBinoHeap,
						pstBinoNode, newDistance))
					{
						myLog(ERROR, "binoDecreaseKey failed!");
						destroyBinoHeap(pstBinoHeap, nodeInBinoHeap);
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
			pstBinoNode = NULL_PTR;
		}
	}
	
	time = ((clock() - startTime) / CLOCKS_PER_MSEC);
	
	myLog(INFO, "Time taken by BinoDijkstra: [%d] msec.", time);
	
	return time;
}
