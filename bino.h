/***************************************************************************************************/
// BINO.H
// Contains enums and macros for binomial scheme
// Author: Aakash Shah
// Email: aakashv2.1@gmail.com
// Year: 2011
/***************************************************************************************************/

#include "header.h"
#include <malloc.h>

/* Binomial Heaps are collection of binomial trees
The insert can be done in O(1) time
Meld can be done in O(1) time
Remove min can be done in O(n) time -- Amortized is O(log(n)) */

#define MAX_BINO_DEGREE 10

/* Node defination of a Binomial Tree */
typedef struct BinoHeap
{
	int key; //for us the key is edge cost
	int vertex; //from this we can derive (sourceVertex, vertex) = key;
	int degree; // No of children it has
	struct BinoHeap *parent; // For root its NULL
	struct BinoHeap *child; // Points only to any one child
	struct BinoHeap *next; //Useful for circular linked list. Points to itself if alone
}BinoHeap;
